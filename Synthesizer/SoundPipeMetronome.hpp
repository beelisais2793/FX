#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Metronome : public SoundPiper
    {    
        Metronome(sp_data * data, float time) :
        SoundPiper(data,(create_func)sp_dist_create,(init_func)sp_dist_init,(create_func)sp_dist_destroy,(compute_func)sp_dist_compute) 
        {        
            setTime(time);
        }        
        void setTime(float t) {
            sp_metro * p = (sp_metro*)ptr;
            p->time = t;
        }
    };
 }