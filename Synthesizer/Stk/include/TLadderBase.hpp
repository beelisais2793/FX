#pragma once 

#include <cstdint>
#include <cmath>

#include "SoundAlchemy.hpp"

namespace SoundAlchemy::MoogFilters
{
    template<typename T>
    class TLadderFilterBase
    {
    public:
        
        TLadderFilterBase(T sampleRate) : sampleRate(sampleRate) {}
        virtual ~TLadderFilterBase() {}
        
        virtual void Process(T * samples, uint32_t n) = 0;
        virtual void SetResonance(T r) = 0;
        virtual void SetCutoff(T c) = 0;
        
        float GetResonance() { return resonance; }
        float GetCutoff() { return cutoff; }
        
    protected:
        
        T cutoff;
        T resonance;
        T sampleRate;
    };
}