#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct EQFilter : public SoundPiper
    {    
        EQFilter(sp_data * data, float f, float b, float g) :
        SoundPiper(data,(create_func)sp_eqfil_create,(init_func)sp_eqfil_init,(create_func)sp_eqfil_destroy,(compute_func)sp_eqfil_compute) 
        {        
            setFreq(f);
            setBandwidth(b);
            setGain(g);
        }        
        void setFreq(float x) {
            sp_eqfil * p = (sp_eqfil*)ptr;
            p->freq = x;
        }
        void setBandwidth(float x) {
            sp_eqfil * p = (sp_eqfil*)ptr;
            p->bw = x;
        }
        void setGain(float x) {
            sp_eqfil * p = (sp_eqfil*)ptr;
            p->gain = x;
        }        
    };
}