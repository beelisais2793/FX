#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_stenh = fv3::stenh_;        
    #else
    using fv3_stenh = fv3::stenh_f;
    #endif
    struct StereoEnhancer : public StereoFXProcessorPlugin<fv3_stenh>
    {
        StereoEnhancer() : StereoFXProcessorPlugin<fv3_stenh>()
        {

        }

        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}