#pragma once

#include <iostream>
#include <cmath>
#include <functional>
#include <random>
#include "SoundObject.hpp"

#define TWOPI 2*M_PI

namespace Oscillators::Generators
{
    inline DspFloatType function_noise() 
    {
        // remeber to seed
        DspFloatType r = std::rand() / (DspFloatType)RAND_MAX;
        return r * 2 - 1;    
    }

    struct SineFunction : public GeneratorProcessor
    {
        enum Polarity 
        {
            BIPOLAR,
            POSITIVE,
            NEGATIVE,
        }
        polarity = BIPOLAR;

        DspFloatType min=-1;
        DspFloatType max=1;

        SineFunction(DspFloatType f, DspFloatType sr = 44100): GeneratorProcessor(), frequency(f), startphase(0), phase(0.0), sampleRate(sr) {}

        void phaseReset(DspFloatType phaseIn) {
            // This allows you to set the phase of the oscillator to anything you like.
            phase = phaseIn;
        }

        inline void phaseIncrement() {
            // can just do an fmod
            if (phase >= 1.0f)
            phase -= 1.0f;
            phase += (1.f / (sampleRate / frequency));
        } 

        DspFloatType function() 
        {
            return sin(phase * 2*M_PI);            
        }
        void setRange(DspFloatType _min, DspFloatType _max) {
            min = _min;
            max = _max;
        }
        DspFloatType phasor(DspFloatType frequency, DspFloatType startphase=0, DspFloatType endphase=1) {
            
            DspFloatType output = phase;
            
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
        DspFloatType Tick(DspFloatType Index=1, DspFloatType A = 1, DspFloatType X = -1, DspFloatType Y = 1) {
            DspFloatType r = A*function();
            DspFloatType p = phase;
            phase = phasor(Index*(frequency + X*frequency)) + (Y*phase);
            phase = std::fmod(phase,1);        
            if(polarity == POSITIVE) r = std::abs(r);
            else if(polarity == NEGATIVE) r = -std::abs(r);
            phase = p;
            phaseIncrement();      
            r = (max-min)*r + min;
            return A*(r);
        }

        DspFloatType sampleRate = 44100;
        DspFloatType frequency;
        DspFloatType phase;    
        DspFloatType startphase;
    };

    struct Phasor : public GeneratorProcessor
    {
        enum Polarity 
        {
            BIPOLAR,
            POSITIVE,
            NEGATIVE,
        }
        polarity = BIPOLAR;

        Phasor(DspFloatType f, DspFloatType sr = 44100) : GeneratorProcessor(), frequency(f),phase(0.0), sampleRate(sr) {}

        void phaseReset(DspFloatType phaseIn) {
            // This allows you to set the phase of the oscillator to anything you like.
            phase = phaseIn;
        }
        DspFloatType phasor(DspFloatType frequency, DspFloatType startphase=0, DspFloatType endphase=1) {
            
            DspFloatType output = phase;
            
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

    
        DspFloatType Tick(DspFloatType I=1, DspFloatType A = 1, DspFloatType X = 1, DspFloatType Y = 1) {
            DspFloatType r = A*phase;
            DspFloatType p = phase;
            phase = phasor(I*(frequency*X)) + (Y*phase);
            phase = std::fmod(phase,1);        
            if(polarity == POSITIVE) r = std::abs(r);
            else if(polarity == NEGATIVE) r = -std::abs(r);
            phase = p;
            phaseIncrement();      
            return A*r;
        }

        DspFloatType sampleRate = 44100;
        DspFloatType frequency;
        DspFloatType phase;    
    };


    struct Function : public GeneratorProcessor
    {
        enum Polarity 
        {
            BIPOLAR,
            POSITIVE,
            NEGATIVE,
        }
        polarity = BIPOLAR;
        DspFloatType min=-1,max=1;

        std::function<DspFloatType (DspFloatType)> func;

        Function(DspFloatType f,DspFloatType sr,std::function<DspFloatType (DspFloatType)> function) 
        : GeneratorProcessor(),
        phase(0.0), sampleRate(sr) {
            func = function;            
        }

        void phaseReset(DspFloatType phaseIn) {
            // This allows you to set the phase of the oscillator to anything you like.
            phase = phaseIn;
        }

        inline void phaseIncrement() {
            // can just do an fmod
            if (phase >= 1.0f)
                phase -= 1.0f;
            phase += (frequency / sampleRate);
        } 

        DspFloatType phasor(DspFloatType frequency, DspFloatType startphase=0, DspFloatType endphase=1) {
            
            DspFloatType output = phase;
            this->frequency = frequency;
            if (phase < startphase) {
                phase = startphase;
            }
            
            if (phase >= endphase)
                phase = startphase;
            
            phase += ((endphase - startphase) * (frequency/sampleRate));
            
            return (output);
        }
        void setFrequency(DspFloatType f) {
            frequency = f;
            phase = phasor(f);
        }
        void setRange(DspFloatType _min, DspFloatType _max) {
            min = _min;
            max = _max;
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A = 1, DspFloatType X = 1, DspFloatType Y = 1) {
            //DspFloatType p = phase;
            //phase = phasor(I*(frequency + 0.5*X*frequency)) + (Y*phase);
            //phase = std::fmod(phase,1);            
            DspFloatType r = func(phase);
            //phase = p;        
            phaseIncrement();                  
            if(polarity == POSITIVE) r = std::abs(r);
            else if(polarity == NEGATIVE) r = -std::abs(r);
            r = (max-min)*r + min;
            return A*r;
        }

        DspFloatType sampleRate = 44100;
        DspFloatType frequency;
        DspFloatType phase;    
    };


    struct SineGenerator : public Function
    {
        SineGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ return std::sin(phase * 2.0f*M_PI); }) 
        {
            setFrequency(freq);
        }
    };
    struct CosGenerator : public Function
    {
        CosGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ return std::cos(phase * 2.0f*M_PI); }) {
            setFrequency(freq);            
        }
    };
    struct TanGenerator : public Function
    {
        TanGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ return std::tan(phase * 2.0f*M_PI); }) {
            setFrequency(freq);            
        }
    };
    struct PhasorGenerator : public Function
    {
        PhasorGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ return phase;}) {
            setFrequency(freq);
        }
    };
    struct SquareGenerator : public Function
    {
        SquareGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ return phase < 0.5f ? -1.0f : 1.0f;}) {
            setFrequency(freq);
        }
    };
    struct SawGenerator : public Function
    {
        SawGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ return phase * 2.0f - 1.0f;}) {
            setFrequency(freq);
        }
    };
    struct TriangleGenerator : public Function
    {
        TriangleGenerator(DspFloatType freq, DspFloatType sr = 44100.0f) : Function(freq,sr,[](DspFloatType phase){ 
            if (phase <= 0.5f) {
                return (phase - 0.25f) * 4;
            } else {
                return ((1.0f - phase) - 0.25f) * 4;
            }}) {
                setFrequency(freq);
            }
    };
}

namespace Oscillators::Functions
{
    struct FunctionGenerator : public GeneratorProcessor
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

        DspFloatType min=-1,max=1;

        FunctionGenerator(Type t) 
        {
            inc = 440.0/sampleRate;
            type = t;
        }
        FunctionGenerator(DspFloatType f, DspFloatType sr=44100) : GeneratorProcessor(), frequency(f),sampleRate(sr),startphase(0),phase(0.0) {
            inc = f/sr;
        }
        void setType(Type t) {
            type = t;
        }
        void setPolarity(Polarity p) {
            polarity = p;
        }
        void setFrequency(DspFloatType f)
        {
            frequency = f;
            inc = f/sampleRate;
        }
        void phaseReset(DspFloatType phaseIn) {
            // This allows you to set the phase of the oscillator to anything you like.
            phase = phaseIn;
        }

        inline void phaseIncrement() {        
            phase = fmod(phase+inc,1);
        } 
        void setRange(DspFloatType _min, DspFloatType _max) {
            min = _min;
            max = _max;
        }
        DspFloatType phasor(DspFloatType frequency, DspFloatType startphase=0, DspFloatType endphase=1) {
            
            output = phase;
            
            if (phase < startphase) {
                phase = startphase;
            }
            
            if (phase >= endphase)
                phase = startphase;
            
            phase += ((endphase - startphase) / (sampleRate / frequency));
            
            return (output);
        }
            
        DspFloatType noise() {
            // White Noise
            // always the same unless you seed it.
            DspFloatType r = std::rand() / (DspFloatType)RAND_MAX;
            return r;
        }

        DspFloatType sinewave() {
            return 0.5 + 0.5*sin(phase * TWOPI);            
        }

        DspFloatType coswave() {
            return 0.5 + 0.5*cos(phase * TWOPI);            
        }

        DspFloatType phasor() {
            return phase;
        }

        DspFloatType square() {
            return phase < 0.5f ? 0 : 1.0f;            
        }

        DspFloatType pulse() {
            DspFloatType output;
            if (phase < duty)
                output = 0;
            if (phase > duty)
                output = 1.f;
            return output;
        }
        DspFloatType saw() 
        { 
            return phase;
        }

        DspFloatType triangle() {     
            DspFloatType output;   
            if (phase <= 0.5f) {
                output = (phase - 0.25f) * 4;
            } else {
                output = ((1.0f - phase) - 0.25f) * 4;
            }
            return (output);
        }

        DspFloatType function() {
            switch(type)
            {
            case SINEWAVE: return sinewave();
            case COSWAVE: return coswave();
            case PHASOR: return phasor();
            case SQUARE: return square();
            case PULSE: return pulse();
            case SAW: return saw();            
            case REVERSE_SAW: return 1-saw();
            }
            return triangle();
        }
    
        DspFloatType Tick(DspFloatType I=0, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0) {        
            //DspFloatType p = phase;
            //phase = phasor(frequency + 0.5*X*frequency) + Y + phase;
            //phase = std::fmod(phase,1);
            DspFloatType r = function();
            //phase = p;        
            phaseIncrement();      
            r = (max-min)*r + min;
            if(polarity == POSITIVE) return r;
            else if(polarity == NEGATIVE) return -r;
            return 2*r-1;
        }

        DspFloatType sampleRate;
        DspFloatType frequency;
        DspFloatType phase;
        DspFloatType duty = 0.5f;
        DspFloatType startphase;
        DspFloatType endphase;
        DspFloatType output;
        DspFloatType tri;
        DspFloatType inc;
    };

    struct NoiseFunction : public FunctionGenerator
    {
        NoiseFunction() : FunctionGenerator(FunctionGenerator::Type::NOISE)
        {

        }
    };
    struct SinFunction : public FunctionGenerator
    {
        SinFunction() : FunctionGenerator(FunctionGenerator::Type::SINEWAVE)
        {

        }
    };
    struct CosFunction : public FunctionGenerator
    {
        CosFunction() : FunctionGenerator(FunctionGenerator::Type::COSWAVE)
        {

        }
    };
    struct PhasorFunction : public FunctionGenerator
    {
        PhasorFunction() : FunctionGenerator(FunctionGenerator::Type::PHASOR)
        {

        }
    };
    struct SquareFunction : public FunctionGenerator
    {
        SquareFunction() : FunctionGenerator(FunctionGenerator::Type::SQUARE)
        {

        }
    };
    struct PulseFunction : public FunctionGenerator
    {
        PulseFunction() : FunctionGenerator(FunctionGenerator::Type::PULSE)
        {

        }
    };
    struct SawFunction : public FunctionGenerator
    {
        SawFunction() : FunctionGenerator(FunctionGenerator::Type::SAW)
        {

        }
    };    
    struct ReverseSawFunction : public FunctionGenerator
    {
        ReverseSawFunction() : FunctionGenerator(FunctionGenerator::Type::REVERSE_SAW)
        {

        }
    };    
    struct TriangleFunction : public FunctionGenerator
    {
        TriangleFunction() : FunctionGenerator(FunctionGenerator::Type::TRIANGLE)
        {

        }
    };
}