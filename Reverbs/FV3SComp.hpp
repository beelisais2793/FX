#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{

    #ifdef DSPFLOATDOUBLE
    using fv3_scomp = fv3::scomp_;        
    #else
    using fv3_scomp = fv3::scomp_f;
    #endif
    struct SComp : public FunctionProcessorPlugin<fv3_scomp>
    {
        SComp() : FunctionProcessorPlugin<fv3_scomp>()
        {
            
        }        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            return A*this->process(I);
        }
    };

}    