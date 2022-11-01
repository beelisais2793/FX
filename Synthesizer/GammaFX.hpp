#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace FX
{
    template<unsigned int N> using ChebyPolynomial = gam::ChebyN<N,float>;
    using Chorus = gam::Chorus<float>;
    using FreqShifter = gam::FreqShift<float>;
    using MonoSynth = gam::MonoSynth;
    using Pan = gam::Pan<float>;
    using Pluck = gam::Pluck;
    using Quantizer = gam::Quantizer<float>;

    using Polar = gam::Polar<float>;
    using Complex = gam::Complex<float>;
    template<int N> using Vec = gam::Vec<N,float>;
}