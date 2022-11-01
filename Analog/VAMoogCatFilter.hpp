#pragma once

#include "SoundObject.hpp"

namespace Analog::MoogFilters::MoogCat
{
    struct MoogCat : public FilterProcessor
    {
        
        DspFloatType fc = 800.0f; //cutoff frequency
        DspFloatType res = 0.0f; //resonance
        DspFloatType Fs = 44100.0f; //sampling frequency
        DspFloatType g = tan(M_PI * fc / Fs); //frequency warping factor
        DspFloatType r_cat = 1.064f; //filter type value    

        //vector of states
        static constexpr size_t numStates = 4;
        std::vector<std::array<DspFloatType, numStates>> state;

        MoogCat() : FilterProcessor() {

        }
        void setCutoff(DspFloatType f) {
            fc = f;        
        }
        void setResonance(DspFloatType r) {
            res = r;
        }

        enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,		
        };
        void setPort(int port, DspFloatType v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                setCutoff(v);
                break;
            case PORT_RESONANCE:
                setResonance(v);
                break;			
            }
        }
        DspFloatType processSample(DspFloatType input, size_t channel) noexcept {
            auto& s = state[channel];
            const DspFloatType g_2 = g*g;
            const DspFloatType g_3 = g_2*g;
            const DspFloatType g_4 = g_3*g;
            const DspFloatType r_cat_2 = r_cat*r_cat;
            const DspFloatType val1 = res*g_4*r_cat_2;
            const DspFloatType val2 = g_3*r_cat;
            const DspFloatType val3 = g*r_cat;
            const DspFloatType val4 = g_2*r_cat_2;
            const DspFloatType val5 = 2*val3 + 1;
            const DspFloatType val6 = g_2 + val5;
            const DspFloatType val7 = g*val6;
            const auto den = (4*val1 + g_4 + 4*val2 + 4*val4 + 2*g_2 + 4*val3 + 1);
            DspFloatType out = (g_3*s[0] - g_2*(val5)*s[1] + val7*s[2] - (2*val2 + 4*val4 + g_2 + 4*val3 + 1)*s[3])/den;
            const DspFloatType a = -(val1*4 + g_4 + 4*val2 + 4*val4 - 1)*s[0] + 2*g*(res*val4*4 + val6)*s[1] - 8*g_2*res*r_cat_2*s[2] + 8*g*res*r_cat_2*(2*val3+1)*s[3] + val7*2*input;
            const DspFloatType b = - 2*val7*s[0] + (-val1*4 - g_4 + 4*val4 + 4*val3 + 1)*s[1] + 8*g_3*res*r_cat_2*s[2] - 8*g_2*res*r_cat_2*(2*val3+1)*s[3] - g_2*(val6)*2*input;
            const DspFloatType c = 2*g_2*s[0] - g*(val5)*2*s[1] - (val1*4 + g_4 + 4*g_3*r_cat + 4*val4 - 1)*s[2] + 2*g*(res*val4*4 + val6)*s[3] + 2*g_3*input;
            const DspFloatType d = -2*g_3*s[0] + g_2*(val5)*2*s[1] - val7*2*s[2] + (-val1*4 - g_4 + 4*val4 + 4*val3 + 1)*s[3] - 2*g_4*input;
            s[0] = a/den;
            s[1] = b/den;
            s[2] = c/den;
            s[3] = d/den;
            return out;
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType F = fc;
            DspFloatType R = res;

            setCutoff(F*fabs(X));
            setResonance(R*fabs(Y));
            DspFloatType out = processSample(I,0);
            return A*out;
        }
    };
}
