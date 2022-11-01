#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_revmodel = fv3::revmodel_;        
    #else
    using fv3_revmodel = fv3::revmodel_f;
    #endif
    
    struct RevModel : public StereoFXProcessorPlugin<fv3_revmodel>
    {
        RevModel() : StereoFXProcessorPlugin<fv3_revmodel>()
        {
            
        }        
        
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}