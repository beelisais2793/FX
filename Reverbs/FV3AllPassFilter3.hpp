#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_allpass3 = fv3::allpass3_;
    #else
    using fv3_allpass3 = fv3::allpass3_f;
    #endif

    struct FV3AllPassFilter3 : public FilterProcessorPlugin<fv3_allpass3>
    {
        FV3AllPassFilter3(size_t size1, size_t size2, size_t size3) : FilterProcessorPlugin<fv3_allpass3>()
        {
            this->setsize(size1,size2,size3);                
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {                
            DspFloatType r = this->process(I);        
            return A*r;
        }        
    };
}