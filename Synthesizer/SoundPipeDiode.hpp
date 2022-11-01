#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
   struct Diode : public SoundPiper
    {
        Diode(sp_data * data) :
        SoundPiper(data,(create_func)sp_diode_create,(init_func)sp_diode_init,(create_func)sp_diode_destroy,(compute_func)sp_diode_compute) 
        {        
        
        }        
    };
}