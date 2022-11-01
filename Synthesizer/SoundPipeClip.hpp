#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Clip : public SoundPiper
    {
        Clip(sp_data * data, float lim, float k1) :
        SoundPiper(data,(create_func)sp_clip_create,(init_func)sp_clip_init,(create_func)sp_clip_destroy,(compute_func)sp_clip_compute) 
        {
            setLimit(lim);
            setK1(k1);
        }
        void setLimit(float limit) {
            sp_clip *p = (sp_clip*)ptr;
            p->lim = limit;
        }
        void setK1(float k1) {
            sp_clip *p = (sp_clip*)ptr;
            p->k1 = k1;
        }
        
    };
}