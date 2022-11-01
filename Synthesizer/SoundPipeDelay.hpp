#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Delay : public SoundPipe
    {
        sp_delay * delay;
        
        Delay(sp_data * data, float time) 
        : SoundPipe(data)        
        {                    
            sp_delay_create(&delay);
            sp_delay_init(data,delay,time);
            
        }
        void setDelayTime(float t) {
            delay->time = t;
        }
        void setFeedback(float f) {
            delay->feedback = f;
        }
        ~Delay() {
            if(delay) sp_delay_destroy(&delay);
        }
        float Tick(float I1, float A=1, float X = 0, float Y = 1) {
            float in  = I1;            
            float out = 0;            
            sp_delay_compute(sp,delay,&in,&out);
            return A * out;
        }
    };    
}