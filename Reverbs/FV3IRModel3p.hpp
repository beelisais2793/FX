#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel3p = fv3::irmodel3p_;    
    #else
    using fv3_irmodel3p = fv3::irmodel3p_f;    
    #endif
    struct IRModel3p : public StereoFXProcessorPlugin<fv3_irmodel3p>
    {
        IRModel3p(DspFloatType * irL, DspFloatType * irR, long size) : StereoFXProcessorPlugin<fv3_irmodel3p>()
        {
            this->loadImpulse(irL,irR,size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}