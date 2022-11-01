#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct BitCrush : public SoundPipe
    {
        sp_bitcrush * p;

        BitCrush(sp_data * data, float bitdepth) 
        : SoundPipe(data) 
        {
            sp_bitcrush_create(&p);
            sp_bitcrush_init(sp,p);
        }
        ~BitCrush() {
            if(p) sp_bitcrush_destroy(&p);
        }
        void setBitDepth(float b) { p->bitdepth = b; }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out= I;
            sp_bitcrush_compute(sp,p,&in,&out);
            return out;
        }
    };
}