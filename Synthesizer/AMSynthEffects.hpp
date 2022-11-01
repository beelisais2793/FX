#pragma once

#include <math.h>
#include <iostream>

#define AT 0.0001		// attack time in seconds
#define RT 0.5			// release time in seconds
#define THRESHOLD 0.9	// THRESHOLD>0 !!

namespace Synth::AMSynth
{
    class Distortion
    {
    public:
        void	SetCrunch(DspFloatType);
        void	ProcessBlock(size_t n, DspFloatType *in, DspFloatType * out);
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            DspFloatType x, s;
            x = I;
            if(x<0) s=-1; else s=1;
            x*=s*Y;
            DspFloatType c = crunch*X;
            x = pow (x, c < 0.01f ? 0.01f : c);
            return A*x*s;
        }
    private:
        DspFloatType crunch;
    };

    class SoftLimiter
    {
    public:
        void	SetSampleRate(int rate);
        void	ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out);
    private:
        double xpeak, attack, release, thresh;
    };

    void Distortion::SetCrunch	(DspFloatType value)
    {
        crunch = 1-value;
    }

    void Distortion::ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out)
    {
        DspFloatType x, s;
        for (size_t i=0; i < n; i++)
        {
            x = in[i];
            if(x<0) s=-1; else s=1;
            x*=s;
            DspFloatType c = crunch;
            x = pow (x, c < 0.01f ? 0.01f : c);
            out[i] = x*s;
        }
    }


    void
    SoftLimiter::SetSampleRate	(int rate)
    {
        xpeak=0;
        attack=1-exp(-2.2/(AT*(DspFloatType)rate));
        release=1-exp(-2.2/(RT*(DspFloatType)rate));
        thresh=(DspFloatType)log(THRESHOLD); // thresh in linear scale :)
    }

    void
    SoftLimiter::ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
    {
        double x;
        size_t i;
        for (i=0; i < n; i++)
        {
            x = fabs(in[0][i]) + fabs (in[1][i]);
            
            if (x>xpeak) xpeak=(1-release)*xpeak + attack*(x-xpeak);
            else xpeak=(1-release)*xpeak;
                
            if (xpeak>0){
    //			x = 1/xpeak;
                x = log(xpeak);
                x -= thresh;
                if (x<0) x = 0;
    //			x *= ((1/Ratio)-1); 
                /* 1<= Ratio < infinity = compressor
                ratio=infinity = limiter
                0 < ratio < 1 = expander
                0 = gate
                */
                else x *= -1;
                x = exp(x);
            } else x=1;
            
            // apply gain
            out[0][i] = in[0][i]*x;
            out[1][i] = in[1][i]*x;
            
            
        }
    }
}    