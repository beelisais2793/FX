#pragma once
#include <cmath>
#include "FX/ClipFunctions.hpp"

struct FunctionLFO : public GeneratorProcessor
{
    enum Polarity 
    {
        BIPOLAR,
        POSITIVE,
        NEGATIVE,
    }
    polarity = BIPOLAR;

    enum Waveform
    {
        kWaveformSine = 0,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSquareSlopedEdges,
        kWaveformSaw,
        kWaveformReverseSaw,
        kWaveformRandomPhase,
        kWaveformRandom,
        kWaveformTangent,
        kNumWaveforms
    };

    Waveform lfoWaveform;
    float    lfoPhase;  
    float    lfoPhaseInc;           
    float    lfoFreqHz;
    double   sampleRate;
    float    duty;
    float    scale;

    FunctionLFO(float freq, Waveform w = kWaveformSine, float sr=44100.0f) : GeneratorProcessor() {
        sampleRate = sr;
        lfoFreqHz  = freq;
        lfoPhaseInc = freq/sr;
        lfoPhase = 0.0f;
        duty = 0.5f;
        lfoWaveform = w;
        scale = 1.0f;
    }
    float Tick(float I = 1, float A = 1, float X = 0, float Y = 0, float D = 0)
    {
        float phase = lfoPhase;        
        float d = duty;
        float r = 0;

        duty = clamp(duty+D,0,1);        
        phase = std::fmod(I*(lfoFreqHz + X*lfoFreqHz*0.5)/sampleRate + phase + Y,1);        
        switch (lfoWaveform)
        {
        case kWaveformRandomPhase:
            r= ((float)rand()/(float)RAND_MAX)*phase;            
            break;
        case kWaveformRandom:
            r= ((float)rand()/(float)RAND_MAX);
            break;
        case kWaveformTangent:
            r= std::tan(M_PI/2 * (phase * 0.995));
            break;
        case kWaveformSaw:
            r= phase;
            break;
        case kWaveformReverseSaw:
            r= 1.0-phase;            
            break;
        case kWaveformTriangle:
            if (phase < 0.25f)
                r= 0.5f + 2.0f*phase;
            else if (phase < 0.75f)
                r= 1.0f - 2.0f*(phase - 0.25f);
            else
                r= 2.0f*(phase - 0.75f);
            break;
        case kWaveformSquare:
            if (phase <duty)
                r= 1.0f;
            else
                r= 0.0f;
            break;
        case kWaveformSquareSlopedEdges:
            if (phase < 0.48f)
                r= 1.0f;
            else if (phase < 0.5f)
                r= 1.0f - 50.0f*(phase - 0.48f);
            else if (phase < 0.98f)
                r= 0.0f;
            else
                r= 50.0f*(phase - 0.98f);
            break;
        case kWaveformSine:
        default:
            r= 0.5f + 0.5f*sinf(2*M_PI*phase);
            break;
        }
        
        duty     = d;
        lfoPhase = fmodf(lfoPhase + lfoPhaseInc,1);
        float x  = scale*A*r;
        if(polarity == POSITIVE) return std::abs(x);
        if(polarity == NEGATIVE) return -std::abs(x);
        return 2*x-1;
    }
};
