#pragma once
#include <cmath>
#include "FX/ClipFunctions.hpp"

namespace Filters::StateVariable
{
    struct ChamberlinLowPassSVF 
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
        DspFloatType Fc,Fs,R,G;
        ChamberlinLowPassSVF(DspFloatType sr, DspFloatType fc, DspFloatType q) {        
            Fc = fc;
            Fs = sr;
            R  = q;
            G  = 1;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(DspFloatType fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(DspFloatType r) { R = r; Q1 = 1.0-r; }
        void setGain(DspFloatType g) { G = g; }
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
    struct ChamberlinHighPassSVF
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
        DspFloatType Fc,Fs,R,G;
        ChamberlinHighPassSVF(DspFloatType sr, DspFloatType fc, DspFloatType q) {        
            Fc = fc;
            Fs = sr;
            R  = q;
            G  = 1;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(DspFloatType fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(DspFloatType r) { R = r; Q1 = 1.0-r; }
        void setGain(DspFloatType g) { G = g; }
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
            return H;
        }
    };
    struct ChamberlinBandPassSVF
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
        DspFloatType Fc,Fs,R,G;
        ChamberlinBandPassSVF(DspFloatType sr, DspFloatType fc, DspFloatType q) {        
            Fc = fc;
            Fs = sr;
            R  = q;
            G  = 1;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(DspFloatType fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(DspFloatType r) { R = r; Q1 = 1.0-r; }
        void setGain(DspFloatType g) { G = g; }
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
            return B;
        }
    };
    struct ChamberlinNotchSVF
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
        DspFloatType Fc,Fs,R,G;
        ChamberlinNotchSVF(DspFloatType sr, DspFloatType fc, DspFloatType q) {        
            Fc = fc;
            Fs = sr;
            R  = q;
            G  = 1;
            Q1 = 1.5*q + 0.5;
            F1 = 2 * std::sin( M_PI * Fc / Fs );
            x=L=B=H=N=F1=Q1=D1=D2 = 0;
        }
        void setCutoff(DspFloatType fc) { Fc = fc; F1 = 2 * std::sin( M_PI * Fc / Fs );}
        void setResonance(DspFloatType r) { R = r; Q1 = 1.0-r; }
        void setGain(DspFloatType g) { G = g; }
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
            return N;
        }
    };
    
    struct StateVariableLowPassFilter
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
            
        StateVariableLowPassFilter(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(DspFloatType F) { cutoff = F; }
        void setResonance(DspFloatType R) { scale = 1.25*(1.0-R); }
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
    struct StateVariableHighPassFilter
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
            
        StateVariableHighPassFilter(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(DspFloatType F) { cutoff = F; }
        void setResonance(DspFloatType R) { scale = 1.25*(1.0-R); }
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
            return high;
        }
    };
    struct StateVariableBandPassFilter
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
            
        StateVariableBandPassFilter(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(DspFloatType F) { cutoff = F; }
        void setResonance(DspFloatType R) { scale = 1.25*(1.0-R); }
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
            return band;
        }
    };
    struct StateVariableNotchFilter
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
            
        StateVariableNotchFilter(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
        }
        void setCutoff(DspFloatType F) { cutoff = F; }
        void setResonance(DspFloatType R) { scale = 1.25*(1.0-R); }
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
            return notch;
        }
    };
}
