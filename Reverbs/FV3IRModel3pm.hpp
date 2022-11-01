#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel3pm = fv3::irmodel3pm_;    
    #else
    using fv3_irmodel3pm = fv3::irmodel3pm_f;    
    #endif
    struct IRModel3pm : public MonoFXProcessorPlugin<fv3_irmodel3pm>
    {
        IRModel3pm(DspFloatType * ir, long size) : MonoFXProcessorPlugin<fv3_irmodel3pm>()
        {
            this->loadImpulse(ir,size);
        }

        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out)
        {
            memcpy(out,in,n*sizeof(DspFloatType));
            this->processreplace(out,n);
        }
    };
}