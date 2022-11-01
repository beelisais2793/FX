#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Fog : public SoundPipe
    {    
        sp_fog * fog;
        Fog(sp_data * data, sp_ftbl * wav, sp_ftbl * win, int iolaps, float iphs) :
        SoundPipe(data)
        {        
            sp_fog_create(&fog);
            sp_fog_init(data,fog,wav,win,iolaps,iphs);
        }        
        ~Fog() {
            sp_fog_destroy(&fog);
        }
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_fog_compute(sp,fog,&in,&out);
            return A * out;
        }  
    };
}