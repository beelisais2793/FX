#pragma once

namespace Analog::Envelopes
{
    struct F2PAR
    {
        StateVariableFilter filter;
        DspFloatType         attack_rate;        
        DspFloatType         release_rate;
        DspFloatType         current;
        enum { ATTACK,RELEASE,IDLE};
        int state = IDLE;

        F2PAR(DspFloatType a, DspFloatType r) :
        filter(1/0.01,sampleRate,0.5)
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
        DspFloatType Tick()
        {
            DspFloatType r = filter.Tick(current);
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

    struct F2PASR
    {
        StateVariableFilter filter;
        DspFloatType         attack_rate;    
        DspFloatType         sustain;
        DspFloatType         release_rate;
        DspFloatType         current;
        enum { ATTACK,SUSTAIN,RELEASE,IDLE};
        int state = IDLE;

        F2PASR(DspFloatType a, DspFloatType s, DspFloatType r)
        :
        filter(1/0.01,sampleRate,0.5)
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
        DspFloatType Tick()
        {
            DspFloatType r = filter.Tick(current);
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

    struct F2PADSR
    {
        StateVariableFilter filter;
        DspFloatType         attack_rate;
        DspFloatType         decay_rate;
        DspFloatType         sustain;
        DspFloatType         release_rate;
        DspFloatType         current;
        enum { ATTACK,DECAY,SUSTAIN,RELEASE,IDLE};
        int state = IDLE;
    
        F2PADSR(DspFloatType a, DspFloatType d, DspFloatType s, DspFloatType r)
        : filter(1/0.01,sampleRate,0.5)
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
        DspFloatType Tick()
        {
            DspFloatType r = filter.Tick(current);
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