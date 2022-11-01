#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel3w = fv3::irmodel3w_;    
    #else
    using fv3_irmodel3w = fv3::irmodel3w_f;    
    #endif
    struct IRModel3w : public StereoFXProcessorPlugin<fv3_irmodel3w>
    {
        IRModel3w(DspFloatType * ir, long size) : StereoFXProcessorPlugin<fv3_irmodel3w>()
        {
            this->loadImpulse(ir,size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}