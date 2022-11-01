#pragma once
// Freeverb3
// SndFilter
// Delays
// DelayLines
// Reverbs

#include "Filters/Biquad.hpp"

struct LiquidBiquad
{
    Filters::Biquad::Biquad * filters[8];    
    Filters::Biquad::Parameters params[8];    
    bool enabled[8];
    CSmoothFilter *smoothers[64];

    LiquidBiquad()
    {
        for(size_t i = 0; i < 8; i++)
        {
            filters[i] = new Filters::Biquad::Biquad;
            params[i].filterType = Filters::Biquad::LowPass;
            params[i].fs = sampleRate;
            params[i].f0 = 1000.0f;
            params[i].Q  = 0.5;
            params[i].dbGain = 1.0;
            enabled[i] = false;            
        }
        for(size_t i = 0; i < 64; i++)
            smoothers[i] = new CSmoothFilter(sampleRate,1/0.01);
    }
    ~LiquidBiquad()
    {
        for(size_t i = 0; i < 8; i++)
            delete filters[i];
    }
    void enableFilter(size_t filter) {
        enabled[filter] = true;
    }
    void disableFilter(size_t filter) {
        enabled[filter] = false;
    }
    void setFilterType(size_t filter, Filters::Biquad::FilterType type)
    {
        params[filter].filterType = type;
    }
    void setCutoff(size_t filter, float fc)
    {            
        params[filter].f0 = fc;
    }
    void setQ(size_t filter, float Q)
    {            
        params[filter].Q = Q;
    }
    void setDbGain(size_t filter, float dbGain)
    {            
        params[filter].dbGain = dbGain;
    }
    void MorphFilters(size_t filt1, size_t filt2, float frac=0.5)
    {
        filt1.morph(*filters[filt2],frac);
    }
    float Morph(float I, float frac = 0.5)
    {
        float R[8];
        float out = 0;
        for(size_t i = 0; i < 8; i++)
            R[i] = filters[i]->Tick(i);
        out = R[0];
        for(size_t i = 1; i < 8; i++)
            if(enabled[i])
                out = out + frac * (R[i] - out);
    }
    float Tick(float I)
    {
        float R = I;
        for(size_t i = 0; i < 8; i++)
        {
            //switch(op[i])
            // case cascade
            // case serial
            // case parallel
            // case morph
            if(enabled[i]) R = filters[i]->process(R);
        }
    }
}
