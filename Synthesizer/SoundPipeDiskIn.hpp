#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct DiskIn : public SoundPipe
    {
        sp_diskin * diskin;
        
        Delay(sp_data * data, const char * filename)
        : SoundPipe(data)        
        {                    
            sp_diskin_create(&diskin);
            sp_diskin_init(data,diskin,filename);            
        }        
        ~DiskIn() {
            if(diskin) sp_diskin_destroy(&diskin);
        }
        float Tick(float I1, float A=1, float X = 0, float Y = 1) {
            float in  = I1;            
            float out = 0;            
            sp_diskin_compute(sp,delay,&in,&out);
            return A * out;
        }
    };    
}