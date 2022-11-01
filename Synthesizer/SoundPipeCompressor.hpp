#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Compressor : public SoundPiper
    {    
        Compressor(sp_data * data, float ratio, float thresh, float atk, float rel) :
        SoundPiper(data,(create_func)sp_compressor_create,(init_func)sp_compressor_init,(create_func)sp_compressor_destroy,(compute_func)sp_compressor_compute) 
        {        

            setRatio(ratio);
            setThreshold(thresh);
            setAttack(atk);
            setRelease(rel);
        }
        void setRatio(float _ratio) {
            sp_compressor *p = (sp_compressor*)ptr;
            *p->ratio = _ratio;
        }
        void setThreshold(float _thresh)
        {         
            sp_compressor *p = (sp_compressor*)ptr;
            *p->thresh = _thresh;
        }
        void setAttack(float _attack)
        {         
            sp_compressor *p = (sp_compressor*)ptr;
            *p->atk = _attack;
        }
        void setRelease(float _release)
        {         
            sp_compressor *p = (sp_compressor*)ptr;
            *p->rel = _release;
        }
    };
}