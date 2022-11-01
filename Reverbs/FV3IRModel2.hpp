#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel2 = fv3::irmodel2_;    
    #else
    using fv3_irmodel2 = fv3::irmodel2_f;    
    #endif

    struct IRModel2 : public StereoFXProcessorPlugin<fv3_irmodel2>
    {
        IRModel2(DspFloatType * irL, DspFloatType * irR, long size) : StereoFXProcessorPlugin<fv3_irmodel2>()
        {
            this->loadImpulse(irL, irR, size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}