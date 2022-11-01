#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct BlTriangle : public SoundPipe
    {
        sp_bltriangle * p;

        BlTriangle(sp_data * data, float freq, float amp)
        : SoundPipe(data) 
        {
            sp_bltriangle_create(&p);
            sp_bltriangle_init(sp,p);
            setFreq(freq);
            setAmp(amp);
        }
        ~BlTriangle() {
            if(p) sp_bltriangle_destroy(&p);
        }
        void setFreq(float f) { *p->freq = f; }
        void setAmp(float a) { *p->amp = a; }

        float Tick(float I=1, float A=1, float X = 0, float Y=0)
        {            
            // input does nothing
            float in = 0;
            float out= 0;
            float f = *p->freq;
            *p->freq = I*(f + X*f);
            sp_bltriangle_compute(sp,p,&in,&out);
            *p->freq = f;
            return out;
        }
    };
}