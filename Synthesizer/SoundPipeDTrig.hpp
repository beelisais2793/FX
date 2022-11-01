#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct DTrig : public SoundPipe
    {    
        sp_dtrig * dtrig;
        // have to fill this table with the impulse response
        
        DTrig(sp_data * data, sp_ftbl * ftbl, size_t partlen) : SoundPipe(data)        
        {                    
            sp_dtrig_create(&dtrig);
            sp_conv_init(data,dtrig,ftbl,partlen);
            
        }
        ~DTrig() {
            if(dtrig) sp_conv_destroy(&dtrig);            
        }
        void setDelay(float x)
        {
            dtrig->delay = x;
        }
        void setScale(float x)
        {
            dtrig->scale = x;
        }
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_dtrig_compute(sp,conv,&in,&out);
            return A * out;
        }
    };
}