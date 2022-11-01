#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct Count: public SoundPiper
    {    
        Count(sp_data * data, int32_t count) :
        SoundPiper(data,(create_func)sp_count_create,(init_func)sp_count_init,(create_func)sp_count_destroy,(compute_func)sp_count_compute) 
        {        
            setCount(count);            
        }
        void setCount(int32_t count) {
            sp_count *p = (sp_count*)ptr;
            p->count = count;
        }        
    };
}