#pragma once

#include <cmath>

#include "Undenormal.hpp"


extern "C"
{
#include "SoundPipe/soundpipe.h"
/*
#include "SoundPipe/adsr.h"
#include "SoundPipe/allpass.h"
#include "SoundPipe/atone.h"
#include "SoundPipe/autowah.h"
#include "SoundPipe/bal.h"
#include "SoundPipe/bar.h"
#include "SoundPipe/base.h"
#include "SoundPipe/biquad.h"
#include "SoundPipe/biscale.h"
#include "SoundPipe/bitcrush.h"
#include "SoundPipe/blsaw.h"
#include "SoundPipe/blsquare.h"
#include "SoundPipe/bltriangle.h"
#include "SoundPipe/brown.h"
#include "SoundPipe/butbp.h"
#include "SoundPipe/butbr.h"
#include "SoundPipe/buthp.h"
#include "SoundPipe/butlp.h"
#include "SoundPipe/clip.h"
#include "SoundPipe/clock.h"
#include "SoundPipe/comb.h"
#include "SoundPipe/compressor.h"
#include "SoundPipe/conv.h"
#include "SoundPipe/count.h"
#include "SoundPipe/crossfade.h"
#include "SoundPipe/dcblock.h"
#include "SoundPipe/delay.h"
#include "SoundPipe/diode.h"
#include "SoundPipe/diskin.h"
#include "SoundPipe/dist.h"
#include "SoundPipe/dmetro.h"
#include "SoundPipe/drip.h"
#include "SoundPipe/dtrig.h"
#include "SoundPipe/dust.h"
#include "SoundPipe/eqfil.h"
#include "SoundPipe/expon.h"
#include "SoundPipe/fftwrapper.h"
#include "SoundPipe/fof.h"
#include "SoundPipe/fofilt.h"
#include "SoundPipe/fog.h"
#include "SoundPipe/fold.h"
#include "SoundPipe/foo.h"
#include "SoundPipe/fosc.h"
#include "SoundPipe/ftbl.h"
#include "SoundPipe/gbuzz.h"
#include "SoundPipe/hilbert.h"
#include "SoundPipe/in.h"
#include "SoundPipe/incr.h"
#include "SoundPipe/jack.h"
#include "SoundPipe/jcrev.h"
#include "SoundPipe/jitter.h"
#include "SoundPipe/line.h"
#include "SoundPipe/lpc.h"
#include "SoundPipe/lpf18.h"
#include "SoundPipe/maygate.h"
#include "SoundPipe/metro.h"
#include "SoundPipe/mincer.h"
#include "SoundPipe/mode.h"
#include "SoundPipe/moogladder.h"
#include "SoundPipe/noise.h"
#include "SoundPipe/nsmp.h"
#include "SoundPipe/osc.h"
#include "SoundPipe/oscmorph.h"
#include "SoundPipe/padsynth.h"
#include "SoundPipe/pan2.h"
#include "SoundPipe/panst.h"
#include "SoundPipe/pareq.h"
#include "SoundPipe/paulstretch.h"
#include "SoundPipe/pdhalf.h"
#include "SoundPipe/peaklim.h"
#include "SoundPipe/phaser.h"
#include "SoundPipe/phasor.h"
#include "SoundPipe/pinknoise.h"
#include "SoundPipe/pitchamdf.h"
#include "SoundPipe/pluck.h"
#include "SoundPipe/port.h"
#include "SoundPipe/posc3.h"
#include "SoundPipe/progress.h"
#include "SoundPipe/prop.h"
#include "SoundPipe/pshift.h"
#include "SoundPipe/ptrack.h"
#include "SoundPipe/randh.h"
#include "SoundPipe/randi.h"
#include "SoundPipe/randmt.h"
#include "SoundPipe/random.h"
#include "SoundPipe/reson.h"
#include "SoundPipe/reverse.h"
#include "SoundPipe/revsc.h"
#include "SoundPipe/rms.h"
#include "SoundPipe/rpi.h"
#include "SoundPipe/rpt.h"
#include "SoundPipe/rspline.h"
#include "SoundPipe/samphold.h"
#include "SoundPipe/saturator.h"
#include "SoundPipe/scale.h"
#include "SoundPipe/scrambler.h"
#include "SoundPipe/sdelay.h"
#include "SoundPipe/slice.h"
#include "SoundPipe/smoothdelay.h"
#include "SoundPipe/spa.h"
#include "SoundPipe/sparec.h"
#include "SoundPipe/sp_base.h"
#include "SoundPipe/streson.h"
#include "SoundPipe/switch.h"
#include "SoundPipe/tabread.h"
#include "SoundPipe/tadsr.h"
#include "SoundPipe/talkbox.h"
#include "SoundPipe/tblrec.h"
#include "SoundPipe/tbvcf.h"
#include "SoundPipe/tdiv.h"
#include "SoundPipe/tenv.h"
#include "SoundPipe/tenv2.h"
#include "SoundPipe/tenvx.h"
#include "SoundPipe/tevent.h"
#include "SoundPipe/tgate.h"
#include "SoundPipe/thresh.h"
#include "SoundPipe/timer.h"
#include "SoundPipe/tin.h"
#include "SoundPipe/tone.h"
#include "SoundPipe/trand.h"
#include "SoundPipe/tseg.h"
#include "SoundPipe/tseq.h"
#include "SoundPipe/vdelay.h"
#include "SoundPipe/voc.h"
#include "SoundPipe/vocoder.h"
#include "SoundPipe/waveset.h"
#include "SoundPipe/wavin.h"
#include "SoundPipe/wavout.h"
#include "SoundPipe/wpkorg35.h"
#include "SoundPipe/zitarev.h"
*/
}

namespace SoundPipe
{
    float pipe_clamp(float x, float a, float b) {
        return x < a? a: x > b? b: x;
    }
    inline sp_data* create_soundpipe(int sample_rate)
    {
        sp_data *sp;
        sp_create(&sp);
        sp->sr = sample_rate;
        return sp;
    }

    inline void destroy_soundpipe(sp_data * sp) 
    {
        sp_destroy(&sp);
    }

    struct SoundPipe
    {
        sp_data * sp;
        SoundPipe(sp_data * d) : sp(d) {
            
        }
        virtual float Tick(float I, float A=1, float X=0, float Y=0) = 0;
    };

    struct SoundPiper : public SoundPipe
    {
        void * ptr;
        
        int (*sp_destroy)(void **);
        int (*sp_compute)(sp_data*,void*,float*,float*);
        SoundPiper(sp_data * data,
                       int (*create)(void **),
                       int (*init)(sp_data*, void*),
                       int (*destroy)(void **),
                       int (*compute)(sp_data*,void *, float *, float *))              
        : SoundPipe(data)                       
        {
                create(&ptr);
                init(data,ptr);
                sp_destroy = destroy;
        }                       
        ~SoundPiper()
        {
            if(ptr) sp_destroy(&ptr);
        }
        float Tick(float I=1,float A=1,float X=0,float Y=0)
        {
            float in = I;
            float out= I;
            sp_compute(sp,ptr,&in,&out);
            return A*out;
        }
    };

    typedef int (*create_func)(void **);
    typedef int (*init_func)(sp_data*,void*);
    typedef int (*compute_func)(sp_data*,void *, float *,float *);
}