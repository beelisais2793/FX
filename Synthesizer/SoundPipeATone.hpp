#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct ATone : public SoundPipe
    {
        sp_atone * obj;

        ATone(sp_data * data, float hp) : SoundPipe(data) {
            sp_atone_create(&obj);
            sp_atone_init(sp,obj);
            setHp(hp);
        }
        ~ATone() {
            if(obj) sp_atone_destroy(&obj);
        }
        void setHp(float h) {
            obj->hp = h;
        }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out = I;
            Undenormal denormal;
            sp_atone_compute(sp,obj,&in,&out);
            return out;
        }
    };
}