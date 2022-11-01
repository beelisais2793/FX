#pragma once

#include <cmath>

namespace Analog::Filters::Moog::MoogFilter2
{
    struct MoogFilter2
    {
        // Moog 24 dB/oct resonant lowpass VCF
        // References: CSound source code, Stilson/Smith CCRMA paper.
        // Modified by paul.kellett@maxim.abel.co.uk July 2000

        float f, p, q;             //filter coefficients
        float b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
        float t1, t2;              //temporary buffers
        float fs,fc,res;

        // Set coefficients given frequency & resonance [0.0...1.0]
        MoogFilter2(float sr, float cutoff, float r)
        {
            fs = sr;
            fc = cutoff/sr;
            res = r;
            calc();
            b0=b1=b2=b3=b4=0;
        }
        void calc()
        {
            q = 1.0f - fc;
            p = fc + 0.8f * fc * q;
            f = p + p - 1.0f;
            q = res * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
        }
        void setCutoff(float f) { fc = f/fs; }
        void setResonance(float r) { res = r; }
        float Tick(float I, float A=1, float X = 0, float Y=0)
        {
            Undenormal denormals;
            calc();
            float in = I - q*b4;       
            t1 = b1; //std::tanh(b1);  
            b1 = (in + b0) * p - b1 * f;
            t2 = b2; //std::tanh(b2);  
            b2 = (b1 + t1) * p - b2 * f;
            t1 = b3; //std::tanh(b3); 
            b3 = (b2 + t2) * p - b3 * f;
            b4 = (b3 + t1) * p - b4 * f;
            b4 = b4 - b4 * b4 * b4 * 0.166667f;
            b0 = in;
            return b4;
        }
    };
}