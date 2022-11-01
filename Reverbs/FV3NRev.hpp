#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_nrev = fv3::nrev_;        
    using fv3_nrevb = fv3::nrevb_;        
    #else
    using fv3_nrev = fv3::nrev_f;
    using fv3_nrevb = fv3::nrevb_f;        
    #endif
    struct NRev : public StereoFXProcessorPlugin<fv3_nrev>
    {
        NRev() : StereoFXProcessorPlugin<fv3_nrev>()
        {
            
        }
                
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };

    struct NRevB : public StereoFXProcessorPlugin<fv3_nrevb>
    {
        NRevB() : StereoFXProcessorPlugin<fv3_nrevb>()
        {

        }
        
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}    