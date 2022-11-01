#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Bar : public SoundPipe
    {
        sp_bar * obj;
    
        Bar(sp_data * data, float iK, float iB) : SoundPipe(data) {
            sp_bar_create(&obj);
            sp_bar_init(sp,obj,iK,iB);        
        }
        ~Bar() {
            if(obj) sp_bar_destroy(&obj);
        }
        void setBcL(float x) {
            obj->bcL = x;
        }
        void setBcR(float x) {
            obj->bcR = x;
        }
        void setIk(float x) {
            obj->iK = x;
        }
        void setIb(float x) {
            obj->ib = x;
        }
        void setScan(float x) {
            obj->scan = x;
        }
        void setT30(float x) {
            obj->T30 = x;
        }
        void setPos(float x) {
            obj->pos = x;
        }
        void setVel(float x) {
            obj->vel = x;        
        }
        void setWid(float x) {
            obj->wid = x;
        }
        
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out = I;
            Undenormal denormal;
            sp_bar_compute(sp,obj,&in,&out);
            return out;
        }
    };
}