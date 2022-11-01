#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel2zlm = fv3::irmodel2zlm_;    
    #else
    using fv3_irmodel2zlm = fv3::irmodel2zlm_f;    
    #endif
    struct IRModel2zlm : public MonoFXProcessorPlugin<fv3_irmodel2zlm>
    {
        IRModel2zlm(DspFloatType * ir, long size) : MonoFXProcessorPlugin<fv3_irmodel2zlm>()
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