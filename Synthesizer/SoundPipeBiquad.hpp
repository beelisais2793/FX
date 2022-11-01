#pragma once
#include "SoundPipe.hpp"

namespace SoundPipe
{
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
}