#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel3wm = fv3::irmodel3wm_;    
    #else
    using fv3_irmodel3wm = fv3::irmodel3wm_f;    
    #endif
    struct IRModel3wm : public MonoFXProcessorPlugin<fv3_irmodel3wm>
    {
        IRModel3wm(DspFloatType * ir, long size) : MonoFXProcessorPlugin<fv3_irmodel3wm>()
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