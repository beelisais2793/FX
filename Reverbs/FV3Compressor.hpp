#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
   #ifdef DSPFLOATDOUBLE
   using fv3_compressor = fv3::compmodel_;
   #else
   using fv3_compressor = fv3::compmodel_f;
   #endif

    struct Compressor : public StereoFXProcessorPlugin<fv3_compressor>
    {
         Compressor(DspFloatType sr=sampleRate) : StereoFXProcessorPlugin<fv3_compressor>()
         {
            this->setSampleRate(sr);
         }         
         void ProcessBlock(size_t n, DspFloatType ** input, DspFloatType **output)
         {            
            this->processreplace(input[0],input[1],output[0],output[1],n);
         }
    };
}