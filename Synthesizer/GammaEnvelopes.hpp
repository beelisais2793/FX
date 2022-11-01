#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Envelopes
{
    using Curve = gam::Curve<float,float>;
    template<int N> using Env   = gam::Env<N,float,float>;
    using AD = gam::AD<float,float>;
    using ADSR = gam::ADSR<float,float>;
    using Decay = gam::Decay<float>;
    using Gate = gam::Gate<float>;
    using Seg = gam::Seg<float>;
    using SegExp = gam::Seg<float>;
}