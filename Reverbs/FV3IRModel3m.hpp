#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel3m = fv3::irmodel3m_;    
    #else
    using fv3_irmodel3m = fv3::irmodel3m_f;    
    #endif
    struct IRModel3m : public MonoFXProcessorPlugin<fv3_irmodel3m>
    {
        IRModel3m(DspFloatType * ir, long size) : MonoFXProcessorPlugin<fv3_irmodel3m>()
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