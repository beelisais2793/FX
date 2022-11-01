#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Oscillators
{

using BrownNoise = gam::NoiseBrown<>;
using PinkNoise  = gam::NoisePink<>;
using WhiteNoise = gam::NoiseWhite<>;
using VioletNoise = gam::NoiseViolet<>;
using BinaryNoise = gam::NoiseBinary<>;

using Osc = gam::Osc<float>;
using CSine = gam::CSine<float>;
using Sine  = gam::Sine<float>;
using SineR  = gam::SineR<float>;
using SineRs = gam::SineRs<float>;
using SineD  = gam::SineD<float>;
using SineDs = gam::SineDs<float>;
using Chirplet = gam::Chirplet<float>;
using LFO = gam::LFO<float>;
using DWO = gam::DWO<float>;
using Buzz = gam::Buzz<float>;
using Impulse = gam::Impulse<float>;
using Saw = gam::Saw<float>;
using Square = gam::Square<float>;
using DSF = gam::DSF<float>;
using Upsample = gam::Upsample<float>;

using AM = gam::AM<float>;
using Burst = gam::Burst;
using Chirp = gam::Chirp<float>;
}