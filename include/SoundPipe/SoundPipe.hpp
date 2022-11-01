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

    struct AllPass : public SoundPipe
    {
        sp_allpass * obj;

        AllPass(sp_data * data, float looptime) : SoundPipe(data) {
            sp_allpass_create(&obj);            
            sp_allpass_init(sp,obj,looptime);
        }
        ~AllPass() {
            if(obj) sp_allpass_destroy(&obj);
        }        
        float Tick(float I, float A = 1, float X = -1, float Y = 1) {
            float out = 0.0f;
            float in  = I;
            if(in < X) in = X;
            if(in > Y) in = Y;
            Undenormal denormal;
            sp_allpass_compute(sp,obj,&in,&out);
            return A*out;
        }
    };

    struct ATone : public SoundPipe
    {
        sp_atone * obj;

        ATone(sp_data * data, float hp) : SoundPipe(data) {
            sp_atone_create(&obj);
            sp_atone_init(sp,obj);
            setHp(hp);
        }
        ~ATone() {
            if(obj) sp_atone_destroy(&obj);
        }
        void setHp(float h) {
            obj->hp = h;
        }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out = I;
            Undenormal denormal;
            sp_atone_compute(sp,obj,&in,&out);
            return out;
        }
    };
    
    struct AutoWah : public SoundPipe
    {
        sp_autowah * obj;

        AutoWah(sp_data * data, float level, float wah, float mix) : SoundPipe(data) {
            sp_autowah_create(&obj);
            sp_autowah_init(sp,obj);
            setLevel(level);
            setWah(wah);
            setMix(mix);
        }
        ~AutoWah() {
            if(obj) sp_autowah_destroy(&obj);
        }
        void setLevel(float x) {
            *obj->level = x;;
        }
        void setWah(float x) {
            *obj->wah = x;;
        }
        void setMix(float x) {
            *obj->mix = x;;
        }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out = I;
            Undenormal denormal;
            sp_autowah_compute(sp,obj,&in,&out);
            return out;
        }
    };

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

    // resonance is weird as fuck
    struct Biquad : public SoundPipe
    {
        sp_biquad * bq;        
        float cutoff,res;

        Biquad(sp_data * data, float cutoff, float res) : SoundPipe(data)
        {
            sp_biquad_create(&bq);
            sp_biquad_init(sp,bq);            
            calc(cutoff,res);
        }
        void setCutoff(float c) { cutoff = c; }
        void setResonance(float r) {res = r; }

        void calc(float c, float r) {
            //bq->tpidsr = 2.0*M_PI / sp->sr;
            //bq->sr = sp->sr;

            if(r < 0.001) r = 0.001;
            if(r > 0.999) r = 0.999;
            // don't know the resonance is very picky
            r = 0.9+0.1*r;
            res=r;
            cutoff=c;
            bq->cutoff = c;
            bq->res    = r;
            float r2 = r*r;            
            SPFLOAT fcon = cutoff * bq->tpidsr;
            float fc = cos(fcon);
            float fs = sin(fcon);
            SPFLOAT alpha = 1-2*r*fc*fc+r2*cos(2*fcon);
            SPFLOAT beta = r2*sin(2*fcon)-2*r*fc*fs;
            SPFLOAT gamma = 1+fc;
            SPFLOAT m1 = alpha*gamma+beta*fs;
            SPFLOAT m2 = alpha*gamma-beta*fs;
            SPFLOAT den = sqrt(m1*m1+m2*m2);
            
            bq->b0 = 1.5*(alpha*alpha+beta*beta)/den;
            bq->b1 = bq->b0;
            bq->b2 = 0.0;
            bq->a0 = 1.0;
            bq->a1 = -2.0*r*fc;
            bq->a2 = r2;

            // another kaka                        
            /*
            bq->a0 = 1.0f;
            bq->a1 = -2*r*std::cos(2*M_PI*c/bq->sr);
            bq->a2 = r*r;
            bq->b0 = -((1-r*r)/2.0f);
            bq->b1 = 0;
            bq->b2 = -bq->b0;
            */
        }
        float Tick(float I, float A = 1, float X = 0, float Y = 0)
        {
            Undenormal denormal;
            float in = I;
            float out=I;  
            float c = cutoff;
            float r = res;                                  
            calc(cutoff,res);                                    
            sp_biquad_compute(sp,bq,&in,&out);
            cutoff = c;
            res    = r;
            return out;
        }
    };
    
    struct BiScale : public SoundPipe
    {
        sp_biscale * p;

        BiScale(sp_data * data, float min, float max) 
        : SoundPipe(data) 
        {
            sp_biscale_create(&p);
            sp_biscale_init(data,p);
            p->min = min;
            p->max = max;
        }
        ~BiScale() {
            if(p) sp_biscale_destroy(&p);
        }
        void setMin(float m) { p->min = m; }
        void setMax(float m) { p->max = m; }

        float Tick(float I, float A=1, float X = 0, float Y = 0) {
            float in = I;
            float out= I;
            sp_biscale_compute(sp,p,&in,&out);
            return out;
        }
    };

    struct BitCrush : public SoundPipe
    {
        sp_bitcrush * p;

        BitCrush(sp_data * data, float bitdepth) 
        : SoundPipe(data) 
        {
            sp_bitcrush_create(&p);
            sp_bitcrush_init(sp,p);
        }
        ~BitCrush() {
            if(p) sp_bitcrush_destroy(&p);
        }
        void setBitDepth(float b) { p->bitdepth = b; }
        float Tick(float I, float A = 1, float X = 0, float Y = 0) {
            float in = I;
            float out= I;
            sp_bitcrush_compute(sp,p,&in,&out);
            return out;
        }
    };

    struct BlSaw : public SoundPipe
    {
        sp_blsaw * p;

        BlSaw(sp_data * data, float freq, float amp) 
        : SoundPipe(data) 
        {
            sp_blsaw_create(&p);
            sp_blsaw_init(sp,p);
            setFreq(freq);
            setAmp(amp);
        }
        ~BlSaw() {
            if(p) sp_blsaw_destroy(&p);
        }
        void setFreq(float f) { *p->freq = f; }
        void setAmp(float a) { *p->amp = a; }

        float Tick(float I=1, float A=1, float X = 0, float Y=0)
        {            
            // in does nothing
            float in = 0;
            float out= 0;
            float f = *p->freq;
            *p->freq = I*(f + X*f);
            sp_blsaw_compute(sp,p,&in,&out);
            *p->freq = f;
            return out;
        }
    };

    struct BlSquare : public SoundPipe
    {
        sp_blsquare * p;

        BlSquare(sp_data * data, float freq, float amp, float duty) 
        : SoundPipe(data) 
        {
            sp_blsquare_create(&p);
            sp_blsquare_init(sp,p);
            setFreq(freq);
            setAmp(amp);
            setDuty(duty);
        }
        ~BlSquare() {
            if(p) sp_blsquare_destroy(&p);
        }
        void setFreq(float f) { *p->freq = f; }
        void setAmp(float a) { *p->amp = a; }
        void setDuty(float d) { *p->width = d; }

        float Tick(float I=0, float A=0, float X = 0, float Y=0)
        {            
            float in = I;
            float out= I;
            float f = *p->freq;
            float d = *p->width;
            *p->freq = I*(f + X*f);
            *p->width = pipe_clamp(d + 0.5*Y+d,0,1);
            sp_blsquare_compute(sp,p,&in,&out);
            *p->freq = f;
            *p->width= d;
            return out;
        }
    };

    struct BlTriangle : public SoundPipe
    {
        sp_bltriangle * p;

        BlTriangle(sp_data * data, float freq, float amp)
        : SoundPipe(data) 
        {
            sp_bltriangle_create(&p);
            sp_bltriangle_init(sp,p);
            setFreq(freq);
            setAmp(amp);
        }
        ~BlTriangle() {
            if(p) sp_bltriangle_destroy(&p);
        }
        void setFreq(float f) { *p->freq = f; }
        void setAmp(float a) { *p->amp = a; }

        float Tick(float I=1, float A=1, float X = 0, float Y=0)
        {            
            // input does nothing
            float in = 0;
            float out= 0;
            float f = *p->freq;
            *p->freq = I*(f + X*f);
            sp_bltriangle_compute(sp,p,&in,&out);
            *p->freq = f;
            return out;
        }
    };

    struct BrownNoise : public SoundPipe
    {
        sp_brown * p;
        // OnePole low,high;
        BrownNoise(sp_data * data, float amt) : SoundPipe(data) {
            sp_brown_create(&p);
            sp_brown_init(sp,p);
            setNoise(amt);
        }
        ~BrownNoise() {
            if(p) sp_brown_destroy(&p);
        }
        void setNoise(float n) { p->brown = n; }

        float Tick(float I=0,float A=1, float X = 0, float Y = 1) {
            float in = 0;
            float out = 0;
            X *= sp->sr;
            Y *= sp->sr;
            // X = lowpass cutoff
            // Y = highpass cutoff
            sp_brown_compute(sp,p,&in,&out);
            return A * out;
        }
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

    struct ButterworthBandpass : public SoundPiper
    {
        ButterworthBandpass(sp_data * data, float cutoff, float bw) :
        SoundPiper(data,(create_func)sp_butbp_create,(init_func)sp_butbp_init,(create_func)sp_butbp_destroy,(compute_func)sp_butbp_compute) 
        {
            setFreq(cutoff);
            setBandwidth(bw);
        }
        void setFreq(float f) {
            sp_butbp *bp = (sp_butbp*)ptr;
            bp->freq = f;
        }
        void setBandwidth(float bw) {
            sp_butbp* bp = (sp_butbp*)ptr;
            bp->bw = bw;
        }
        void setLkf(float f) {
            sp_butbp* bp = (sp_butbp*)ptr;
            bp->lkf = f;
        }
        void setLkb(float f) {
            sp_butbp* bp = (sp_butbp*)ptr;
            bp->lkb = f;
        }
    };

    struct ButterworthBandstop : public SoundPiper
    {
        ButterworthBandstop(sp_data * data, float cutoff, float bw) :
        SoundPiper(data,(create_func)sp_butbr_create,(init_func)sp_butbr_init,(create_func)sp_butbr_destroy,(compute_func)sp_butbr_compute) 
        {
            setFreq(cutoff);
            setBandwidth(bw);
        }
        void setFreq(float f) {
            sp_butbr *bp = (sp_butbr*)ptr;
            bp->freq = f;
        }
        void setBandwidth(float bw) {
            sp_butbr* bp = (sp_butbr*)ptr;
            bp->bw = bw;
        }
        void setLkf(float f) {
            sp_butbr* bp = (sp_butbr*)ptr;
            bp->lkf = f;
        }
        void setLkb(float f) {
            sp_butbr* bp = (sp_butbr*)ptr;
            bp->lkb = f;
        }
    };

    struct ButterworthHighpass : public SoundPiper
    {
        ButterworthHighpass(sp_data * data, float cutoff) :
        SoundPiper(data,(create_func)sp_buthp_create,(init_func)sp_buthp_init,(create_func)sp_buthp_destroy,(compute_func)sp_buthp_compute) 
        {
            setFreq(cutoff);            
        }
        void setFreq(float f) {
            sp_buthp *bp = (sp_buthp*)ptr;
            bp->freq = f;
        }        
        void setLkf(float f) {
            sp_buthp* bp = (sp_buthp*)ptr;
            bp->lkf = f;
        }        
    };

    struct ButterworthLowpass : public SoundPiper
    {
        ButterworthLowpass(sp_data * data, float cutoff) :
        SoundPiper(data,(create_func)sp_butlp_create,(init_func)sp_butlp_init,(create_func)sp_butlp_destroy,(compute_func)sp_butlp_compute) 
        {
            setFreq(cutoff);         
        }
        void setFreq(float f) {
            sp_butlp *bp = (sp_butlp*)ptr;
            bp->freq = f;
        }        
        void setLkf(float f) {
            sp_butlp* bp = (sp_butlp*)ptr;
            bp->lkf = f;
        }        
    };

    struct Clip : public SoundPiper
    {
        Clip(sp_data * data, float lim, float k1) :
        SoundPiper(data,(create_func)sp_clip_create,(init_func)sp_clip_init,(create_func)sp_clip_destroy,(compute_func)sp_clip_compute) 
        {
            setLimit(lim);
            setK1(k1);
        }
        void setLimit(float limit) {
            sp_clip *p = (sp_clip*)ptr;
            p->lim = limit;
        }
        void setK1(float k1) {
            sp_clip *p = (sp_clip*)ptr;
            p->k1 = k1;
        }
        
    };

    struct Clock : public SoundPiper
    {    
        Clock(sp_data * data, float bpm, float subdiv) :
        SoundPiper(data,(create_func)sp_clock_create,(init_func)sp_clock_init,(create_func)sp_clock_destroy,(compute_func)sp_clock_compute) 
        {        
            setBPM(bpm);
            setSubDivision(subdiv);
        }
        void setBPM(float _bpm) {
            sp_clock *p = (sp_clock*)ptr;
            p->bpm = _bpm;
        }
        void setSubDivision(float _sub) {
            sp_clock *p = (sp_clock*)ptr;
            p->subdiv = _sub;
        }
    };

    struct CombFilter : public SoundPiper
    {    
        CombFilter(sp_data * data, float revtime, float looptime) :
        SoundPiper(data,(create_func)sp_comb_create,(init_func)sp_comb_init,(create_func)sp_comb_destroy,(compute_func)sp_comb_compute) 
        {        

            setRevTime(revtime);
            setLoopTime(looptime);
        }
        void setRevTime(float _revtime) {
            sp_comb *p = (sp_comb*)ptr;
            p->revtime = _revtime;
        }
        void setLoopTime(float _looptime) {
            sp_comb *p = (sp_comb*)ptr;
            p->looptime = _looptime;
        }
    };

    struct Compressor : public SoundPiper
    {    
        Compressor(sp_data * data, float ratio, float thresh, float atk, float rel) :
        SoundPiper(data,(create_func)sp_compressor_create,(init_func)sp_compressor_init,(create_func)sp_compressor_destroy,(compute_func)sp_compressor_compute) 
        {        

            setRatio(ratio);
            setThreshold(thresh);
            setAttack(atk);
            setRelease(rel);
        }
        void setRatio(float _ratio) {
            sp_compressor *p = (sp_compressor*)ptr;
            *p->ratio = _ratio;
        }
        void setThreshold(float _thresh)
        {         
            sp_compressor *p = (sp_compressor*)ptr;
            *p->thresh = _thresh;
        }
        void setAttack(float _attack)
        {         
            sp_compressor *p = (sp_compressor*)ptr;
            *p->atk = _attack;
        }
        void setRelease(float _release)
        {         
            sp_compressor *p = (sp_compressor*)ptr;
            *p->rel = _release;
        }
    };


    struct Conv : public SoundPipe
    {    
        sp_conv * conv;
        // have to fill this table with the impulse response
        
        Conv(sp_data * data, sp_ftbls * ftbl, size_t partlen) : SoundPipe(data)        
        {                    
            sp_conv_create(&conv);
            sp_conv_init(data,conv,ftbl,partlen);
            
        }
        ~Conv() {
            if(conv) sp_conv_destroy(&conv);            
        }
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_conv_compute(sp,conv,&in,&out);
            return A * out;
        }
    };
    
    
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

    struct Crossfade : public SoundPipe
    {    
        sp_crossfade * fader;
        // have to fill this table with the impulse response
        
        Crossfade(sp_data * data, float pos=0.5) : SoundPipe(data)        
        {                    
            sp_crossfade_create(&fader);
            sp_crossfade_init(data,fader);
            
        }
        ~Crossfade() {
            if(fader) sp_crossfade_destroy(&fader);
        }
        void setPosition(float pos) {
            fader->pos = pos;
        }
        float Tick(float I1, float I2, float A=1, float X = 0, float Y = 1) {
            float in1 = I1;
            float in2 = I2;
            float out = 0;            
            sp_crossfade_compute(sp,fader,&in1,&in2,&out);
            return A * out;
        }
    };

    struct DCBlock : public SoundPiper
    {    
        DCBlock(sp_data * data) :
        SoundPiper(data,(create_func)sp_dcblock_create,(init_func)sp_dcblock_init,(create_func)sp_dcblock_destroy,(compute_func)sp_dcblock_compute) 
        {        
        
        }        
    };

    struct Delay : public SoundPipe
    {
        sp_delay * delay;
        
        Delay(sp_data * data, float time) 
        : SoundPipe(data)        
        {                    
            sp_delay_create(&delay);
            sp_delay_init(data,delay,time);
            
        }
        void setDelayTime(float t) {
            delay->time = t;
        }
        void setFeedback(float f) {
            delay->feedback = f;
        }
        ~Delay() {
            if(delay) sp_delay_destroy(&delay);
        }
        float Tick(float I1, float A=1, float X = 0, float Y = 1) {
            float in  = I1;            
            float out = 0;            
            sp_delay_compute(sp,delay,&in,&out);
            return A * out;
        }
    };    

    struct Diode : public SoundPiper
    {
        Diode(sp_data * data) :
        SoundPiper(data,(create_func)sp_diode_create,(init_func)sp_diode_init,(create_func)sp_diode_destroy,(compute_func)sp_diode_compute) 
        {        
        
        }        
    };

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

    struct Distortion : public SoundPiper
    {    
        Distortion(sp_data * data) :
        SoundPiper(data,(create_func)sp_dist_create,(init_func)sp_dist_init,(create_func)sp_dist_destroy,(compute_func)sp_dist_compute) 
        {        
        
        }        
        void setPreGain(float pre) {
            sp_dist * p = (sp_dist*)ptr;
            p->pregain = pre;
        }
        void setPostGain(float post) {
            sp_dist * p = (sp_dist*)ptr;
            p->postgain = post;
        }
        void setShape1(float shape) {
            sp_dist * p = (sp_dist*)ptr;
            p->shape1 = shape;
        }
        void setShape2(float shape) {
            sp_dist * p = (sp_dist*)ptr;
            p->shape2 = shape;
        }
    };    

    struct Metronome : public SoundPiper
    {    
        Metronome(sp_data * data, float time) :
        SoundPiper(data,(create_func)sp_dist_create,(init_func)sp_dist_init,(create_func)sp_dist_destroy,(compute_func)sp_dist_compute) 
        {        
            setTime(time);
        }        
        void setTime(float t) {
            sp_metro * p = (sp_metro*)ptr;
            p->time = t;
        }
    };
    
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
    
    struct Dust : public SoundPiper
    {    
        Dust(sp_data * data) :
        SoundPiper(data,(create_func)sp_dust_create,(init_func)sp_dust_init,(create_func)sp_dust_destroy,(compute_func)sp_dust_compute) 
        {        
            
        }        
        void setAmp(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->amp = x;
        }
        void setDensity(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->density = x;
        }
        void setDensity0(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->density0 = x;
        }
        void setThreshold(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->thresh = x;
        }
        void setScale(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->scale = x;
        }
        void setOneDSR(float x) {
            sp_dust * p = (sp_dust*)ptr;
            p->onedsr = x;
        }
        void setBipolar(bool x) {
            sp_dust * p = (sp_dust*)ptr;
            p->bipolar = x;
        }
    };


    struct EQFilter : public SoundPiper
    {    
        EQFilter(sp_data * data, float f, float b, float g) :
        SoundPiper(data,(create_func)sp_eqfil_create,(init_func)sp_eqfil_init,(create_func)sp_eqfil_destroy,(compute_func)sp_eqfil_compute) 
        {        
            setFreq(f);
            setBandwidth(b);
            setGain(g);
        }        
        void setFreq(float x) {
            sp_eqfil * p = (sp_eqfil*)ptr;
            p->freq = x;
        }
        void setBandwidth(float x) {
            sp_eqfil * p = (sp_eqfil*)ptr;
            p->bw = x;
        }
        void setGain(float x) {
            sp_eqfil * p = (sp_eqfil*)ptr;
            p->gain = x;
        }        
    };

    struct DTrig : public SoundPipe
    {    
        sp_dtrig * dtrig;
        // have to fill this table with the impulse response
        
        DTrig(sp_data * data, sp_ftbl * ftbl, size_t partlen) : SoundPipe(data)        
        {                    
            sp_dtrig_create(&dtrig);
            sp_conv_init(data,dtrig,ftbl,partlen);
            
        }
        ~DTrig() {
            if(dtrig) sp_conv_destroy(&dtrig);            
        }
        void setDelay(float x)
        {
            dtrig->delay = x;
        }
        void setScale(float x)
        {
            dtrig->scale = x;
        }
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_dtrig_compute(sp,conv,&in,&out);
            return A * out;
        }
    };

    struct Exponential : public SoundPiper
    {    
        Exponential(sp_data * data, float a, float d, float b) :
        SoundPiper(data,(create_func)sp_eqfil_create,(init_func)sp_eqfil_init,(create_func)sp_eqfil_destroy,(compute_func)sp_eqfil_compute) 
        {        
            setA(f);
            setB(b);
            setDur(g);
        }        
        void setA(float x) {
            sp_expon * p = (sp_expon*)ptr;
            p->a = x;
        }
        void setB(float x) {
            sp_expon * p = (sp_expon*)ptr;
            p->b = x;
        }
        void setDur(float x) {
            sp_expon * p = (sp_expon*)ptr;
            p->dur = x;
        }        
    };

    struct FOF : public SoundPipe
    {    
        sp_fof * fof;
            
        FOF(sp_data * data, sp_ftbl * sine, sp_ftbl * win, int overlaps, float iphs) : SoundPipe(data)        
        {                    
            sp_fof_create(&fof);
            sp_fof_init(data,fof,sine,win,overlaps,iphs);            
        }
        ~DTrig() {
            if(fof) sp_conv_destroy(&fof);            
        }        
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_fof_compute(sp,fof,&in,&out);
            return A * out;
        }
    };

    struct FOFilt : public SoundPiper
    {    
        FOFilt(sp_data * data, float f, float a, float d, float istor)
        SoundPiper(data,(create_func)sp_eqfil_create,(init_func)sp_eqfil_init,(create_func)sp_eqfil_destroy,(compute_func)sp_eqfil_compute) 
        {        
            setFreq(f);
            setAtk(a);
            setDec(d);
            setIStor(istor);
        }        
        void setFreq(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->freq = x;
        }
        void setAtk(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->atk = x;
        }
        void setDec(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->dec = x;
        }        
        void setIStor(float x) {
            sp_fofilt * p = (sp_fofilt*)ptr;
            p->istor = x;
        }        
    };


    struct Fog : public SoundPipe
    {    
        sp_fog * fog;
        Fog(sp_data * data, sp_ftbl * wav, sp_ftbl * win, int iolaps, float iphs) :
        SoundPipe(data)
        {        
            sp_fog_create(&fog);
            sp_fog_init(data,fog,wav,win,iolaps,iphs);
        }        
        ~Fog() {
            sp_fog_destroy(&fog);
        }
        float Tick(float I,float A=1, float X = 0, float Y = 0) {
            float in = 0;
            float out = 0;            
            sp_fog_compute(sp,fog,&in,&out);
            return A * out;
        }
        
    };

}
