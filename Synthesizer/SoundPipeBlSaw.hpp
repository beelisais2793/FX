#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct BlSaw : public SoundPipe
    {
        sp_blsaw * p;

        BlSaw(sp_data * data, float freq, float amp) 
        : SoundPipe(data) 
        {
            sp_blsaw_create(&p);
            sp_blsaw_init(sp,p);
            setFreq(freq);
            setAmp(amp);
        }
        ~BlSaw() {
            if(p) sp_blsaw_destroy(&p);
        }
        void setFreq(float f) { *p->freq = f; }
        void setAmp(float a) { *p->amp = a; }

        float Tick(float I=1, float A=1, float X = 0, float Y=0)
        {            
            // in does nothing
            float in = 0;
            float out= 0;
            float f = *p->freq;
            *p->freq = I*(f + X*f);
            sp_blsaw_compute(sp,p,&in,&out);
            *p->freq = f;
            return out;
        }
    };
}