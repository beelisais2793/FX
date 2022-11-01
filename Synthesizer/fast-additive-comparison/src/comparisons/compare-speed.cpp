#include <iostream>
#include <cstddef>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include "nanobench.h"
#include "../implementations/phase-to-amplitude.hpp"
#include "../implementations/oscillator-bank.hpp"
#include "../implementations/recursive.hpp"
#include "xsimd/xsimd.hpp"

namespace xs = xsimd;

namespace gfac = goldenrockefeller::fast_additive_comparison;

using std::cout;
using std::vector;
using std::size_t;
using std::ostringstream;
using std::iota;
using std::for_each;

using float_avx_t = xs::batch<float, xs::avx>;
using double_avx_t = xs::batch<double, xs::avx>;

using gfac::OscillatorBank;
using gfac::SimpleExactSineOscillator;
using gfac::SineOscillator;
using FloatCosCalc = gfac::ExactCosineCalculator<float>;
using DoubleCosCalc = gfac::ExactCosineCalculator<double>;
using LookupDoubleCosCalc = gfac::LookupCalculator<double>;
using gfac::ApproxCos14Calculator;
using gfac::ApproxCos10Calculator;
using gfac::IdentityCalculator;




template <typename GeneratorT>
void do_regular_bench(ankerl::nanobench::Bench* bench, char const* name, size_t chunk_size, size_t n_oscs) {
    using sample_type = typename GeneratorT::sample_type;

    GeneratorT gen(n_oscs);
    vector<sample_type> output(chunk_size);

    vector<sample_type> freqs(n_oscs);
    iota(freqs.begin(), freqs.end(), 0.);
    for_each(freqs.begin(), freqs.end(), [&] (sample_type& freq) {freq /= (2 * n_oscs);});

    bench->run(name, [&]() {
        for (size_t osc_id = 0; osc_id < n_oscs; ++osc_id) {
            gen.reset_osc(osc_id, freqs[osc_id], 1., 0.);
        }
        gen.progress_and_add(output.begin(), output.end());
    });
}

void do_all_regular_benches(size_t chunk_size, size_t n_oscs) {
    ankerl::nanobench::Bench bench;

    ostringstream title_stream;
    title_stream << "All Regular Bench. Chunck Size: " << chunk_size << "; Num of Oscs: " << n_oscs;
    bench.title(title_stream.str());

    bench.minEpochIterations(100);

    do_regular_bench<OscillatorBank<SimpleExactSineOscillator<double>>>(
        &bench, "Phase-to-Amplitude Simple Double", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double, 1,DoubleCosCalc>>>(
        &bench, "Phase-to-Amplitude Exact Double-1", chunk_size, n_oscs
    );

     do_regular_bench<OscillatorBank<SineOscillator<double, double, 4,DoubleCosCalc>>>(
        &bench, "Phase-to-Amplitude Exact Double-4", chunk_size, n_oscs
    );

     do_regular_bench<OscillatorBank<SineOscillator<double, double, 16,DoubleCosCalc>>>(
        &bench, "Phase-to-Amplitude Exact Double-16", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 4, IdentityCalculator>>>(
        &bench, "Phase-to-Amplitude Identity Double-AVX-4", chunk_size, n_oscs
    );


    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 1,DoubleCosCalc>>>(
        &bench, "Phase-to-Amplitude Exact Double-AVX-1", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 4,DoubleCosCalc>>>(
        &bench, "Phase-to-Amplitude Exact Double-AVX-4", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double, 1, ApproxCos10Calculator>>>(
        &bench, "Phase-to-Amplitude Approx 10-deg Double-1", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double, 16, ApproxCos10Calculator>>>(
        &bench, "Phase-to-Amplitude Approx 10-deg Double-16", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 1, ApproxCos10Calculator>>>(
        &bench, "Phase-to-Amplitude Approx 10-deg Double-AVX-1", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 4, ApproxCos10Calculator>>>(
        &bench, "Phase-to-Amplitude Approx 10-deg Double-AVX-4", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 4, ApproxCos14Calculator>>>(
        &bench, "Phase-to-Amplitude Approx 14-deg Double-AVX-4", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<SineOscillator<double, double_avx_t, 4,LookupDoubleCosCalc>>>(
        &bench, "Phase-to-Amplitude Lookup Double-AVX-4", chunk_size, n_oscs
    );

    do_regular_bench<OscillatorBank<gfac::MagicCircleOscillator<double, double_avx_t, 4>>>(
        &bench, "Recursive Double-AVX-4", chunk_size, n_oscs
    );
    
}
 



int main() {

    do_all_regular_benches(50000, 1);
    // do_all_regular_benches(1024, 1);
    // do_all_regular_benches(1, 1);
  
    return 0;
}