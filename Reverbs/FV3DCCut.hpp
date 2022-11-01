#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_dccut = fv3::dccut_;
    #else
    using fv3_dccut = fv3::dccut_f;
    #endif

    struct DCCut : public FilterProcessorPlugin<fv3_dccut>
    {        
        DCCut() : FilterProcessorPlugin<fv3_dccut>()
        {            
            
        }        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {                        
            DspFloatType out = this->process(I);            
            return A*out;
        }
    };   
}