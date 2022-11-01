#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct CombFilter : public SoundPiper
    {    
        CombFilter(sp_data * data, float revtime, float looptime) :
        SoundPiper(data,(create_func)sp_comb_create,(init_func)sp_comb_init,(create_func)sp_comb_destroy,(compute_func)sp_comb_compute) 
        {        

            setRevTime(revtime);
            setLoopTime(looptime);
        }
        void setRevTime(float _revtime) {
            sp_comb *p = (sp_comb*)ptr;
            p->revtime = _revtime;
        }
        void setLoopTime(float _looptime) {
            sp_comb *p = (sp_comb*)ptr;
            p->looptime = _looptime;
        }
    };
}