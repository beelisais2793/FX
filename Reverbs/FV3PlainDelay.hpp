#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{   
    #ifdef DSPFLOATDOUBLE
    using fv3_delay = fv3::delay_;        
    #else
    using fv3_delay = fv3::delay_f;
    #endif
    struct PlainDelay : public FunctionProcessorPlugin<fv3_delay>
    {
        PlainDelay(size_t size, float fbk) : FunctionProcessorPlugin<fv3_delay>()
        {
            this->setsize(size);
            this->setfeedback(fbk);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {            
            DspFloatType out = this->process(I);         
            return out;            
        }
    };
}