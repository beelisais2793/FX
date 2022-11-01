#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    struct IIRLR2 : public FilterProcessorPlugin<fv3::iir_lr2_>
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
        DspFloatType Fc,Fs;

        IIRLR2(int type, DspFloatType fc, DspFloatType fs) : FilterProcessorPlugin<fv3::iir_lr2_>()
        {            
            setFilter(type,fc,fs);
        }
        void setFilter(int type, DspFloatType fc, DspFloatType fs)
        {
            Fc = fc;            
            Fs = fs;
            this->type = type;
            switch(type)
            {
                case LPF: this->setLPF(fc,fs); break;
                case HPF: this->setHPF(fc,fs); break;                
            }            
        }
        void setCutoff(DspFloatType f) {
            Fc = f;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType out = I;            
            out = this->process(I); 
            return A*out;
        }
    };   
}