#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_delay = fv3::delay_;
    #else
    using fv3_delay = fv3::delay_f;
    #endif
    struct FeedbackDelay : public FunctionProcessorPlugin<fv3_delay>
    {
        FeedbackDelay(size_t size, DspFloatType fbk) : FunctionProcessorPlugin<fv3_delay>()
        {
            this->setsize(size);
            this->setfeedback(fbk);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {
            DspFloatType f=this->getfeedback();
            this->setfeedback(f*X);            
            DspFloatType out = this->process_wf(I);
            this->setfeedback(f);
            return out;            
        }
    };
}