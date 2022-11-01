#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel2zl = fv3::irmodel2zl_;    
    #else
    using fv3_irmodel2zl = fv3::irmodel2zl_f;    
    #endif
    struct IRModel2zl : public StereoFXProcessorPlugin<fv3_irmodel2zl>
    {
        IRModel2zl(DspFloatType * irL, DspFloatType *irR, long size) : StereoFXProcessorPlugin<fv3_irmodel2zl>()
        {
            this->loadImpulse(irL, irR, size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}