#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_lfo = fv3::lfo_;
    #else
    using fv3_lfo = fv3::lfo_f;
    #endif

    struct ArcLFO : public GeneratorProcessorPlugin<fv3_lfo>
    {
        DspFloatType f;
        ArcLFO(DspFloatType freq, DspFloatType fs = sampleRate) : GeneratorProcessorPlugin<fv3_lfo>()
        {        
            f = freq;    
            this->setFreq(freq,fs);
        }        
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {            
            DspFloatType arc = this->process();                     
            return A*arc;
        }
    };   
}