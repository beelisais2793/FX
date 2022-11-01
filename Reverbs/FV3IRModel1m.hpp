#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel1m = fv3::irmodel1m_;    
    #else
    using fv3_irmodel1m = fv3::irmodel1m_f;    
    #endif
    
    struct IRModel1m : public MonoFXProcessorPlugin<fv3_irmodel1m>
    {
        IRModel1m(DspFloatType * ir, long size) : MonoFXProcessorPlugin<fv3_irmodel1m>()
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