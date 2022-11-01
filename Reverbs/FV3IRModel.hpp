#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_irmodel1 = fv3::irmodel1_;    
    #else
    using fv3_irmodel1 = fv3::irmodel1_f;    
    #endif
    struct IRModel1 : public StereoFxProcessorPlugin<fv3_irmodel1>
    {
        IRModel1(DspFloatType * ir, long size) : StereoFxProcessorPlugin<fv3_irmodel1>()
        {
            this->loadImpulse(ir,size);
        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {            
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}