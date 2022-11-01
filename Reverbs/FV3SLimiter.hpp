#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_slimit = fv3::slimit_;        
    #else
    using fv3_slimit = fv3::slimit_f;
    #endif
    struct SLimit : public FunctionProcessorPlugin<fv3_slimit>
    {
        SLimit() : FunctionProcessorPlugin<fv3_slimit>()
        {
            
        }
        SLimit(long rms, long lookahead, DspFloatType lookratio, DspFloatType atk, DspFloatType rel, DspFloatType thresh, DspFloatType ceil) 
        : FunctionProcessorPlugin<fv3_slimit>()        
        {
            this->setRMS(rms);
            this->setLookahead(lookahead);
            this->setLookaheadRatio(lookratio);
            this->setAttack(atk);
            this->setRelease(rel);
            this->setThreshold(thresh);
            this->setCeiling(ceil);            
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            return A*this->process(I);
        }
    };

}    