#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_iir_1st = fv3::iir_1st_;    
    #else
    using fv3_iir_1st = fv3::iir_1st_f;    
    #endif
    struct IIR1 : public FilterProcessorPlugin<fv3_iir_1st>
    {
        enum {
            LPF_BW,
            HPF_BW,
            LPF_A,
            HPF_A,
            LSF_A,
            HSF_A,
            HPFWLFS_A,
            LPF_C,
            HPF_C,
            POLE,
            ZERO,
            POLE_LPF,
            POLE_HPF,
            ZERO_LPF,
            ZERO_HPF
        };
        enum 
        {
            DIRECT_FORM_I,
            DIRECT_FORM_II
        };
        int form = DIRECT_FORM_I;
        int type = LPF_A;
        DspFloatType Fc,Fs,Fc2;

        IIR1(int type, DspFloatType fc, DspFloatType fs, DspFloatType fc2=0) : FilterProcessorPlugin<fv3_iir_1st>()
        {            
            setFilter(type,fc,fs);
        }
        void setFilter(int type, DspFloatType fc, DspFloatType fs, DspFloatType fc2=0)
        {
            Fc = fc;
            Fc2 = fc2;
            Fs = fs;
            this->type = type;
            switch(type)
            {
                case LPF_BW: this->setLPF_BW(fc,fs); break;
                case HPF_BW: this->setHPF_BW(fc,fs); break;
                case LPF_A: this->setLPF_A(fc,fs); break;
                case HPF_A: this->setHPF_A(fc,fs); break;
                case LSF_A: this->setLSF_A(fc,fc2,fs); break;
                case HSF_A: this->setHSF_A(fc,fc2,fs); break;
                case HPFWLFS_A: this->setHPFwLFS_A(fc,fs); break;
                case LPF_C: this->setLPF_C(fc,fs); break;
                case HPF_C: this->setHPF_C(fc,fs); break;
                case POLE_LPF: this->setPoleLPF(fc,fs); break;
                case POLE_HPF: this->setPoleHPF(fc,fs); break;
                case ZERO_LPF: this->setZeroLPF(fc,fs); break;
                case ZERO_HPF: this->setZeroHPF(fc,fs); break;
            }            
        }
        void setCutoff(DspFloatType f) {
            Fc = f;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType out = I;
            this->setFilter(type,Fc*fabs(X),Fs,Fc2*fabs(Y));
            out = this->process(I); 
            this->setFilter(type,Fc,Fs,Fc2);
            return A*out;
        }
    };   
}