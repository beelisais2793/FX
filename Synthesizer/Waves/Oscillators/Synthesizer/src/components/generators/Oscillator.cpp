#include "Oscillator.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

Sample Oscillator::NextSample()
{
    double f = frequency;
    if (detune >= 0) f *= detune + 1;
    else f *= 1/(-detune + 1);
    
    double delta = (f + fm) / (double)SAMPLE_RATE;
    phase = std::fmod(1 + phase + delta, 1);
    
    double p = phase;
    if (phaseDistort) p = std::max(std::min(phaseDistort(phase), 1.0), 0.0);
    
    sample = wavetable->Value(p) * am;
    fm = 0;
    am = 1;
    return sample;
}

Sample Oscillator::GetSample()
{
    return sample;
}

void Oscillator::ResetPhase() 
{
    phase = 0;
}

Oscillator& Oscillator::FM(Sample fm)
{
    this->fm = fm;
    return *this;
}

Oscillator& Oscillator::Detune(double f)
{
    this->detune = f;
    return *this;
}

Oscillator& Oscillator::AM(Sample am)
{
    this->am = am;
    return *this;
}

Oscillator& Oscillator::Frequency(double f) 
{
    frequency = f;
    return *this;
}

Oscillator& Oscillator::WTP(double f)
{
    wavetable->Position(f);
    return *this;
}

double Oscillator::Frequency()
{
    return frequency;
}

double Oscillator::Phase() 
{
    return phase;
}

Oscillator& Oscillator::Sync(double o) 
{
    wavetable->Sync(o);
    return *this;
}

Oscillator& Oscillator::PhaseDistort(PhaseFunction pd)
{
    phaseDistort = pd;
    return *this;
}

Oscillator& Oscillator::Wavetable(Wavetables::Wavetable* wt)
{
    wavetable = wt;
    return *this;
}