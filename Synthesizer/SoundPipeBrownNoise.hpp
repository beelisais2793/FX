#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct BrownNoise : public SoundPipe
    {
        sp_brown * p;
        // OnePole low,high;
        BrownNoise(sp_data * data, float amt) : SoundPipe(data) {
            sp_brown_create(&p);
            sp_brown_init(sp,p);
            setNoise(amt);
        }
        ~BrownNoise() {
            if(p) sp_brown_destroy(&p);
        }
        void setNoise(float n) { p->brown = n; }

        float Tick(float I=0,float A=1, float X = 0, float Y = 1) {
            float in = 0;
            float out = 0;
            X *= sp->sr;
            Y *= sp->sr;
            // X = lowpass cutoff
            // Y = highpass cutoff
            sp_brown_compute(sp,p,&in,&out);
            return A * out;
        }
    };
}