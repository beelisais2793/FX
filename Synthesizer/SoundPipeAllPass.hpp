#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct AllPass : public SoundPipe
    {
        sp_allpass * obj;

        AllPass(sp_data * data, float looptime) : SoundPipe(data) {
            sp_allpass_create(&obj);            
            sp_allpass_init(sp,obj,looptime);
        }
        ~AllPass() {
            if(obj) sp_allpass_destroy(&obj);
        }        
        float Tick(float I, float A = 1, float X = -1, float Y = 1) {
            float out = 0.0f;
            float in  = I;
            if(in < X) in = X;
            if(in > Y) in = Y;
            Undenormal denormal;
            sp_allpass_compute(sp,obj,&in,&out);
            return A*out;
        }
    };
}