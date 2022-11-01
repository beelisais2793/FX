#pragma once 
#include "Undenormal.hpp"
#include <cmath>

namespace Filters::SVF
{
struct StateVariableFilter
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
        
    StateVariableFilter(float Fc, float Q, float Fs) {
        scale = Q;
        cutoff= Fc;
        fs    = Fs;
        low=high=band=notch=0;
    }
    void setCutoff(float F) { cutoff = F; }
    void setResonance(float R) { scale = 1.25*(1.0-R); }
    float Tick(float I, float A = 1, float X=0, float Y=0)
    {
        Undenormal denormal;
        float f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
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