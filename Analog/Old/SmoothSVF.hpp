#pragma once

namespace Analog
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
    float Ftarget,Qtarget,cutoff,scale,fs,low,high,band,notch;
    Filters::CSmoothFilter lpSmooth,hpSmooth,bpSmooth,npSmooth,cutSmooth,resSmooth;

    SmoothSVF(float Fc, float Fs, float Q) :
     lpSmooth(Fs,1/0.01), hpSmooth(Fs,1/0.01), bpSmooth(Fs,1/0.01), npSmooth(Fs,1/0.01), cutSmooth(Fs,1/0.01),resSmooth(Fs,1/0.01) {
        scale = Q;
        cutoff= Fc;
        fs    = Fs;
        Ftarget = cutoff;
        Qtarget = Q;
        low=high=band=notch=0;
    }
    float operator()(float I, float A=1, float X=0, float Y=0) {
        return Tick(I,A,X,Y);
    }
    void setCutoff(float F) { Ftarget = F; }
    void setResonance(float R) { Qtarget = R*0.995; }
    float getLP() { return lpSmooth.process(low); }
    float getHP() { return hpSmooth.process(high); }
    float getBP() { return bpSmooth.process(band); }
    float getNotch() { return npSmooth.process(notch); }    

    float LPHMorph(float f) { 
        float L = getLP();
        return L + f*(L+getHP());
    }
    float LPBMorph(float f) { 
        float L = getLP();
        return L + f*(L+getBP());
    }
    float LPNMorph(float f) { 
        float L = getLP();
        return L + f*(L+getNotch());
    }
    
    float Tick(float I, float A = 1, float X=1, float Y=1)
    {
        Undenormal denormal;
        cutoff =cv2freq(cutSmooth.process(fabs(Ftarget*X)));        
        scale = 1.5*(1.0-(resSmooth.process(fabs(Qtarget*Y))));
        if(scale == 0.0f) scale = 0.001f;
        float f     = std::sin(2 * M_PI * cutoff/fs);        
        //--beginloop
        I = Distortion::tanhify(I);
        
        low = low + f * band;
        high = scale * I - low - scale*band;
        band = f * high + band;
        notch = high + low;
                
        return A/scale*lpSmooth.process(low);
    }
};
}