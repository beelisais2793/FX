#pragma once

#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    #ifdef DSPFLOATDOUBLE
    using fv3_dl_gd_largeroom = fv3::dl_gd_largeroom_;    
    using fv3_gd_largeroom = fv3::gd_largeroom_;    
    #else
    using fv3_dl_gd_largeroom = fv3::dl_gd_largeroom_f;    
    using fv3_gd_largeroom = fv3::gd_largeroom_f;    
    #endif
    struct LargeRoomDelayLine : public FilterProcessorPlugin<fv3_dl_gd_largeroom>
    {
        DspFloatType Dcc,Fc,Dcy;
        LargeRoomDelayLine(DspFloatType dcc, DspFloatType fc, DspFloatType dcy) 
        : FilterProcessorPlugin<fv3_dl_gd_largeroom>()
        {
            Dcc =dcc;
            Fc  =fc;
            Dcy =dcy;
            this->setDCC(dcc);
            this->setLPF(fc);
            this->setDecay(dcy);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            this->setDCC(Dcc*A);
            this->setLPF(Fc*X);
            this->setDecay(Dcy*Y);
            DspFloatType out = this->process(I);
            this->setDCC(Dcc);
            this->setLPF(Fc);
            this->setDecay(Dcy);
            return out;
        }
    };

    struct LargeRoom : public StereoFXProcessorPlugin<fv3_gd_largeroom>
    {
        LargeRoom(size_t size, DspFloatType damp, DspFloatType fc, DspFloatType dr) 
        : StereoFXProcessorPlugin<fv3_gd_largeroom>()
        {
            this->setroomsize(size);
            this->setdccutfreq(fc);
            this->setdamp(damp);
            this->setLRDiffFactor(dr);
        }
        void ProcessBlock(size_t n, DspFloatType ** input, DspFloatType ** output) {
            this->processreplace(input[0],input[1],output[0],output[1],n);
        }
        void InplaceProcess(size_t n, DspFloatType ** buf) {
            ProcessBlock(n,buf,buf);
        }
    };
}