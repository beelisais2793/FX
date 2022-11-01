#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Bal : public SoundPipe
    {
        sp_bal * obj;
        float compressor;

        Bal(sp_data * data) : SoundPipe(data) {
            sp_bal_create(&obj);
            sp_bal_init(sp,obj);
            compressor = 0;
        }
        ~Bal() {
            if(obj) sp_bal_destroy(&obj);
        }
        void setAsig(float x) {
            obj->asig = x;
        }
        void setCsig(float x) {
            obj->csig = x;
        }
        void setIhp(float x) {
            obj->ihp = x;
        }
        void setCompress(float c) {
            compressor = c;
        }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out = I;
            sp_bal_compute(sp,obj,&in,&compressor,&out);
            return out;
        }
    };
}