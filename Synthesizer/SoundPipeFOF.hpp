#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct FOF : public SoundPipe
    {    
        sp_fof * fof;
            
        FOF(sp_data * data, sp_ftbl * sine, sp_ftbl * win, int overlaps, float iphs) : SoundPipe(data)        
        {                    
            sp_fof_create(&fof);
            sp_fof_init(data,fof,sine,win,overlaps,iphs);            
        }
        ~DTrig() {
            if(fof) sp_conv_destroy(&fof);            
        }        
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_fof_compute(sp,fof,&in,&out);
            return A * out;
        }
    };
}