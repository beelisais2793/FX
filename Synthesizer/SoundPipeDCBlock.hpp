#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
    struct DCBlock : public SoundPiper
    {    
        DCBlock(sp_data * data) :
        SoundPiper(data,(create_func)sp_dcblock_create,(init_func)sp_dcblock_init,(create_func)sp_dcblock_destroy,(compute_func)sp_dcblock_compute) 
        {        
        
        }        
    };
}