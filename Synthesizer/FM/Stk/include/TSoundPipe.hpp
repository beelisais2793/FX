/////////////////////////////////////////////////////////////////////
// SoundPipe/SndKit
// LoaD SoundPipe/SndKit opcodes
/////////////////////////////////////////////////////////////////////
#pragma once

#include "SoundAlchemy.hpp"

extern "C"
{
#include "soundpipe.h"
};

namespace SoundAlchemy::SoundPipe
{
    struct SoundPipeLoader
    {
        void    * obj;
        int    (*delete_func)(void * ptr);
        int     (*compute)
        sp_data * data;

        SoundPipeLoader() {    
            int err = sp_create(&sp);
            obj = NULL;
        }
        ~SoundPiperLoader() {
            int err;
            if(obj) err=delete_func(obj);
        }


        sp_osc_create(&ud.osc);
        sp_ftbl_create(sp, &ud.ft, 2048);

        sp_adsr_init(sp, ud.adsr);
        sp_gen_sine(sp, ud.ft);
        sp_osc_init(sp, ud.osc, ud.ft, 0);
        ud.osc->amp = 0.5;

        sp->len = 44100 * 5;
        sp_process(sp, &ud, process);
        sp_adsr* CreateADSR()
        {        
            int err = sp_adsr_create(&obj);
            assert(obj != NULL);
            delete_func = sp_adsr_destroy;
            compute = sp_adsr_compute;
        }

        T Tick(T I, T A = 1, T F = 1, T P = 1)
        {        
            I = clamp(I,-1,1);
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            T in = I*P;
            T out = 0.0f;
            int err = compute(data,obj,&in,&out);
            return A * F * out;
        }
    }
}