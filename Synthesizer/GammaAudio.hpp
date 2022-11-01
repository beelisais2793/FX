#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Audio
{

    using AudioCallback    = gam::AudioCallback;
    using AudioDevice      = gam::AudioDevice;
    using AudioIOData      = gam::AudioIOData;
    using AudioIO          = gam::AudioIO;

    using Recorder = gam::Recorder;
    using Sample = gam::Sample<float>;
    using SamplePlayer = gam::SamplePlayer<float>;
    using LinearSamplePlayer = gam::SamplePlayer<float, gam::ipl::Linear>;
    using CubicSamplePlayer = gam::SamplePlayer<float,gam::ipl::Cubic>;

    using Func = gam::Func;
    using SchedularAudioIOData = gam::SchedulerAudioIOData;
    using ProcessNode = gam::ProcessNode;
    template<class T> using Process = gam::Process<T>;
    using ControlFunc = gam::ControlFunc;
    using Scheduler = gam::Scheduler;
    using SoundFile = gam::SoundFile;


    using IndexPool = gam::IndexPool;
    using Voice = gam::Voice<float>;
    using Param = gam::Param;
    template<class VoiceGen, unsigned N> using Voices = gam::Voices<VoiceGen,N>;

}

