#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct ButterworthBandpass : public SoundPiper
    {
        ButterworthBandpass(sp_data * data, float cutoff, float bw) :
        SoundPiper(data,(create_func)sp_butbp_create,(init_func)sp_butbp_init,(create_func)sp_butbp_destroy,(compute_func)sp_butbp_compute) 
        {
            setFreq(cutoff);
            setBandwidth(bw);
        }
        void setFreq(float f) {
            sp_butbp *bp = (sp_butbp*)ptr;
            bp->freq = f;
        }
        void setBandwidth(float bw) {
            sp_butbp* bp = (sp_butbp*)ptr;
            bp->bw = bw;
        }
        void setLkf(float f) {
            sp_butbp* bp = (sp_butbp*)ptr;
            bp->lkf = f;
        }
        void setLkb(float f) {
            sp_butbp* bp = (sp_butbp*)ptr;
            bp->lkb = f;
        }
    };

    struct ButterworthBandstop : public SoundPiper
    {
        ButterworthBandstop(sp_data * data, float cutoff, float bw) :
        SoundPiper(data,(create_func)sp_butbr_create,(init_func)sp_butbr_init,(create_func)sp_butbr_destroy,(compute_func)sp_butbr_compute) 
        {
            setFreq(cutoff);
            setBandwidth(bw);
        }
        void setFreq(float f) {
            sp_butbr *bp = (sp_butbr*)ptr;
            bp->freq = f;
        }
        void setBandwidth(float bw) {
            sp_butbr* bp = (sp_butbr*)ptr;
            bp->bw = bw;
        }
        void setLkf(float f) {
            sp_butbr* bp = (sp_butbr*)ptr;
            bp->lkf = f;
        }
        void setLkb(float f) {
            sp_butbr* bp = (sp_butbr*)ptr;
            bp->lkb = f;
        }
    };

    struct ButterworthHighpass : public SoundPiper
    {
        ButterworthHighpass(sp_data * data, float cutoff) :
        SoundPiper(data,(create_func)sp_buthp_create,(init_func)sp_buthp_init,(create_func)sp_buthp_destroy,(compute_func)sp_buthp_compute) 
        {
            setFreq(cutoff);            
        }
        void setFreq(float f) {
            sp_buthp *bp = (sp_buthp*)ptr;
            bp->freq = f;
        }        
        void setLkf(float f) {
            sp_buthp* bp = (sp_buthp*)ptr;
            bp->lkf = f;
        }        
    };

    struct ButterworthLowpass : public SoundPiper
    {
        ButterworthLowpass(sp_data * data, float cutoff) :
        SoundPiper(data,(create_func)sp_butlp_create,(init_func)sp_butlp_init,(create_func)sp_butlp_destroy,(compute_func)sp_butlp_compute) 
        {
            setFreq(cutoff);         
        }
        void setFreq(float f) {
            sp_butlp *bp = (sp_butlp*)ptr;
            bp->freq = f;
        }        
        void setLkf(float f) {
            sp_butlp* bp = (sp_butlp*)ptr;
            bp->lkf = f;
        }        
    };
}