#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct BiScale : public SoundPipe
    {
        sp_biscale * p;

        BiScale(sp_data * data, float min, float max) 
        : SoundPipe(data) 
        {
            sp_biscale_create(&p);
            sp_biscale_init(data,p);
            p->min = min;
            p->max = max;
        }
        ~BiScale() {
            if(p) sp_biscale_destroy(&p);
        }
        void setMin(float m) { p->min = m; }
        void setMax(float m) { p->max = m; }

        float Tick(float I, float A=1, float X = 0, float Y = 0) {
            float in = I;
            float out= I;
            sp_biscale_compute(sp,p,&in,&out);
            return out;
        }
    };
}