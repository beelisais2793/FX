#pragma once
#include <cmath>
#include "ClipFunctions.hpp"

namespace Analog::Filters::SVF
{
    struct ChamberlinSVF : public FilterProcessor
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
        DspFloatType x,L,B,H,N,F1,Q1,D1,D2;
        DspFloatType Fc,Fs,R;

        ChamberlinSVF(DspFloatType sr, DspFloatType fc, DspFloatType q) : FilterProcessor() {        
            Fc = fc;
            Fs = sr;
            R  = q;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(DspFloatType fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(DspFloatType r) { R = r; Q1 = 1.0-r; }

        enum {
            PORT_CUTOFF,
            PORT_RESONANCE
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_RESONANCE: setResonance(v); break;
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
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
    };
}