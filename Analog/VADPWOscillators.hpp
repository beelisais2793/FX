#pragma once

#include <cmath>
#include "SoundObject.hpp"

namespace Analog::Oscillators::DPW
{
    struct DPWSaw : public OscillatorProcessor
    {
        DspFloatType freq,fs,inc;
        DspFloatType phase,lastPhase;
        DspFloatType lastValue,position;
        DspFloatType scaleFactor;
        DPWSaw() : OscillatorProcessor()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValue = phase = lastPhase = position = 0.0f;
            scaleFactor = sampleRate / (4.0f * freq);
        }
        void setFrequency(DspFloatType f) {
            freq = f;
            inc  = f/fs;
            scaleFactor = sampleRate / (4.0f * freq);
        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_FREQ) setFrequency(v);
            else printf("No port %d\n", port);
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {                                    
            position += phase - lastPhase;
            lastPhase = phase;

            position = fmod(position, 1.0f);

            DspFloatType value = position * 2 - 1;
            value = value * value;
            
            DspFloatType out = scaleFactor * (value - lastValue);
            lastValue = value;

            phase = fmod(phase + inc,1.0f);
            return A*out;
        }   
    };

    struct DPWPulse : public OscillatorProcessor
    {
        DspFloatType freq,fs,inc;
        DspFloatType phase,lastPhase;
        DspFloatType lastValueA,lastValueB,position;
        DspFloatType positionA,positionB;
        DspFloatType scaleFactor;

        DPWPulse() : OscillatorProcessor()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValueA = lastValueB = phase = lastPhase = position = 0.0f;
            positionA = 0.5f;
            positionB = 0.5f;
            scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
            phase = 0.5;
        }
        void setFrequency(DspFloatType f) {
            freq = f;
            inc  = f/fs;
            scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
        }
        void setDuty(DspFloatType d) {
            phase = clamp(d,0.01f,0.99f);
        }
        enum {
            PORT_FREQ,
            PORT_DUTY,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_FREQ: setFrequency(v); break;
                case PORT_DUTY: setDuty(v); break;
                default: printf("No port %d\n", port);
            }            
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
                            
            positionB += phase - lastPhase;
            lastPhase = phase;

            positionA = fmod(positionA, 1.0f);
            positionB = fmod(positionB, 1.0f);

            DspFloatType valueA = positionA * 2.0f - 1.0f;
            DspFloatType valueB = positionB * 2.0f - 1.0f;
            valueA = valueA * valueA;
            valueB = valueB * valueB;
            DspFloatType out = ((valueA - lastValueA) -(valueB - lastValueB)) * scaleFactor;
            lastValueA = valueA;
            lastValueB = valueB;

            positionA += freq * invSampleRate;
            positionB += freq * invSampleRate;

            return out;        
        }
    };

    struct DPWTriangle : public OscillatorProcessor
    {
        DspFloatType freq,fs,inc;
        DspFloatType phase,lastPhase;
        DspFloatType lastValue,position;    
        DspFloatType scaleFactor;

        DPWTriangle() : OscillatorProcessor()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValue = phase = lastPhase = position = 0.0f;
            position = 0.0f;        
            scaleFactor =  sampleRate / (2.0f * freq);
            phase = 0.5;
        }
        void setFrequency(DspFloatType f) {
            freq = f;
            inc  = f/fs;
            scaleFactor =  sampleRate / (2.0f * freq);
        }
        void setDuty(DspFloatType d) {
            phase = clamp(d,0.01f,0.99f);
        }
        enum {
            PORT_FREQ,
            PORT_DUTY,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_FREQ: setFrequency(v); break;
                case PORT_DUTY: setDuty(v); break;
                default: printf("No port %d\n", port);
            }            
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {        
            position += phase - lastPhase;
            lastPhase = phase;
            position = fmod(position, 1.0f);                
            DspFloatType out = std::abs(position - 0.5) * 4 - 1;                
            position += freq * invSampleRate;        
            return A*out;
        }
    };
}