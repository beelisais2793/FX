
#pragma once
#include <cmath>

namespace Analog::Filters::SVFSmoother
{
    struct SVFSmoother : public FilterProcessor
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
        DspFloatType target;
        enum {
            LOW,
            HIGH,
            BAND,
            NOTCH
        };
        int type = LOW;
        SVFSmoother(DspFloatType Fc, DspFloatType Fs, DspFloatType Q) : FilterProcessor() {
            scale = Q;
            cutoff= Fc;
            fs    = Fs;
            low=high=band=notch=0;
            target = 1.0f;
        }
        void setTarget(DspFloatType t) { target = t; }
        // F=1/S
        // 1 / (smoothingTimeInMs * 0.001f)
        void setCutoff(DspFloatType F) { cutoff = F; }
        void setResonance(DspFloatType R) { scale = 1.25*(1.0-R); }
        
        DspFloatType lowPassOutput() { return low; }
        DspFloatType highPassOutput() { return high; }
        DspFloatType bandPassOutput() { return band; }
        DspFloatType notchOutput() { return notch; }
        enum {
            PORT_TARGET,
            PORT_CUTOFF,
            PORT_RESONANCE,
            PORT_LP,
            PORT_HP,
            PORT_BP,
            PORT_NOTCH
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_RESONANCE: setResonance(v); break;
                case PORT_LP: type = LP; break;
                case PORT_HP: type = HP; break;
                case PORT_BP: type = BP; break;
                case PORT_NOTCH: type = NOTCH; break;               
                case PORT_TARGET: setTarget(v); break;
            }
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            Undenormal denormal;
            DspFloatType f     = 2 * std::sin(2 * M_PI * cutoff/fs);        
            //--beginloop
            DspFloatType I = target;
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