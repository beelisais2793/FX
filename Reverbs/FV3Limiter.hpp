#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{

    #ifdef DSPFLOATDOUBLE
    using fv3_limitmodel = fv3::limitmodel_;    
    #else    
    using fv3_limitmodel = fv3::limitmodel_f;    
    #endif
    struct Limiter : public StereoFXProcessorPlugin<fv3_limitmodel>
    {
        Limiter(DspFloatType fs, DspFloatType rms, DspFloatType look, DspFloatType lookratio, DspFloatType attack, DspFloatType release, DspFloatType thresh, DspFloatType ceil)
         : StereoFXProcessorPlugin<fv3_limitmodel>()
        {
            this->setSampleRate(fs);
            this->setRMS(rms);
            this->setLookahead(look);
            this->setLookaheadRatio(lookratio);
            this->setAttack(attack);
            this->setRelease(release);
            this->setThreshold(thresh);
            this->setCeiling(ceil);
        }
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out)
        {
            this->processreplace(in[0],in[1],out[0],out[1],n);
        }
    };
}