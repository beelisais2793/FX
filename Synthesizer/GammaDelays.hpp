#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Delays
{
using Array            = gam::Array<float>;
using ArrayPow2        = gam::ArrayPow2<float>;
using RingBuffer       = gam::Ring<float>;
using DoubleRingBuffer = gam::DoubleRing<float>;
using DelayN           = gam::DelayN<float>;

using Delay            = gam::Delay<float>;
using CubicDelay       = gam::Delay<float, gam::ipl::Cubic>;
using MultitapDelay    = gam::Multitap<float>;

template<int N> using NDelay = gam::DelayShift<N,float>;

// one sample
using Delay1           = gam::Delay1<float>;
// two sample
using Delay2           = gam::Delay2<float>;

using CombFilter       = gam::Comb<float>;
using CubicCombFilter  = gam::Comb<float,gam::ipl::Cubic>;


using LoopGain = gam::LoopGain<float>;
using Loop1P   = gam::Loop1P<float>;
using Loop1P1Z = gam::Loop1P1Z<float>;
using Echo     = gam::Echo<float>;
using CubicEcho     = gam::Echo<float,gam::ipl::Cubic>;
using EchoCSine     = gam::EchoCSine<float>;
using CubicEchoCSine = gam::EchoCSine<float,gam::ipl::Cubic>;

using ReverbMS = gam::ReverbMS<float>;
using ReverbMS1P1Z = gam::ReverbMS<float,gam::Loop1P1Z>;
using Dist = gam::Dist<2,float>;
template<int N> using DistN = gam::Dist<N,float>;
}