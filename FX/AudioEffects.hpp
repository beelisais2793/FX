#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <random>
#include <chrono>
#include <list>
#include <array>
#include <iostream>

#include "SoundObject.hpp"
#include "Undenormal.hpp"
#include "DSP/SimpleResampler.hpp"
#include "DSP/Decimators.hpp"

#include "ClipFunctions.hpp"
#include "DistortionFunctions.hpp"
#include "Amplifiers.hpp"
#include "Modulation.hpp"
#include "Filters.hpp"
#include "FunctionGenerator.hpp"
#include "FunctionLFO.hpp"

#include "Std/StdObject.h"
#include "Std/StdRandom.h"
#include "NoiseGenerator.h"
#include "NoiseGenerators.hpp"

//#include "pinknoise.hpp"
//#include "Noisey.hpp"
//#include "Noise.h"
//#include "ResonantLowpassFilter.h"
//#include "BasicDelayLine.h"


extern DspFloatType  sampleRate;
extern DspFloatType  inverseSampleRate;
extern Std::RandomMersenne noise;

namespace FX
{
    enum Waveform
    {
        kWaveformSine = 0,
        kWaveformTriangle,
        kWaveformSquare,
        kWaveformSquareSlopedEdges,
        kWaveformSaw,
        kWaveformReverseSaw,
        kWaveformRandomPhase,
        kWaveformRandom,
        kWaveformTangent,
        kNumWaveforms
    };

    enum Interpolation
    {
        kInterpolationNearestNeighbour = 1,
        kInterpolationLinear,
        kInterpolationCubic,
        kNumInterpolations
    };
}

#include "AudioEffectLFO.hpp"
#include "AudioEffectAmplifier.hpp"
#include "AudioEffectChorus.hpp"
#include "AudioEffectRingMod.hpp"
#include "AudioEffectTremolo.hpp"
#include "AudioEffectDistortion.hpp"
#include "AudioEffectAutoWah.hpp"
#include "AudioEffectCompressor.hpp"
#include "AudioEffectDelay.hpp"
#include "AudioEffectFlanger.hpp"
#include "AudioEffectPhaser.hpp"
#include "AudioEffectPingPong.hpp"
#include "AudioEffectSVF.hpp"
#include "AudioEffectPitchShifter.hpp"
#include "AudioEffectVibrato.hpp"

#include "freeverb/earlyref.hpp"
#include "freeverb/zrev2.hpp"
#include "freeverb/nrev.hpp"
#include "freeverb/nrevb.hpp"
#include "freeverb/strev.hpp"
#include "freeverb/progenitor2.hpp"

#include "AudioEffectEarlyReflection.hpp"
#include "AudioEffectEarlyDelay.hpp"
#include "AudioEffectHallReverb.hpp"
#include "AudioEffectPlateReverb.hpp"
#include "AudioEffectRoomReverb.hpp"
#include "AudioEffectStereoDelay.hpp"