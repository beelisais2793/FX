#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Exponential : public SoundPiper
    {    
        Exponential(sp_data * data, float a, float d, float b) :
        SoundPiper(data,(create_func)sp_eqfil_create,(init_func)sp_eqfil_init,(create_func)sp_eqfil_destroy,(compute_func)sp_eqfil_compute) 
        {        
            setA(f);
            setB(b);
            setDur(g);
        }        
        void setA(float x) {
            sp_expon * p = (sp_expon*)ptr;
            p->a = x;
        }
        void setB(float x) {
            sp_expon * p = (sp_expon*)ptr;
            p->b = x;
        }
        void setDur(float x) {
            sp_expon * p = (sp_expon*)ptr;
            p->dur = x;
        }        
    };
}