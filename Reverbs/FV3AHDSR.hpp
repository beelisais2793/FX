#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_adhsr = fv3::ahdsr_;
    #else
    using fv3_adhsr = fv3::ahdsr_f;
    #endif

    struct AHDSR : public GeneratorProcessorPlugin<fv3_adhsr>
    {
        
        DspFloatType Samples,A,D,H,S,R;
        DspFloatType gate=0;
        AHDSR(long samples, DspFloatType a, DspFloatType h, DspFloatType d, DspFloatType s, DspFloatType r, bool loop=false) 
        : GeneratorProcessorPlugin<fv3_adhsr>()
        {         
            Samples = samples;
            A = a;
            D = d;
            H = h;
            S = s;
            R = r;           
            this->setRAHDSR(samples,a,h,d,s,r);
            this->setLoopMode(loop);
        }
        void noteOn() { gate = 1.0; }
        void noteOff() { gate = 0.0; }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {                     
            DspFloatType out = A*this->process(gate);            
            return out;
        }
    };   
}