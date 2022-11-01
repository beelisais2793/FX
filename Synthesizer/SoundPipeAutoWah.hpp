#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct AutoWah : public SoundPipe
    {
        sp_autowah * obj;

        AutoWah(sp_data * data, float level, float wah, float mix) : SoundPipe(data) {
            sp_autowah_create(&obj);
            sp_autowah_init(sp,obj);
            setLevel(level);
            setWah(wah);
            setMix(mix);
        }
        ~AutoWah() {
            if(obj) sp_autowah_destroy(&obj);
        }
        void setLevel(float x) {
            *obj->level = x;;
        }
        void setWah(float x) {
            *obj->wah = x;;
        }
        void setMix(float x) {
            *obj->mix = x;;
        }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out = I;
            Undenormal denormal;
            sp_autowah_compute(sp,obj,&in,&out);
            return out;
        }
    };
}