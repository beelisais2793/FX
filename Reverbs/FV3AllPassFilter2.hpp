#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_allpass2 = fv3::allpass2_;
    #else
    using fv3_allpass2 = fv3::allpass2_f;
    #endif
    struct FV3AllPassFilter2 : public FilterProcessorPlugin<fv3_allpass2>
    {
        FV3AllPassFilter2(size_t size1, size_t size2, DspFloatType fbk1, DspFloatType fbk2, DspFloatType dcy1, DspFloatType dcy2) 
        : FilterProcessorPlugin<fv3_allpass2>()
        {
            this->setsize(size1,size2);        
            this->setfeedback1(fbk1);
            this->setfeedback2(fbk2);
            this->setdecay1(dcy1);        
            this->setdecay2(dcy2);        
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {                    
            DspFloatType r = this->process(I);            
            return A*r;
        }        
    };
}