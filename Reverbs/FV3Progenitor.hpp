#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_progenitor = fv3::progenitor_;
    using fv3_progenitor2 = fv3::progenitor2_;
    #else
    using fv3_progenitor = fv3::progenitor_f;
    using fv3_progenitor2 = fv3::progenitor2_f;
    #endif
    struct Progenitor : public StereoFXProcessorPlugin<fv3_progenitor>
    {
        Progenitor() : StereoFXProcessorPlugin<fv3_progenitor>()
        {
            
        }        
        
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };

    struct Progenitor2 : public StereoFXProcessorPlugin<fv3_progenitor2>
    {
        Progenitor2() : StereoFXProcessorPlugin<fv3_progenitor2>()
        {
            
        }        
        
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}