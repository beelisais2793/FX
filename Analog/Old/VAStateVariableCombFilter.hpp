#pragma once

struct StateVariableCombFilter
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
    DelayLines::fircombfilter comb;

    StateVariableCombFilter(float Fc, float Fs, float Q) : comb(0.995,0.001*sampleRate)
    {
        scale = Q;
        cutoff= Fc;
        fs    = Fs;
        low=high=band=notch=0;
    }
    void setCutoff(float F) { cutoff = clamp(F,10,sampleRate/2); }
    void setResonance(float R) { scale = 2*(1.0-clamp(std::log(1+R)/std::log(2),0.5,0.99)); Q=R; }
    float Tick(float I, float A = 1, float X=1, float Y=1)
    {
        Undenormal denormal;
        float f     = std::sin(2 * M_PI * (fabs(X)*cutoff/fs));        
        //--beginloop
        I = comb.Tick(Distortion::tanhify(I));
        float s = scale;
        scale *= fabs(Y);
        low = low + f * band;
        high = scale * I - low - scale*band;
        band = f * high + band;
        notch = high + low;
        scale = s;
        return Distortion::serpent_curve((A*5*Q)*low);
    }
};
