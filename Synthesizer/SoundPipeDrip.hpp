#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Drip : public SoundPipe
    {
        sp_drip * drip;
        
        Drip(sp_data * data, float d)
        : SoundPipe(data)        
        {                    
            sp_drip_create(&drip);
            sp_drip_init(data,drip,d);            
        }        
        
        ~Drip() {
            if(drip) sp_drip_destroy(&drip);
        }
        
        // dont know if all of these will do anything
        void setAmp(float x) {
            drip->amp = x;
        }
        void setNumTubes(int x) {
            drip->num_tubes = x;
        }
        void setDamp(float x) {
            drip->damp = x;
        }
        void setShakeMix(float x)
        {
            drip->shake_mix = x;
        }
        void setFreq(float x) {
            drip->freq = x;
        }
        void setFreq1(float x) {
            drip->freq1 = x;
        }
        void setFreq2(float x) {
            drip->freq2 = x;
        }
        void setDrip(float d) {
            drip->dettack=d;
        }
        float Tick(float I1, float A=1, float X = 0, float Y = 1) {
            float in  = I1;            
            float out = 0;            
            sp_drip_compute(sp,drip,&in,&out);
            return A * out;
        }
    };    
 }