
#pragma once
#include <cmath>

namespace Analog::Filters::SVFSmoother
{
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
        double cutoff,scale,fs,low,high,band,notch;
        double target;
        enum {
            LOW,
            HIGH,
            BAND,
            NOTCH
        };
        int type = LOW;
        SVFSmoother(double Fc, double Fs, double Q) {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
            target = 1.0f;
        }
        void setTarget(double t) { target = t; }
        // F=1/S
        // 1 / (smoothingTimeInMs * 0.001f)
        void setCutoff(double F) { cutoff = F; }
        void setResonance(double R) { scale = 1.25*(1.0-R); }
        
        double lowPassOutput() { return low; }
        double highPassOutput() { return high; }
        double bandPassOutput() { return band; }
        double notchOutput() { return notch; }

        double Tick()
        {
            Undenormal denormal;
            double f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
            //--beginloop
            double I = target;
            low = low + f * band;
            high = scale * I - low - scale*band;
            band = f * high + band;
            notch = high + low;
            switch(type)
            {
            case LOW: return low;
            case HIGH: return high;
            case BAND: return band;
            }
            return notch;
        }
    };
}