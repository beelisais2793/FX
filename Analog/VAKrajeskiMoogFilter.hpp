#pragma once

#include <cmath>
#include <cstdint>
#include "Undenormal.hpp"
#include "SoundObject.hpp"

namespace Analog::Filters::Moog
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // The Krajtski 5
    ///////////////////////////////////////////////////////////////////////////////////////////
    struct KrajeskiMoog : public FilterProcessor
    {
        KrajeskiMoog(DspFloatType sr) : FilterProcessor(),sampleRate(sr)
        {
            memset(state, 0, sizeof(state));
            memset(delay, 0, sizeof(delay));

            drive = 1.0;
            gComp = 1.0;

            SetCutoff(1000.0f);
            SetResonance(0.1f);
        }

        virtual ~KrajeskiMoog() { }

        void Process(size_t n, float * samples, float * output)
        {
            Undenormal denormal;
            for (uint32_t s = 0; s < n; ++s)
            {
                state[0] = std::tanh(drive * (samples[s] - 4 * gRes * (state[4] - gComp * samples[s])));

                for(int i = 0; i < 4; i++)
                {
                    state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
                    delay[i] = state[i];
                }
                output[s] = state[4];
            }
        }

        void Process(size_t n, float * samples)
        {
            Process(n,samples,samples);
        }
        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            Undenormal denormal;
            DspFloatType c = GetCutoff();
            DspFloatType r = GetResonance();
            SetCutoff(c * fabs(X));
            SetResonance(r * fabs(Y));
            state[0] = std::tanh(drive * (I - 4 * gRes * (state[4] - gComp * I)));
            for(int i = 0; i < 4; i++)
            {
                state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
                delay[i] = state[i];
            }
            SetCutoff(c);
            SetResonance(r);
            return A * state[4];
        }
        void SetResonance(DspFloatType r)
        {
            resonance = r;
            gRes = resonance * (1.0029 + 0.0526 * wc - 0.926 * std::pow(wc, 2) + 0.0218 * std::pow(wc, 3));
        }
        void SetCutoff(DspFloatType c)
        {
            cutoff = c;
            wc = 2 * M_PI * cutoff / sampleRate;
            g = 0.9892 * wc - 0.4342 * std::pow(wc, 2) + 0.1381 * std::pow(wc, 3) - 0.0202 * std::pow(wc, 4);
        }

        void setDrive(DspFloatType d) {
            drive = d;
        }

        DspFloatType GetResonance() { return resonance; }
        DspFloatType GetCutoff() { return cutoff; }
        
        enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,
            PORT_DRIVE,
        };
        void setPort(int port, DspFloatType v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                SetCutoff(v);
                break;
            case PORT_RESONANCE:
                SetResonance(v);
                break;
            case PORT_DRIVE:
                setDrive(v);
                break;
            }
        }
        DspFloatType state[5];
        DspFloatType delay[5];
        DspFloatType wc; // The angular frequency of the cutoff.
        DspFloatType g; // A derived parameter for the cutoff frequency
        DspFloatType gRes; // A similar derived parameter for resonance.
        DspFloatType gComp; // Compensation factor.
        DspFloatType drive; // A parameter that controls intensity of nonlinearities.
        DspFloatType sampleRate;
        DspFloatType resonance;
        DspFloatType cutoff;        
    };
}