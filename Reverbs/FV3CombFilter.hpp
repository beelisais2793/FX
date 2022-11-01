#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_comb = fv3::comb_;
    #else
    using fv3_comb = fv3::comb_f;
    #endif
    struct CombFilter : public FilterProcessorPlugin<fv3_comb>
    {
        CombFilter(size_t size, DspFloatType damp, DspFloatType fbk) : FilterProcessorPlugin<fv3_comb>()
        {
            this->setsize(size);
            this->setdamp(damp);
            this->setfeedback(fbk);
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType damp = this->getdamp();
            DspFloatType fbk  = this->getfeedback();
            this->setfeedback(fbk*X);
            this->setdamp(damp*Y);
            DspFloatType out = this->process(I);
            this->setfeedback(fbk);
            this->setdamp(damp);
            return A*out;
        }
    };

    struct ModulationCombFilter : public FilterProcessorPlugin<fv3::combm_>
    {
        ModulationCombFilter(size_t size, size_t mod, DspFloatType damp, DspFloatType fbk) : FilterProcessorPlugin<fv3::combm_>()
        {
            this->setsize(size,mod);
            this->setdamp(damp);
            this->setfeedback(fbk);
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType damp = this->getdamp();
            DspFloatType fbk  = this->getfeedback();
            this->setfeedback(fbk*X);
            this->setdamp(damp*Y);
            DspFloatType out = this->process(I,A);
            this->setfeedback(fbk);
            this->setdamp(damp);
            return out;
        }
    };
}