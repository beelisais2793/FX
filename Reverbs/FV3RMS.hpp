#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_rms = fv3::rms_;        
    #else
    using fv3_rms = fv3::rms_f;
    #endif
    struct RMS : public FunctionProcessorPlugin<fv3_rms>
    {
        RMS(long size) : FunctionProcessorPlugin<fv3_rms>()
        {
            this->setsize(size);
        }
        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            return A*this->process(I);
        }
    };
}