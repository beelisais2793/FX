#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct ADSR : public SoundPipe
    {
        
        sp_adsr * adsr;
        float     gate;
        enum { CLEAR, ATTACK, DECAY, SUSTAIN, RELEASE };
        ADSR(sp_data * data, float a, float d, float s, float r) : SoundPipe(data) 
        {
            sp_adsr_create(&adsr);            
            sp_adsr_init(sp, adsr);    
            adsr->atk = a;
            adsr->dec = d;
            adsr->sus = s;
            adsr->rel = r;
            gate = 0;
        }
        ~ADSR() {
            if(adsr) sp_adsr_destroy(&adsr);
        }
        void noteOn() { gate = 1.0f; }
        void noteOff() { gate = 0.0f; }
        void setAttack(float a) { adsr->atk = a; }
        void setDecay(float d) { adsr->dec = d; }
        void setSustain(float s) { adsr->sus = s; }
        void setRelease(float r) { adsr->rel = r; }

        float Tick(float I=0, float A = 1, float X = -1, float Y = 1) {            
            float out= 0;
            Undenormal denormal;
            sp_adsr_compute(sp,adsr,&gate,&out);            
            return out;
        }
    };
}    