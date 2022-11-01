#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_delaym = fv3::delaym_;        
    #else
    using fv3_delaym = fv3::delaym_f;
    #endif
    struct ModulationDelay : public FunctionProcessorPlugin<fv3_delaym>
    {
        ModulationDelay(size_t size, size_t modsize, DspFloatType fbk) : FunctionProcessorPlugin<fv3_delaym>()
        {
            this->setsize(size,modsize);            
            this->setfeedback(fbk);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=0)
        {
            DspFloatType f=this->getfeedback();
            this->setfeedback(f*X);            
            DspFloatType out = this->process(I,Y);
            this->setfeedback(f);
            return out;            
        }
    };
}