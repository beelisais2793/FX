#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{   
    #ifdef DSPFLOATDOUBLE
    using fv3_lfo = fv3::lfo_;        
    #else
    using fv3_dl_gd_largeroom = fv3::dl_gd_largeroom_f;    
    using fv3_gd_largeroom = fv3::gd_largeroom_f;    
    #endif
    struct LFO : public GeneratorProcessorPlugin<fv3::lfo_>
    {
        DspFloatType f;
        LFO(DspFloatType freq, DspFloatType fs = sampleRate) : GeneratorProcessorPlugin<fv3::lfo_>()
        {        
            f = freq;    
            this->setFreq(freq,fs);
        }        
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {                        
            DspFloatType out = this->process(I);            
            return A*out;
        }
    };   
}