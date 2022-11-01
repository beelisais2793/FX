#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Conv : public SoundPipe
    {    
        sp_conv * conv;
        // have to fill this table with the impulse response
        
        Conv(sp_data * data, sp_ftbls * ftbl, size_t partlen) : SoundPipe(data)        
        {                    
            sp_conv_create(&conv);
            sp_conv_init(data,conv,ftbl,partlen);
            
        }
        ~Conv() {
            if(conv) sp_conv_destroy(&conv);            
        }
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_conv_compute(sp,conv,&in,&out);
            return A * out;
        }
    };
}