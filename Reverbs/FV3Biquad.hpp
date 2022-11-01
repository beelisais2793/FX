#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_biquad = fv3::biquad_;
    #else
    using fv3_biquad = fv3::biquad_f;
    #endif
    struct Biquad : public FilterProcessorPlugin<fv3_biquad>
    {
        
        enum {
            APF,
            LPF,
            HPF,
            BSF,
            BPF,
            BPFP,
            PEAKEQ,
            LSF,
            HSF
        };
        enum {
            DIRECT_FORM_I,
            DIRECT_FORM_II,
        };

        int type = LPF;
        int di = DIRECT_FORM_I;
        DspFloatType Fc,Bw,Fs,Gain,Slope;
        unsigned mode=0;

        Biquad() : FilterProcessorPlugin<fv3_biquad>() {
            Fs = sampleRate;
            Fc = 440.0;
            Bw = 0.5;
            Gain = 1.0;
            Slope = 0.5;
        }                
        void setAPF_RBJ(DspFloatType fc, DspFloatType bw, DspFloatType fs, unsigned mode)
        {
            
            Fc = fc;
            Bw = bw;
            type = APF;
            this->setAPF_RBJ(fc,bw,fs,mode);
        }
        void setLPF_RBJ(DspFloatType fc, DspFloatType bw, DspFloatType fs, unsigned mode)
        {
            Fc = fc;
            Bw = bw;
            type = LPF;
            this->setLPF_RBJ(fc,bw,fs,mode);
        }
        void setHPF_RBJ(DspFloatType fc, DspFloatType bw, DspFloatType fs, unsigned mode)
        {
            Fc = fc;
            Bw = bw;
            type = HPF;
            this->setHPF_RBJ(fc,bw,fs,mode);
        }
        void setBSF_RBJ(DspFloatType fc, DspFloatType bw, DspFloatType fs, unsigned mode)
        {
            type = BSF;
            this->setBSF_RBJ(fc,bw,fs,mode);
        }
        void setBPF_RBJ(DspFloatType fc, DspFloatType bw, DspFloatType fs, unsigned mode)
        {
            Fc = fc;
            Bw = bw;
            type = BPF;
            this->setBPF_RBJ(fc,bw,fs,mode);
        }
        void seBPFP_RBJ(DspFloatType fc, DspFloatType bw, DspFloatType fs, unsigned mode)
        {
            Fc = fc;
            Bw = bw;
            type = BPFP;
            this->setBPFP_RBJ(fc,bw,fs,mode);
        }
        void setPeakEQ_RBJ(DspFloatType fc, DspFloatType gain, DspFloatType bw, DspFloatType fs)
        {
            Fc = fc;
            Gain = gain;
            type = PEAKEQ;
            this->setPeakEQ_RBJ(fc,gain,bw,fs);
        }
        void setLSF_RBJ(DspFloatType fc, DspFloatType gain, DspFloatType slope, DspFloatType fs)
        {
            Fc = fc;
            Gain = gain;
            type = LSF;
            this->setLSF_RBJ(fc,gain,slope,fs);
        }
        void setHSF_RBJ(DspFloatType fc, DspFloatType gain, DspFloatType slope, DspFloatType fs)
        {
            Fc = fc;
            Gain = gain;
            type = HSF;
            this->setHSF_RBJ(fc,gain,slope,fs);
        }
        void setFilterParameters(DspFloatType fc, DspFloatType bw, DspFloatType gain, DspFloatType slope) {
            switch(type) {
                case APF: setAPF_RBJ(fc,bw,Fs,mode); break;
                case LPF: setLPF_RBJ(fc,bw,Fs,mode); break;
                case HPF: setHPF_RBJ(fc,bw,Fs,mode); break;
                case BSF: setBSF_RBJ(fc,bw,Fs,mode); break;
                case BPF: setBPF_RBJ(fc,bw,Fs,mode); break;
                case BPFP: setBPFP_RBJ(fc,bw,Fs,mode); break;
                case PEAKEQ: setPeakEQ_RBJ(fc,gain,bw,Fs); break;
                case LSF: setLSF_RBJ(fc,gain,slope,Fs); break;
                case HSF: setHSF_RBJ(fc,gain,slope,Fs); break;
            }            
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {          
            DspFloatType r;  
            if(di == DIRECT_FORM_I)
                r=this->processd1(I);
            else
                r=this->processd2(I);         
            return A*r;
        }
    };
}