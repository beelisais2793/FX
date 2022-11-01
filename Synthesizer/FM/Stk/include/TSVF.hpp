#pragma once

#include "SoundAlchemy.hpp"
    
namespace SoundAlchemy::Filters
{    
    template<typename T>
    struct TChamberlinSVF 
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
        TChamberlinSVF(T sr, T fc, T q) {        
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
        T GetLowPassOut()  { return L; }
        T GetHighPassOut() { return H; }
        T GetBandPassOut() { return B; }
        T GetNotchOUt()    { return N; }
    };

    template<typename T>
    struct TStateVariableFilter
    {
        /*
        cutoff = cutoff freq in Hz
        fs = sampling frequency //(e.g. 44100Hz)
        f = 2 sin (pi * cutoff / fs) //[approximately]
        q = resonance/bandwidth [0 < q <= 1]  most res: q=1, less: q=0
        low = lowpass output
        high = highpass output
        band = bandpass output
        notch = notch output
        */
        T cutoff,scale,fs,low,high,band,notch;
            
        TStateVariableFilter( T Fs, T Fc, T Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(T F) { cutoff = F; }
        void setResonance(T R) { scale = 1.25*(1.0-R); }
        T Tick(T I, T A = 1, T X=0, T Y=0)
        {
            Undenormal denormal;
            T f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
            //--beginloop
            I = tanhify(I);
            low = low + f * band;
            high = scale * I - low - scale*band;
            band = f * high + band;
            notch = high + low;
            return low;
        }
        T GetLowPassOut()  { return low; }
        T GetHighPassOut() { return high; }
        T GetBandPassOut() { return band; }
        T GetNotchOUt()    { return notch; }

    };
}