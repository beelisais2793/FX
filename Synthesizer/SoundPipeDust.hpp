#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Dust : public SoundPiper
    {    
        Dust(sp_data * data) :
        SoundPiper(data,(create_func)sp_dust_create,(init_func)sp_dust_init,(create_func)sp_dust_destroy,(compute_func)sp_dust_compute) 
        {        
            
        }        
        void setAmp(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->amp = x;
        }
        void setDensity(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->density = x;
        }
        void setDensity0(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->density0 = x;
        }
        void setThreshold(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->thresh = x;
        }
        void setScale(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->scale = x;
        }
        void setOneDSR(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->onedsr = x;
        }
        void setBipolar(bool x) {
            sp_dust * p = (sp_dust*)ptr;
            p->bipolar = x;
        }
    };
}