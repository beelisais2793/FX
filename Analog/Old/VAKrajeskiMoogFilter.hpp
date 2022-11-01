#pragma once

#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::Moog
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // The Krajtski 5
    ///////////////////////////////////////////////////////////////////////////////////////////
    struct KrajeskiMoog 
    {
        KrajeskiMoog(double sr) : sampleRate(sr)
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
        
        double Tick(double I, double A=1, double X=1, double Y=1) {
            Undenormal denormal;
            state[0] = std::tanh(drive * (I - 4 * gRes * (state[4] - gComp * I)));
            for(int i = 0; i < 4; i++)
            {
                state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
                delay[i] = state[i];
            }
            return A * state[4];
        }
        void SetResonance(double r)
        {
            resonance = r;
            gRes = resonance * (1.0029 + 0.0526 * wc - 0.926 * std::pow(wc, 2) + 0.0218 * std::pow(wc, 3));
        }
        void SetCutoff(double c)
        {
            cutoff = c;
            wc = 2 * MOOG_PI * cutoff / sampleRate;
            g = 0.9892 * wc - 0.4342 * std::pow(wc, 2) + 0.1381 * std::pow(wc, 3) - 0.0202 * std::pow(wc, 4);
        }

        void setDrive(double d) {
            drive = d;
        }

        double GetResonance() { return resonance; }
        double GetCutoff() { return cutoff; }
    
        double state[5];
        double delay[5];
        double wc; // The angular frequency of the cutoff.
        double g; // A derived parameter for the cutoff frequency
        double gRes; // A similar derived parameter for resonance.
        double gComp; // Compensation factor.
        double drive; // A parameter that controls intensity of nonlinearities.
        double sampleRate;
        double resonance;
        double cutoff;        
    };
}