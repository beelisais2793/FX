#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_efilter = fv3::efilter_;
    #else
    using fv3_efilter = fv3::efilter_f;
    #endif

    struct EFilter : public StereoFXProcessorPlugin<fv3_efilter>
    {
        enum {
            LPF,
            HPF
        };
        enum 
        {
            DIRECT_FORM_I,
            DIRECT_FORM_II
        };
        int form = DIRECT_FORM_I;
        int type = LPF;
        DspFloatType Fc;

        EFilter(int type, DspFloatType fc) : StereoFXProcessorPlugin<fv3_efilter>()
        {            
            setFilter(type,fc);
        }
        void setFilter(int type, DspFloatType fc)
        {
            Fc = fc;                        
            this->type = type;
            switch(type)
            {
                case LPF: this->setLPF(fc); break;
                case HPF: this->setHPF(fc); break;                
            }            
        }
        void setCutoff(DspFloatType f) {
            Fc = f;
        }
        DspFloatType Tick(DspFloatType iL, DspFloatType iR, DspFloatType &L, DspFloatType &R, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {            
            this->setFilter(type,Fc*fabs(X));
            L = this->processL(iL);
            R = this->processR(iR);
            this->setFilter(type,Fc);
            return A*0.5*(L+R);
        }
    };       
}