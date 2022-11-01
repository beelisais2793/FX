#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
      #ifdef DSPFLOATDOUBLE
    using fv3_sweep = fv3::sweep_;        
    #else
    using fv3_sweep = fv3::sweep_f;
    #endif

    struct SweepGenerator : public FunctionProcessorPlugin<fv3_sweep>
    {
         SweepGenerator(DspFloatType sr) : FunctionProcessorPlugin<fv3_sweep>()
         {
            this->setSampleRate(sr);
         }
         DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
         {
            return this->process(I);
         }
    };
}