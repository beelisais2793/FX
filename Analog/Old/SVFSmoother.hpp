
#pragma once
#include <cmath>

struct SVFSmoother
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
    float cutoff,scale,fs,low,high,band,notch;
    float target;

    SVFSmoother(float Fc, float Fs, float Q) {
        scale = Q;
        cutoff= Fc;
        fs    = Fs;
        low=high=band=notch=0;
        target = 1.0f;
    }
    void setTarget(float t) { target = t; }
    // F=1/S
    // 1 / (smoothingTimeInMs * 0.001f)
    void setCutoff(float F) { cutoff = F; }
    void setResonance(float R) { scale = 1.25*(1.0-R); }
    
    float lowPassOutput() { return low; }
    float highPassOutput() { return high; }
    float bandPassOutput() { return band; }
    float notchOutput() { return nothc; }

    float Tick()
    {
        Undenormal denormal;
        float f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
        //--beginloop
        float I = target;
        low = low + f * band;
        high = scale * I - low - scale*band;
        band = f * high + band;
        notch = high + low;
        return low;
    }
};
