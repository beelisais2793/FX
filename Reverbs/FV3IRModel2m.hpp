#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel2m = fv3::irmodel2m_;    
    #else
    using fv3_irmodel2m = fv3::irmodel2m_f;    
    #endif
    struct IRModel2m : public MonoFXProcessorPlugin<fv3_irmodel2m>
    {
        IRModel2m(DspFloatType * ir, long size) : MonoFXProcessorPlugin<fv3_irmodel2m>()
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