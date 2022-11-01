#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel3 = fv3::irmodel3_;    
    #else
    using fv3_irmodel3 = fv3::irmodel3_f;    
    #endif
    struct IRModel3 : public StereoFXProcessorPlugin<fv3_irmodel3>
    {
        IRModel3(DspFloatType * irL, DspFloatType * irR, long size) : StereoFXProcessorPlugin<fv3_irmodel3>()
        {
            this->loadImpulse(irL,irR,size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}