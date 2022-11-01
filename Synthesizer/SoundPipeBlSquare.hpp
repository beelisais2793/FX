#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct BlSquare : public SoundPipe
    {
        sp_blsquare * p;

        BlSquare(sp_data * data, float freq, float amp, float duty) 
        : SoundPipe(data) 
        {
            sp_blsquare_create(&p);
            sp_blsquare_init(sp,p);
            setFreq(freq);
            setAmp(amp);
            setDuty(duty);
        }
        ~BlSquare() {
            if(p) sp_blsquare_destroy(&p);
        }
        void setFreq(float f) { *p->freq = f; }
        void setAmp(float a) { *p->amp = a; }
        void setDuty(float d) { *p->width = d; }

        float Tick(float I=0, float A=0, float X = 0, float Y=0)
        {            
            float in = I;
            float out= I;
            float f = *p->freq;
            float d = *p->width;
            *p->freq = I*(f + X*f);
            *p->width = pipe_clamp(d + 0.5*Y+d,0,1);
            sp_blsquare_compute(sp,p,&in,&out);
            *p->freq = f;
            *p->width= d;
            return out;
        }
    };
}