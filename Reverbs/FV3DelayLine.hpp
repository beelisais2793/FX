#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_delayline = fv3::delayline_;
    #else
    using fv3_delayline = fv3::delayline_f;
    #endif
    struct DelayLine : public FunctionProcessorPlugin<fv3_delayline>
    {
        DspFloatType feedback;
        DspFloatType depth = 0.01;
        DspFloatType mix = 0.5;
        DelayLine(size_t size, DspFloatType fbk) : FunctionProcessorPlugin<fv3_delayline>()
        {
            this->setsize(size);            
            feedback = fbk;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {
            int idx = Y * this->getsize() * depth;
            DspFloatType ft = feedback;
            feedback *= X;
            DspFloatType r1 = this->at(idx);
            DspFloatType out = this->process(I + feedback*r1);            
            feedback = ft;
            return mix*I + (1.0-mix)*out;
        }
    };
}