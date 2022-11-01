#pragma once

#include <cmath>
#include <cstdlib>

namespace Analog::Filters::DinkFilter
{
    class DinkyFilter : public FilterProcessor {
    public:
        enum FilterMode {
            FILTER_MODE_LOWPASS = 0,
            FILTER_MODE_HIGHPASS,
            FILTER_MODE_BANDPASS,
            kNumFilterModes
        };
        DinkyFilter() :
        cutoff(0.99),
        resonance(0.0),
        mode(FILTER_MODE_LOWPASS),
        buf0(0.0),
        buf1(0.0),
        buf2(0.0),
        buf3(0.0)
        {
            calculateFeedbackAmount();
        };

        double process(double inputValue);
        void setCutoff(double newCutoff) { cutoff = newCutoff; calculateFeedbackAmount(); };
        void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); };
        void setFilterMode(FilterMode newMode) { mode = newMode; }

        double Tick(double in, double A=1, double X=0, double Y=0) { 
            return A*process(in); 
        }
        void   Process(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
        void   Process(float * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick(samples[i]);
        }
        
    private:
        double cutoff;
        double resonance;
        FilterMode mode;
        double feedbackAmount;
        void calculateFeedbackAmount() { feedbackAmount = resonance + resonance/(1.0 - cutoff); }
        double buf0;
        double buf1;
        double buf2;
        double buf3;
    };

    inline double DinkyFilter::process(double inputValue) {
        buf0 += cutoff * (inputValue - buf0 + (buf0-buf1)*feedbackAmount);
        buf1 += cutoff * (buf0 - buf1);
        buf2 += cutoff * (buf1 - buf2);
        buf3 += cutoff * (buf2 - buf3);
        switch (mode) {
            case FILTER_MODE_LOWPASS:
                return buf3;
            case FILTER_MODE_HIGHPASS:
                return inputValue - buf0;
            case FILTER_MODE_BANDPASS:
                return buf0 - buf3;
            default:
                return 0.0;
        }
    }
}