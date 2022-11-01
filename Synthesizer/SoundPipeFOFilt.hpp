#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct FOFilt : public SoundPiper
    {    
        FOFilt(sp_data * data, float f, float a, float d, float istor)
        SoundPiper(data,(create_func)sp_eqfil_create,(init_func)sp_eqfil_init,(create_func)sp_eqfil_destroy,(compute_func)sp_eqfil_compute) 
        {        
            setFreq(f);
            setAtk(a);
            setDec(d);
            setIStor(istor);
        }        
        void setFreq(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->freq = x;
        }
        void setAtk(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->atk = x;
        }
        void setDec(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->dec = x;
        }        
        void setIStor(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->istor = x;
        }        
    };
}