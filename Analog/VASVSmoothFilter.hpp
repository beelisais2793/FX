#pragma once

#include "FX/CSmoothFilters.hpp"

namespace Analog::Filters::SmoothSVF
{
    struct SmoothSVF
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
        DspFloatType Ftarget,Qtarget,cutoff,scale,fs,low,high,band,notch;
        FX::Filters::Smoothers::CSmoothFilter lpSmooth,hpSmooth,bpSmooth,npSmooth,cutSmooth,resSmooth;
        enum {
            LOW,
            HIGH,
            BAND,
            NOTCH,
            LPHMORPH,
            LPBMORPH,
            LPNMORPH,
        };
        int type = LOW;

        SmoothSVF(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) :
        lpSmooth(Fs,1/0.01), hpSmooth(Fs,1/0.01), bpSmooth(Fs,1/0.01), npSmooth(Fs,1/0.01), cutSmooth(Fs,1/0.01),resSmooth(Fs,1/0.01) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            Ftarget = cutoff;
            Qtarget = Q;
            low=high=band=notch=0;
        }
        DspFloatType operator()(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return Tick(I,A,X,Y);
        }
 
        void setCutoff(DspFloatType F) { Ftarget = F; }
        void setResonance(DspFloatType R) { Qtarget = R*0.995; }
        DspFloatType getLP() { return lpSmooth.process(low); }
        DspFloatType getHP() { return hpSmooth.process(high); }
        DspFloatType getBP() { return bpSmooth.process(band); }
        DspFloatType getNotch() { return npSmooth.process(notch); }    

        DspFloatType LPHMorph(DspFloatType f) { 
            DspFloatType L = getLP();
            return L + f*(L+getHP());
        }
        DspFloatType LPBMorph(DspFloatType f) { 
            DspFloatType L = getLP();
            return L + f*(L+getBP());
        }
        DspFloatType LPNMorph(DspFloatType f) { 
            DspFloatType L = getLP();
            return L + f*(L+getNotch());
        }
        
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X=1, DspFloatType Y=1)
        {
            Undenormal denormal;
            cutoff =cv2freq(cutSmooth.process(fabs(Ftarget*X)));        
            scale = 1.5*(1.0-(resSmooth.process(fabs(Qtarget*Y))));
            if(scale == 0.0f) scale = 0.001f;
            DspFloatType f     = std::sin(2 * M_PI * cutoff/fs);        
            //--beginloop
            //I = Distortion::tanhify(I);
            
            low = low + f * band;
            high = scale * I - low - scale*band;
            band = f * high + band;
            notch = high + low;
            
            switch(type)
            {
            case LOW: return A/scale*getLP();
            case HIGH: return A/scale*getHP();
            case BAND: return A/scale*getBP();
            case NOTCH: return A/scale*getNotch();
            case LPHMORPH: return A/scale*LPHMorph();
            case LPBMORPH: return A/scale*LPBMorph();
            case LPNMORPH: return A/scale*LPNMorph();
            }
            
            return A/scale*lpSmooth.process(low);
        }
    };
}