#pragma once

#include <cmath>

namespace Analog
{
    struct DPWSaw : public OscillatorProcessor
    {
        float freq,fs,inc;
        float phase,lastPhase;
        float lastValue,position;
        float scaleFactor;
        DPWSaw() : OscillatorProcessor()
        {
            freq = 440.0f;
            fs   = sampleRate;
            inc  = freq/fs;
            lastValue = phase = lastPhase = position = 0.0f;
            scaleFactor = sampleRate / (4.0f * freq);
        }
        void setFrequency(float f) {
            freq = f;
            inc  = f/fs;
            scaleFactor = sampleRate / (4.0f * freq);
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1)
        {                                    
            position += phase - lastPhase;
            lastPhase = phase;

            position = fmod(position, 1.0f);

            float value = position * 2 - 1;
            value = value * value;
            
            float out = scaleFactor * (value - lastValue);
            lastValue = value;

            phase = fmod(phase + inc,1.0f);
            return A*out;
        }   
    };

    struct DPWPulse : public OscillatorProcessor
    {
        float freq,fs,inc;
        float phase,lastPhase;
        float lastValueA,lastValueB,position;
        float positionA,positionB;
        float scaleFactor;

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
        void setFrequency(float f) {
            freq = f;
            inc  = f/fs;
            scaleFactor = 0.5f * sampleRate /(4.0f * freq);    
        }
        void setDuty(float d) {
            phase = clamp(d,0.01f,0.99f);
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
                            
            positionB += phase - lastPhase;
            lastPhase = phase;

            positionA = fmod(positionA, 1.0f);
            positionB = fmod(positionB, 1.0f);

            float valueA = positionA * 2.0f - 1.0f;
            float valueB = positionB * 2.0f - 1.0f;
            valueA = valueA * valueA;
            valueB = valueB * valueB;
            float out = ((valueA - lastValueA) -(valueB - lastValueB)) * scaleFactor;
            lastValueA = valueA;
            lastValueB = valueB;

            positionA += freq * invSampleRate;
            positionB += freq * invSampleRate;

            return out;        
        }
    };

    struct DPWTriangle : public OscillatorProcessor
    {
        float freq,fs,inc;
        float phase,lastPhase;
        float lastValue,position;    
        float scaleFactor;

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
        void setFrequency(float f) {
            freq = f;
            inc  = f/fs;
            scaleFactor =  sampleRate / (2.0f * freq);
        }
        void setDuty(float d) {
            phase = clamp(d,0.01f,0.99f);
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1)
        {        
            position += phase - lastPhase;
            lastPhase = phase;
            position = fmod(position, 1.0f);                
            float out = std::abs(position - 0.5) * 4 - 1;                
            position += freq * invSampleRate;        
            return A*out;
        }
    };
}