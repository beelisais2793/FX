#pragma once

#include <cmath>

#define TWOPI 2*M_PI

inline double function_noise() 
{
    // remeber to seed
    double r = std::rand() / (double)RAND_MAX;
    return r * 2 - 1;    
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

    SineFunction(double f, double sr = 44100): frequency(f), startphase(0), phase(0.0), sampleRate(sr) {}

    void phaseReset(double phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {
        // can just do an fmod
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

    double function() 
    {
        return sin(phase * 2*M_PI);            
    }

    double phasor(double frequency, double startphase=0, double endphase=1) {
           
        double output = phase;
        
        if (phase < startphase) {
            phase = startphase;
        }
        
        if (phase >= endphase)
            phase = startphase;
        
        phase += ((endphase - startphase) / (sampleRate / frequency));
        
        return (output);
    }

    // I = dc offset
    // X = frequency modulation
    // Y = phase modulation
    double Tick(double I=0, double A = 1, double X = -1, double Y = 1) {
        double r = A*function();
        double p = phase;
        phase = phasor(frequency + 0.5*X*frequency) + Y + phase;
        phase = std::fmod(phase,1);        
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);
        phase = p;
        phaseIncrement();      
        return A*(r+I);
    }

    double sampleRate = 44100;
    double frequency;
    double phase;    
    double startphase;
};

struct PhasorFunction
{
    enum Polarity 
    {
        BIPOLAR,
        POSITIVE,
        NEGATIVE,
    }
    polarity = BIPOLAR;

    PhasorFunction(double f, double sr = 44100) : frequency(f),phase(0.0), sampleRate(sr) {}

    void phaseReset(double phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }
    double phasor(double frequency, double startphase=0, double endphase=1) {
           
        double output = phase;
        
        if (phase < startphase) {
            phase = startphase;
        }
        
        if (phase >= endphase)
            phase = startphase;
        
        phase += ((endphase - startphase) / (sampleRate / frequency));
        
        return (output);
    }

    inline void phaseIncrement() {
        // can just do an fmod
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

   
    double Tick(double I=0, double A = 1, double X = -1, double Y = 1) {
        double r = A*phase;
        double p = phase;
        phase = phasor(frequency + 0.5*X*frequency) + Y + phase;
        phase = std::fmod(phase,1);        
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);
        phase = p;
        phaseIncrement();      
        return A*r;
    }

    double sampleRate = 44100;
    double frequency;
    double phase;    
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

    std::function<double (double)> func;

    Function(double f,double sr,std::function<double (double)> function) 
    : phase(0.0), sampleRate(sr) {
        func = function;
    }

    void phaseReset(double phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {
        // can just do an fmod
        if (phase >= 1.0f)
        phase -= 1.0f;
        phase += (1.f / (sampleRate / frequency));
    } 

    double phasor(double frequency, double startphase=0, double endphase=1) {
           
        double output = phase;
        
        if (phase < startphase) {
            phase = startphase;
        }
        
        if (phase >= endphase)
            phase = startphase;
        
        phase += ((endphase - startphase) / (sampleRate / frequency));
        
        return (output);
    }

    
    double Tick(double I=0, double A = 1, double X = 0, double Y = 0) {
        double p = phase;
        phase = phasor(frequency + 0.5*X*frequency) + Y + phase;
        phase = std::fmod(phase,1);
        double r = func(phase);
        phase = p;        
        phaseIncrement();      
        if(polarity == POSITIVE) r = std::abs(r);
        else if(polarity == NEGATIVE) r = -std::abs(r);
        return A*r;
    }

    double sampleRate = 44100;
    double frequency;
    double phase;    
};


struct SineGenerator : public Function
{
    SineGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ return std::sin(phase * 2.0f*M_PI); }) {}
};
struct CosGenerator : public Function
{
    CosGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ return std::cos(phase * 2.0f*M_PI); }) {}
};
struct TanGenerator : public Function
{
    TanGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ return std::tan(phase * 2.0f*M_PI); }) {}
};
struct PhasorGenerator : public Function
{
    PhasorGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ return phase;}) {}
};
struct SquareGenerator : public Function
{
    SquareGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ return phase < 0.5f ? -1.0f : 1.0f;}) {}
};
struct SawGenerator : public Function
{
    SawGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ return phase * 2.0f - 1.0f;}) {}
};
struct TriangleGenerator : public Function
{
    TriangleGenerator(double freq, double sr = 44100.0f) : Function(freq,sr,[](double phase){ 
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
        PULSE,
        SAW,
        REVERSE_SAW,
        TRIANGLE,
    } 
    type = SAW;

    enum Polarity
    {
        BIPOLAR,
        POSITIVE,
        NEGATIVE,
    }
    polarity = POSITIVE;

    FunctionGenerator(double f, double sr=44100) : frequency(f),sampleRate(sr),startphase(0),phase(0.0) {
        inc = f/sr;
    }
    void setFrequency(double f)
    {
        frequency = f;
        inc = f/sampleRate;
    }
    void phaseReset(double phaseIn) {
        // This allows you to set the phase of the oscillator to anything you like.
        phase = phaseIn;
    }

    inline void phaseIncrement() {        
        phase = fmod(phase+inc,1);
    } 

    double phasor(double frequency, double startphase=0, double endphase=1) {
        
        output = phase;
        
        if (phase < startphase) {
            phase = startphase;
        }
        
        if (phase >= endphase)
            phase = startphase;
        
        phase += ((endphase - startphase) / (sampleRate / frequency));
        
        return (output);
    }
        
    double noise() {
        // White Noise
        // always the same unless you seed it.
        double r = std::rand() / (double)RAND_MAX;
        return r;
    }

    double sinewave() {
        return 0.5 + 0.5*sin(phase * TWOPI);            
    }

    double coswave() {
        return 0.5 + 0.5*cos(phase * TWOPI);            
    }

    double phasor() {
        return phase;
    }

    double square() {
        return phase < 0.5f ? 0 : 1.0f;            
    }

    double pulse() {
        double output;
        if (phase < duty)
            output = 0;
        if (phase > duty)
            output = 1.f;
        return output;
    }
    double saw() 
    { 
        return phase;
    }

    double triangle() {     
        double output;   
        if (phase <= 0.5f) {
            output = (phase - 0.25f) * 4;
        } else {
            output = ((1.0f - phase) - 0.25f) * 4;
        }
        return (output);
    }

    double function() {
        switch(type)
        {
        case SINEWAVE: return sinewave();
        case COSWAVE: return coswave();
        case PHASOR: return phasor();
        case SQUARE: return square();
        case PULSE: return pulse();
        case SAW: return saw();
        // pesuro reverse
        case REVERSE_SAW: return 1-saw();
        }
        return triangle();
    }
  
    double Tick(double I=0, double A = 1, double X = 0, double Y = 0) {        
        //double p = phase;
        //phase = phasor(frequency + 0.5*X*frequency) + Y + phase;
        //phase = std::fmod(phase,1);
        double r = function();
        //phase = p;        
        phaseIncrement();      
        if(polarity == POSITIVE) return r;
        else if(polarity == NEGATIVE) return -r;
        return 2*r-1;
    }

    double sampleRate;
    double frequency;
    double phase;
    double duty = 0.5f;
    double startphase;
    double endphase;
    double output;
    double tri;
    double inc;
};


