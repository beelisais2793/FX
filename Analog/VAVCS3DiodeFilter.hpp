#pragma once

#include <cmath>
#include <cstdint>
#include "Undenormal.hpp"

struct VCS3DiodeFilter : public MonoFXProcessor
{
    
    DspFloatType eta = 1.836f;
    DspFloatType VT = 0.0260f;
    DspFloatType gamma = eta * VT;
    DspFloatType C = 1.0e-7f;
    DspFloatType Mp = 1.0e-4f;
    DspFloatType biasParameter = 0.0f;
    DspFloatType gainParameter = 1.0f;
    DspFloatType VC1, VC2, VC3, VC4;
    DspFloatType u1, u2, u3, u4, u5;
    DspFloatType s1, s2, s3, s4;
    DspFloatType Vin;
    DspFloatType Vout;
    DspFloatType VoutPrev;
    DspFloatType Fs;
    DspFloatType inputFs;

    int iteration = 0;
    int maxNrIterations = 100;

    uint8_t numChannels = 2;

    VCS3DiodeFilter(DspFloatType sr = 44100.0f) : MonoFXProcessor()
    {
        VC1 = 0.0f;
        VC2 = 0.0f;
        VC3 = 0.0f;
        VC4 = 0.0f;
        u1 = 0.0f;
        u2 = 0.0f;
        u3 = 0.0f;
        u4 = 0.0f;
        u5 = 0.0f;
        s1 = 0.0f;
        s2 = 0.0f;
        s3 = 0.0f;
        s4 = 0.0f;
        Vout = 0.0f;
        VoutPrev = 0.0f;
        Fs = sr;
        biasParameter = 440.0f;
    }

    void setCutoff(DspFloatType f) {
        biasParameter = f;
    }
    // i think it's resonance
    void setResonance(DspFloatType G) {
        gainParameter = G;
    }
    enum {
        PORT_CUTOFF,
        PORT_RESONANCE,
    };
    void setPort(int port, DspFloatType v) {
        switch(port) {
            case PORT_CUTOFF: setCutoff(v); break;
            case PORT_RESONANCE: setResonance(v); break;
        }
    }
    void ProcessBlock (size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
    {
        Undenormal nodenormals;
        
        auto* InputChannel   = inputs;
        auto* OutputChannel  = outputs;
        
        auto I0 = 8 * C * VT * 2 * Fs * tan((M_PI * biasParameter) / Fs);
        DspFloatType K = gainParameter;

        for (auto n = 0; n < numSamples; n++)
        {
            Vin = InputChannel[n];
            iteration = 0;
            while (1) {
                u1 = tanh((Vin - VoutPrev) / (2 * VT));
                VC1 = (I0 / (4.0 * C * Fs)) * (u2 + u1) + s1;
                u2 = tanh((VC2 - VC1) / (2 * gamma));
                VC2 = (I0 / (4.0 * C * Fs) * (u3 - u2)) + s2;
                u3 = tanh((VC3 - VC2) / (2 * gamma));
                VC3 = (I0 / (4.0 * C * Fs) * (u4 - u3)) + s3;
                u4 = tanh((VC4 - VC3) / (2 * gamma));
                VC4 = (I0 / (4.0 * C * Fs) * (-u5 - u4)) + s4;
                u5 = tanh(VC4 / (6.0f * gamma));
                Vout = (K + 0.5f) * VC4;
                if (abs(Vout - VoutPrev) >= Mp * abs(VoutPrev) || iteration > maxNrIterations)
                {
                    VoutPrev = Vout;
                    break;
                }
                VoutPrev = Vout;
                iteration++;
            }
            s1 = 1 / (2 * Fs) * u1 + VC1;
            s2 = 1 / (2 * Fs) * u2 + VC2;
            s3 = 1 / (2 * Fs) * u3 + VC3;
            s4 = 1 / (2 * Fs) * u4 + VC4;
            OutputChannel[n] = DspFloatType(Vout);        
        }    
    }
};