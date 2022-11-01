#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Crossfade : public SoundPipe
    {    
        sp_crossfade * fader;
        // have to fill this table with the impulse response
        
        Crossfade(sp_data * data, float pos=0.5) : SoundPipe(data)        
        {                    
            sp_crossfade_create(&fader);
            sp_crossfade_init(data,fader);
            
        }
        ~Crossfade() {
            if(fader) sp_crossfade_destroy(&fader);
        }
        void setPosition(float pos) {
            fader->pos = pos;
        }
        float Tick(float I1, float I2, float A=1, float X = 0, float Y = 1) {
            float in1 = I1;
            float in2 = I2;
            float out = 0;            
            sp_crossfade_compute(sp,fader,&in1,&in2,&out);
            return A * out;
        }
    };
}