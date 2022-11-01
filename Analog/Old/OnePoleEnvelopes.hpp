#pragma once

#include "Filters/CSmoothFilter.hpp"

namespace Envelopes
{
    struct FAR
    {
        Filters::CSmoothFilter filter;
        float         attack_rate;        
        float         release_rate;
        float         current;
        enum { ATTACK,RELEASE,IDLE};
        int state = IDLE;

        FAR(float a, float r)
        {
            attack_rate = 1/a;                
            release_rate = 1/r;
            current     = 0;        
        }
        void noteOn() {
            state = ATTACK;        
            current = 1;
            filter.setCutoff(attack_rate);
        }
        void noteOff() {
            state = RELEASE;
            current = 0;
            filter.setCutoff(release_rate);
        }
        float Tick()
        {
            float r = filter.process(current);
            if(state == IDLE) return 0;
            if(state == ATTACK) {
                if(r >= 1.0f) {
                    state = RELEASE;                
                }            
            }
            else if(state == RELEASE) {
                if(r <= 0.0f) {
                    state = IDLE;
                    current = 0;            
                    r = 0;
                }
            }        
            return r;
        }
    };

    struct FASR
    {
        Filters::CSmoothFilter filter;
        float         attack_rate;    
        float         sustain;
        float         release_rate;
        float         current;
        enum { ATTACK,SUSTAIN,RELEASE,IDLE};
        int state = IDLE;

        FASR(float a, float s, float r)
        {
            attack_rate = 1/a;        
            sustain     = s;
            release_rate = 1/r;
            current     = 0;        
        }
        void noteOn() {
            state = ATTACK;        
            current = 1;
            filter.setCutoff(attack_rate);
        }
        void noteOff() {
            state = RELEASE;
            current = 0;
            filter.setCutoff(release_rate);
        }
        float Tick()
        {
            float r = filter.process(current);
            if(state == IDLE) return 0;
            if(state == ATTACK) {
                if(r >= sustain) {
                    state = SUSTAIN;
                    current = sustain;                
                }            
            }
            else if(state == RELEASE) {
                if(r <= 0.0f) {
                    state = IDLE;
                    current = 0;            
                    r = 0;
                }
            }
            else if(state == SUSTAIN) {            
                r = sustain;
            }
            return r;
        }
    };

    struct FADSR
    {
        Filters::CSmoothFilter filter;
        float         attack_rate;
        float         decay_rate;
        float         sustain;
        float         release_rate;
        float         current;
        enum { ATTACK,DECAY,SUSTAIN,RELEASE,IDLE};
        int state = IDLE;
    
        FADSR(float a, float d, float s, float r)
        {
            attack_rate = 1/a;
            decay_rate  = 1/d;
            sustain     = s;
            release_rate = 1/r;
            current     = 0;        
        }
        void noteOn() {
            state = ATTACK;        
            current = 1;
            filter.setCutoff(attack_rate);
        }
        void noteOff() {
            state = RELEASE;
            current = 0;
            filter.setCutoff(release_rate);
        }
        float Tick()
        {
            float r = filter.process(current);
            if(state == IDLE) return 0;
            if(state == ATTACK) {
                if(r >= 1.0f) {
                    state = DECAY;
                    current = sustain;
                    filter.setCutoff(decay_rate);
                }            
            }
            else if(state == DECAY) {
                if(r <= sustain) {
                    state = SUSTAIN;                                                
                }            
            }
            else if(state == RELEASE) {
                if(r <= 0.0f) {
                    state = IDLE;
                    current = 0;            
                    r = 0;
                }
            }
            else if(state == SUSTAIN) {            
                r = sustain;
            }
            return r;
        }
    };
}