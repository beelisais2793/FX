#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodels = fv3::irmodels_;    
    #else
    using fv3_irmodels = fv3::irmodels_f;    
    #endif
    struct IRModels : public StereoFXProcessorPlugin<fv3_irmodels>
    {
        IRModels(DspFloatType * irL, DspFloatType * irR, long size) : StereoFXProcessorPlugin<fv3_irmodels>()
        {
            this->loadImpulse(irL,irR,size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}    