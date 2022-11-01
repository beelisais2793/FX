#pragma once

#include <cmath>
#include "SoundObject.hpp"

namespace Analog::MoogFilters::MoogFilter3
{
    struct MoogVCF : public FilterProcessor
    {
        //Init
        DspFloatType fc;
        DspFloatType fs;
        DspFloatType res;
        DspFloatType out1,out2,out3,out4;
        DspFloatType in1,in2,in3,in4;
        
        MoogVCF(DspFloatType sr, DspFloatType Fc, DspFloatType R) : FilterProcessor()
        {
            fs = sr;
            fc = Fc/sr;
            res= R;
            out1=out2=out3=out4=0;
            in1=in2=in3=in4=0;
        }
        void setCutoff(DspFloatType f) { fc = f/fs; }
        void setResonance(DspFloatType r) { res = r; }
        enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,			
        };
        void setPort(int port, DspFloatType v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                setCutoff(v);
                break;
            case PORT_RESONANCE:
                setResonance(v);
                break;		
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            DspFloatType f = fc * 1.16;
            DspFloatType fb = res * (1.0 - 0.15 * f * f);
            DspFloatType input = I;
            input -= out4 * fb;
            input *= 0.35013 * (f*f)*(f*f);
            out1 = input + 0.3 * in1 + (1 - f) * out1; // Pole 1
            in1  = input;
            out2 = out1 + 0.3 * in2 + (1 - f) * out2;  // Pole 2
            in2  = out1;
            out3 = out2 + 0.3 * in3 + (1 - f) * out3;  // Pole 3
            in3  = out2;
            out4 = out3 + 0.3 * in4 + (1 - f) * out4;  // Pole 4
            in4  = out3;
            return out4;
        }
    };
}