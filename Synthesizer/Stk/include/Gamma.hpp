#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma
{
using EnvelopeFollower = gam::EnvFollow<float,float>;
using SilenceDetector  = gam::SilenceDetect;
using Threshold        = gam::Threshold<float>;
using ZeroCross        = gam::ZeroCross<float>;
using PeriodicCounter  = gam::PCounter;
using MaxAbs           = gam::MaxAbs<float>;
using ZeroCrossRate    = gam::ZeroCrossRate<float>;

using AudioCallback    = gam::AudioCallback;
using AudioDevice      = gam::AudioDevice;
using AudioIOData      = gam::AudioIOData;
using AudioIO          = gam::AudioIO;

using Array            = gam::Array<float>;
using ArrayPow2        = gam::ArrayPow2<float>;
using RingBuffer       = gam::Ring<float>;
using DoubleRingBuffer = gam::DoubleRing<float>;
using DelayN           = gam::DelayN<float>;

using DFT              = gam::DFT;
using STFT             = gam::STFT;
using SlidingDFT       = gam::SlidingDFT<float>;

using Delay            = gam::Delay<float>;
using DelayRound       = gam::Delay<float,gam::ipl::Round>;

}