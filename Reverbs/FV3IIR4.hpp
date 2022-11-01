#pragma once

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_iir_lr4 = fv3::iir_1r4_;    
    #else
    using fv3_iir_lr4 = fv3::iir_1r4_f;    
    #endif
    struct IIR4 : public FilterProcessorPlugin<fv3_iir_lr4>
    {        
        enum {
            DIRECT_FORM_I,
            DIRECT_FORM_II,
        };

        int type = LPF;
        int di = DIRECT_FORM_I;
        DspFloatType Fc,Fs;

        IIR4() : FilterProcessorPlugin<fv3_iir_lr4>()  {
            Fc = 1000.0f;
            Fs = sampleRate;        
        }
        
        void setLPF(DspFloatType fc, DspFloatType sr) {
            type = LPF;
            Fc = fc;
            Fs = sr;
            this->setLPF_RBJ(fc,fs);
        }
        void setHPF(DspFloatType fc, DspFloatType sr) {
            type = HPF;
            Fc = fc;
            Fs = sr;
            this->setHPF_RBJ(fc,fs);
        }
        void setFilterParameters(DspFloatType fc) {
            switch(type)
            {
                case LPF: setLPF(fc,Fs); break;
                case HPF: setHPF(fc,Fs); break;
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType f = Fc;            
            Fc *= X;            
            setFilterParameters(Fc);
            if(di == DIRECT_FORM_I)
                r=this->processd1(I);
            else
                r=this->processd2(I);
            setFilterParameters(f);
            return A*r;
        }
    };
};