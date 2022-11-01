// this is not for audio unless you want to polyblip it

#pragma once

#include "core.h"

inline float function_noise() 
{
    // kaka
    // White Noise
    // always the same unless you seed it.
    float r = std::rand() / (float)RAND_MAX;
    output = r * 2 - 1;
    return (output);
}




struct SineFunction
{
    enum Polarity 
    {
        BIPOLAR,
        POSITIVE,
        NEGATIVE,
    }
    polarity = BIPOLAR;

    SineFunction(float f, float sr = 44100): frequenc(f), startphase(0), phase(0.0), sampleRate(sr) {}

    void phaseReset(float phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {
        // can just do an fmod
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

   float function() 
   {
        output = sin(phase * 2*M_PI);    
        return output;
    }

    // I = dc offset
    // X = frequency modulation
    // Y = phase modulation
    float Tick(float I=0, float A = 1, float X = -1, float Y = 1) {
        float r = A*function();
        if(r < X) r = X;
        if(r > Y) r = Y;
        PhaseIncrement();      
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);
        return A*(r+I);
    }

    float sampleRate = 44100;
    float frequency;
    float phase;    
    float startphase;
};

struct PhasorFunction
{
    PhasorFunction(float sr = 44100) phase(0.0), sampleRate(sr) {}

    void phaseReset(float phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {
        // can just do an fmod
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

   
    float Tick(float I=0, float A = 1, float X = -1, float Y = 1) {
        float r = A*phase;
        if(r < X) r = X;
        if(r > Y) r = Y;
        PhaseIncrement();      
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);
        return A*r;
    }

    float sampleRate = 44100;
    float frequency;
    float phase;    
};


struct Function
{
    enum Polarity 
    {
        BIPOLAR,
        POSITIVE,
        NEGATIVE,
    }
    polarity = BIPOLAR;

    std::function<float (float)> func;

    Function(float f,float sr = 44100,std::function<float (float)> function)) phase(0.0), sampleRate(sr) {
        func = function;
    }

    void phaseReset(float phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {
        // can just do an fmod
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

    float phasor(float frequency, float startphase, float endphase) {
           
        output = phase;
        
        if (phase < startphase) {
            phase = startphase;
        }
        
        if (phase >= endphase)
            phase = startphase;
        
        phase += ((endphase - startphase) / (sampleRate / frequency));
        
        return (output);
    }

    
    float Tick(float I=0, float A = 1, float X = 0, float Y = 0) {
        float p = phase;
        phase = phasor(f + X*frequency) + Y;
        phase = std::fmod(phase,1);
        float r = func();
        phase = p;        
        PhaseIncrement();      
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);
        return A*r;
    }

    float sampleRate = 44100;
    float frequency;
    float phase;    
};


struct SineGenerator : public Function
{
    SineGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ return std::sin(phase * 2.0f*M_PI); }) {}
};
struct CosGenerator : public Function
{
    CosGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ return std::cos(phase * 2.0f*M_PI); }) {}
};
struct TanGenerator : public Function
{
    TanGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ return std::tan(phase * 2.0f*M_PI); }) {}
};
struct PhasorGenerator : public Function
{
    TanGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ return phase;}) {}
};
struct SquareGenerator : public Function
{
    SquareGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ return phase < 0.5f ? -1.0f : 1.0f;}) {}
};
struct SawGenerator : public Function
{
    SawGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ return phase * 2.0f - 1.0f;}) {}
};
struct TriangleGenerator : public Function
{
    TriangleGenerator(T freq, T sr = 44100.0f) : Function(freq,sr,[](float phase){ 
        if (phase <= 0.5f) {
            return (phase - 0.25f) * 4;
        } else {
            return ((1.0f - phase) - 0.25f) * 4;
        }}) {}
};


struct FunctionGenerator
{
    enum Type
    {
        NOISE,
        SINEWAVE,   
        COSWAVE,
        PHASOR,
        SQUARE,
        PULSE
        SAW,
        REVERSE_SAW,
        TRIANGLE,
    } 
    type = SINEWAVE;

    enum Polarity 
    {
        BIPOLAR,
        POSITIVE,
        NEGATIVE,
    }
    polarity = BIPOLAR,

    FunctionGenerator(float f, float sr=44100) : frequency(f),sampleRate(sr),startphase(0),phase(0.0) {}
        
    void phaseReset(float phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

    float phasor(float frequency, float startphase, float endphase) {
        
        output = phase;
        
        if (phase < startphase) {
            phase = startphase;
        }
        
        if (phase >= endphase)
            phase = startphase;
        
        phase += ((endphase - startphase) / (sampleRate / frequency));
        
        return (output);
    }
        
    float noise() {
        // White Noise
        // always the same unless you seed it.
        float r = std::rand() / (float)RAND_MAX;
        output = r * 2 - 1;
        return (output);
    }

    float sinewave() {
        output = sin(phase * TWOPI);    
        return output;
    }

    float coswave() {
        output = cos(phase * TWOPI);    
        return output;
    }

    float phasor() {
        output = phase;    
        return output;
    }

    float square() {
        output = phase < 0.5f ? -1.0f : 1.0f;    
        return output;
    }

    float pulse() {
        if (phase < duty)
            output = -1.f;
        if (phase > duty)
            output = 1.f;
        return output;
    }
    float saw() 
    { 
        return phase * 2.0f - 1.0f; 
    }

    float triangle() {        
        if (phase <= 0.5f) {
            output = (phase - 0.25f) * 4;
        } else {
            output = ((1.0f - phase) - 0.25f) * 4;
        }
        return (output);
    }

    float function() {
        switch(type)
        {
        case SINWAVE: return sinwave(frequency);
        case COSNWAVE: return coswave(frequency);
        case PHASOR: return phasor(frequency);
        case SQUARE: return square(frequency);
        case PULSE: return pulse(frequency);
        case SAW: return saw(frequency);
        // pesuro reverse
        case REVERSE_SAW: return -saw(frequency);
        }
        return triangle(frequency);
    }
  
    float Tick(float I=0, float A = 1, float X = 0, float Y = 0) {
        
        float p = phase;
        phase = phasor(f + X*frequency) + Y;
        phase = std::fmod(phase,1);
        float r = function();
        phase = p;        
        PhaseIncrement();      
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);                
        return A*(r+I);
    }

    float frequency;
    float phase;
    float duty = 0.5f;
    float startphase;
    float endphase;
    float output;
    float tri;
};
