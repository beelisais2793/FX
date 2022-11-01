#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Clock : public SoundPiper
    {    
        Clock(sp_data * data, float bpm, float subdiv) :
        SoundPiper(data,(create_func)sp_clock_create,(init_func)sp_clock_init,(create_func)sp_clock_destroy,(compute_func)sp_clock_compute) 
        {        
            setBPM(bpm);
            setSubDivision(subdiv);
        }
        void setBPM(float _bpm) {
            sp_clock *p = (sp_clock*)ptr;
            p->bpm = _bpm;
        }
        void setSubDivision(float _sub) {
            sp_clock *p = (sp_clock*)ptr;
            p->subdiv = _sub;
        }
    };
}
    
