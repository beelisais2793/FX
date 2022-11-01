#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <numeric>

#include "../implementations/common.hpp"
#include "../implementations/phase-to-amplitude.hpp"
#include "../implementations/recursive.hpp"

#include "xsimd/xsimd.hpp"

namespace xs = xsimd;
namespace gfac = goldenrockefeller::fast_additive_comparison;


using std::cout;
using std::vector;
using std::sin;
using std::cos;
using std::abs;
using std::exp2;
using std::log10;
using std::iota;
using std::transform;
using float_avx_t = xs::batch<float, xs::avx>;
using double_avx_t = xs::batch<double, xs::avx>;
using std::ostringstream;
using std::invalid_argument;
using gfac::tau;

using FloatCosCalc = gfac::ExactCosineCalculator<float>;
using DoubleCosCalc = gfac::ExactCosineCalculator<double>;
using gfac::ApproxCos10Calculator;

using LookupDoubleCosCalc = gfac::LookupCalculator<double>;

template<typename T>
T clamp(T v, T lo, T hi) {
    return std::max(std::min(v, hi), lo);
}

struct AmplitudeAccuracyRecord {
    double freq;
    double snr_db;
    double abs_gain_db;
};

struct AnalysisResult {
    AmplitudeAccuracyRecord worst_snr_record;
    AmplitudeAccuracyRecord worst_abs_gain_record;
};

double dot(
    vector<double>::const_iterator it_a,
    vector<double>::const_iterator it_b,
    size_t size
) {
    double total = 0.;

    for (size_t i = 0; i < size; i++) {
        total += *it_a * *it_b;
        it_a++; 
        it_b++; 
    }

    return total;
}

struct LeastSquaresSinCosModel {
    double freq;
    vector<double> sin;
    vector<double> cos;
    vector<double> transformed_sin;
    vector<double> transformed_cos;

    LeastSquaresSinCosModel(double freq, size_t size) :
        freq(freq),
        sin(size),
        cos(size),
        transformed_sin(size),
        transformed_cos(size)
    {
        vector<double> steps(size);

        iota(steps.begin(), steps.end(), 0.);

        // Calculate sin and cos.
        transform(steps.cbegin(), steps.cend(), this->sin.begin(), [=](double x){return std::sin(tau<double>() * freq * x);});
        transform(steps.cbegin(), steps.cend(), this->cos.begin(), [=](double x){return std::cos(tau<double>() * freq * x);});


        // The following calculates X.T @ X for the least squares calculation. (@ is matrix multiplicaiton)
        // X.T @ X = [[ss, sc], [sc, cc]] where X = [this->sin, this->cos].T
        auto ss = dot(this->sin.cbegin(), this->sin.cbegin(), size);
        auto sc = dot(this->sin.cbegin(), this->cos.cbegin(), size);
        auto cc = dot(this->cos.cbegin(), this->cos.cbegin(), size);

        // The following calculates the inverse of (X.T @ X) for the least squares calculation.
        auto det = ss * cc - sc * sc;
        auto i00 = cc / det;
        auto i01 = -sc / det;
        auto i11 = ss / det;

        // The following calculates the transformed sin and cos as (X.T @ X)^-1 @ X.T for the least squares calculation.
        for (size_t i = 0; i < size; i++) {
            this->transformed_sin[i] = i00 * this->sin[i] + i01 * this->cos[i];
            this->transformed_cos[i] = i01 * this->sin[i] + i11 * this->cos[i];
        }    
    }
};


AmplitudeAccuracyRecord amplitude_accuracy_for_slice(
    vector<double>::const_iterator signal_it,  
    const LeastSquaresSinCosModel& sin_cos_model,
    size_t slice_size
) {
    double sin_dot_signal = dot(sin_cos_model.transformed_sin.begin(), signal_it, slice_size);
    double cos_dot_signal = dot(sin_cos_model.transformed_cos.begin(), signal_it, slice_size);

    double power_gain = sin_dot_signal * sin_dot_signal + cos_dot_signal * cos_dot_signal;

    double fitted_signal_power = 0;
    double residual_power = 0.;

    for (size_t i = 0; i < slice_size; i++) {
        auto fitted_sample = sin_dot_signal * sin_cos_model.sin[i] + cos_dot_signal * sin_cos_model.cos[i];
        auto residual = signal_it[i] - fitted_sample;
        fitted_signal_power += fitted_sample * fitted_sample;
        residual_power += residual * residual;
    }

    AmplitudeAccuracyRecord record;
    record.freq = sin_cos_model.freq;
    record.snr_db = 10 * log10(fitted_signal_power / residual_power);
    record.abs_gain_db = abs(10 * log10(power_gain));

    return record;
}

vector<AmplitudeAccuracyRecord> rolling_amplitude_accuracy_analyis_for_freq(const vector<double>& signal, double freq) {
    double osc_size = double(signal.size());
    double raw_analysis_period_as_double = 2. / (abs(freq) + 1. / osc_size);
    size_t analysis_period = clamp(size_t(raw_analysis_period_as_double), size_t(4), signal.size());

    auto sin_cos_model = LeastSquaresSinCosModel(freq, analysis_period);
    
    vector<AmplitudeAccuracyRecord> accuracy_records;

    for (auto it = signal.begin(); it + analysis_period < signal.end(); it++) {
        accuracy_records.push_back(amplitude_accuracy_for_slice(it, sin_cos_model, analysis_period));
    }

    return accuracy_records;
}

AnalysisResult signal_analysis(const vector<double>& signal, double freq){
    AnalysisResult result;
    vector<AmplitudeAccuracyRecord> current_amplitude_accuracy_records;

    current_amplitude_accuracy_records = rolling_amplitude_accuracy_analyis_for_freq(signal, freq);
    auto min_record = 
    result.worst_snr_record = *(
        min_element(
            current_amplitude_accuracy_records.cbegin(),
            current_amplitude_accuracy_records.cend(),
            [](const AmplitudeAccuracyRecord& a, const AmplitudeAccuracyRecord& b){return a.snr_db < b.snr_db;}
        )
    );
    result.worst_abs_gain_record = *(
        max_element(
            current_amplitude_accuracy_records.cbegin(),
            current_amplitude_accuracy_records.cend(),
            [](const AmplitudeAccuracyRecord& a, const AmplitudeAccuracyRecord& b){return a.abs_gain_db < b.abs_gain_db;}
        )
    );

    return result;
}


    // for (double freq : freqs) {
    //     if (freq == freqs[0]) {
    //         continue;
    //     }
    //     current_amplitude_accuracy_records = rolling_amplitude_accuracy_analyis_for_freq(signal, freq);
    //     worst_snr_record = (
    //         min_element(
    //             current_amplitude_accuracy_records.cbegin(),
    //             current_amplitude_accuracy_records.cend(),
    //             [](const AmplitudeAccuracyRecord& a, const AmplitudeAccuracyRecord& b){return a.snr_db < b.snr_db;}
    //         )
    //     )
    //     worst_abs_gain_record = (
    //         max_element(
    //             current_amplitude_accuracy_records.cbegin(),
    //             current_amplitude_accuracy_records.cend(),
    //             [](const AmplitudeAccuracyRecord& a, const AmplitudeAccuracyRecord& b){return a.abs_gain_db < b.abs_gain_db;}
    //         )
    //     )
    //     if (worst_snr_record.snr_db < result.worst_snr_record.snr_db) {
    //         result.worst_snr_record = worst_snr_record;
    //     }
    //     if (worst_abs_gain_record.abs_gain_db < result.worst_abs_gain_record.abs_gain_db) {
    //         result.worst_abs_gain_record = worst_abs_gain_record;
    //     }
    // }

    // return result;

template<typename OscillatorT>
AnalysisResult oscillator_analysis(const vector<double>& freqs, size_t analysis_len) {
    using sample_type = typename OscillatorT::sample_type;

    AnalysisResult result;
    vector<AnalysisResult> results_by_freqs;

    OscillatorT oscillator(sample_type(0.), sample_type(1.), sample_type(0.));

    for (auto freq : freqs) {
        vector<sample_type> raw_oscillator_signal(analysis_len, 0.);
        oscillator.reset(sample_type(freq), sample_type(1.), sample_type(0.));
        oscillator.progress_and_add(raw_oscillator_signal.begin(), raw_oscillator_signal.end());

        vector<double> signal(analysis_len);
        for (size_t i = 0; i < analysis_len; i++) {
            signal[i] = double(raw_oscillator_signal[i]);
        }

        results_by_freqs.push_back(signal_analysis(signal, freq));
    }

    auto worst_snr_result = *(
        min_element(
            results_by_freqs.cbegin(),
            results_by_freqs.cend(),
            [](const AnalysisResult& a, const AnalysisResult& b){return a.worst_snr_record.snr_db < b.worst_snr_record.snr_db;}
        )
    );


    auto worst_abs_gain_result = *(
        max_element(
            results_by_freqs.cbegin(),
            results_by_freqs.cend(),
            [](const AnalysisResult& a, const AnalysisResult& b){return a.worst_abs_gain_record.abs_gain_db < b.worst_abs_gain_record.abs_gain_db;}
        )
    );

    result.worst_snr_record = worst_snr_result.worst_snr_record;
    result.worst_abs_gain_record = worst_abs_gain_result.worst_abs_gain_record;

    return result;
}

int main() {

    vector<double> freqs(15);

    for(size_t i = 0; i < freqs.size(); i++) {
        freqs[i] = 0.45 / exp2(double(i));
    }
    

    auto result = oscillator_analysis<gfac::MagicCircleOscillator<double, double_avx_t, 4>>(freqs, 50000);
    
    // auto result = oscillator_analysis<gfac::SimpleExactSineOscillator<double>>(freqs, 50000);

    cout << "SNR (db): " << result.worst_snr_record.snr_db << " at " << result.worst_snr_record.freq << " cycles/sample \n"; 
    cout << "Absolute Gain (db): " << result.worst_abs_gain_record.abs_gain_db << " at " << result.worst_abs_gain_record.freq << " cycles/sample \n"; 

    return 0;
}

