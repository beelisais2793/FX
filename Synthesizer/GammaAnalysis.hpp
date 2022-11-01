#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Analysis
{
    using EnvelopeFollower = gam::EnvFollow<float,float>;
    using SilenceDetector  = gam::SilenceDetect;
    using Threshold        = gam::Threshold<float>;
    using ZeroCross        = gam::ZeroCross<float>;
    using PeriodicCounter  = gam::PCounter;
    using MaxAbs           = gam::MaxAbs<float>;
    using ZeroCrossRate    = gam::ZeroCrossRate<float>;

    using DFT              = gam::DFT;
    using STFT             = gam::STFT;
    using SlidingDFT       = gam::SlidingDFT<float>;

    using TransferFunction = gam::TransferFunc;

    using CFFT = gam::CFFT<float>;
    using RFFT = gam::RFFT<float>;
}