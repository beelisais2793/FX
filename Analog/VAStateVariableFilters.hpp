#pragma once
#include <cmath>
#include "FX/ClipFunctions.hpp"

namespace Analog::Filters::StateVariable
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

    // it should be oversampled
    struct AnalogSVF : public FilterProcessor
    {    
        DspFloatType fc,fs,q,K;
        DspFloatType lp,hp,bp,ubp,shelf,notch,apf,peak;
        DspFloatType z1,z2;

        AnalogSVF(DspFloatType Fs, DspFloatType Fc, DspFloatType Q) : FilterProcessor() {
            fc = Fc;
            fs = Fs;
            q = Q;
            if(q == 0) q = 0.001;
            z1 = z2 = 0.0;
            K = 1;
        }
        void setCutoff(DspFloatType f) {
            fc = f;
        }
        void setQ(DspFloatType Q) {   
            if(q <= 0.0) q = 0.001;        
            q = Q;
        }
        enum {
            PORT_CUTOFF,
            PORT_RESONANCE
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_RESONANCE: setQ(v); break;
            }
        }
 
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            DspFloatType wd = 2*M_PI*fc;
            DspFloatType T  = 1/fs;
            DspFloatType wa = (2/T)*tan(wd*T/2);
            DspFloatType g  = wa*T/2;
            DspFloatType xn = I;
            DspFloatType  R  = 1.0/(2*(q));
            
            hp = (xn - (2*R+g)*z1 - z2) / (1.0 + 2*R*g + g*g);
            bp = g*hp + z1;
            lp = g*bp + z2;        
            // not sure these work right yet
            ubp = 2 * R * bp;
            // dont know exactly what K is it's not explained
            shelf = xn + 2*K*R*bp;
            notch = xn - 2*R*bp;
            apf   = xn - 4*R*bp;
            peak  = lp - hp;

            // delay feedback
            z1 = g*hp + bp;
            z2 = g*bp + lp;
            
            return lp;
        }
        void ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void InplaceProcess(size_t n, DspFloatType * input) {
            ProcessBlock(n,input,input);
        }
    };

    struct StereoSVF : public StereoFilterProcessor
    {
        AnalogSVF left,right;

        StereoSVF(DspFloatType sr) : 
            StereoFilterProcessor(),
            left(sr,1000.0f,0.5),
            right(sr,1000.0f,0.5)
        {

        }
        void setCutoff(DspFloatType f) {
            left.setCutoff(f);
            right.setCutoff(f);
        }
        void setQ(DspFloatType q) {
            left.setQ(q);
            right.setQ(q);
        }
        void ProcessBlock(size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            left.ProcessBlock(n,inputs[0],outputs[0]);
            right.ProcessBlock(n,inputs[1],outputs[1]);

        }
    };

    struct StateVariableFilter : public FilterProcessor
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
        DspFloatType cutoff,scale,fs,low,high,band,notch;
            
        StateVariableFilter(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) : FilterProcessor() {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(DspFloatType F) { cutoff = F; }
        void setResonance(DspFloatType R) { scale = 1.25*(1.0-R); }
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
 
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X=0, DspFloatType Y=0)
        {
            Undenormal denormal;
            DspFloatType f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
            //--beginloop
            //I = tanhify(I);
            low = low + f * band;
            high = scale * I - low - scale*band;
            band = f * high + band;
            notch = high + low;
            return low;
        }
    };

}
