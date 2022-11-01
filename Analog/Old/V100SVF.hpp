#pragma once
#include <cmath>
#include "clip_functions.h"

namespace V100
{
    template<typename T>
    struct SVF
    {
        /*
        //Input/Output
        I - input sample
        L - lowpass output sample
        B - bandpass output sample
        H - highpass output sample
        N - notch output sample
        F1 - Frequency control parameter
        Q1 - Q control parameter
        D1 - delay associated with bandpass output
        D2 - delay associated with low-pass output
        */
        T x,L,B,H,N,F1,Q1,D1,D2;
        T Fc,Fs,R;
        SVF(T sr, T fc, T q) {        
            Fc = fc;
            Fs = sr;
            R  = q;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(T fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(T r) { R = r; Q1 = 1.0-r; }

        T Tick(T I, T A=1, T X=0, T Y=0)
        {    
            Undenormal denormal;
            x = I;
            // algorithm
            // loop
            L = D2 + F1 * D1;
            H = I - L - Q1*D1;
            B = F1 * H + D1;
            N = H + L;

            // store delays
            D1 = B;
            D2 = L;

            // outputs
            //L,H,B,N
            return L;
        }

        void Process(size_t n, T * I, T * O)
        {
            for(size_t i = 0; i < n; i++)
                O[i] = Tick(I[i]);
        }
    };
}