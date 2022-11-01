#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma
{

	namespace Analysis {
		using EnvelopeFollower = gam::EnvFollow<float,float>;
		using SilenceDetector  = gam::SilenceDetect;
		using Threshold        = gam::Threshold<float>;
		using ZeroCross        = gam::ZeroCross<float>;
		using PeriodicCounter  = gam::PCounter;
		using MaxAbs           = gam::MaxAbs<float>;
		using ZeroCrossRate    = gam::ZeroCrossRate<float>;
	}
	namespace Audio {
		using AudioCallback    = gam::AudioCallback;
		using AudioDevice      = gam::AudioDevice;
		using AudioIOData      = gam::AudioIOData;
		using AudioIO          = gam::AudioIO;
	}
	namespace Containers {
		using Array            = gam::Array<float>;
		using ArrayPow2        = gam::ArrayPow2<float>;
		using RingBuffer       = gam::Ring<float>;
		using DoubleRingBuffer = gam::DoubleRing<float>;
		using DelayN           = gam::DelayN<float>;
	}
	namespace DFT {
		using DFT              = gam::DFT;
		using STFT             = gam::STFT;
		using SlidingDFT       = gam::SlidingDFT<float>;
	}

// Delay<type,gam::ipl::Interpolation>
//using DelayRound       = gam::Delay<float,gam::ipl::Round>;

	namespace Delays {
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
	}

	namespace FX 
	{
		using AM = gam::AM<float>;
		using Biquad3 = gam::Biquad3;
		using Burst = gam::Burst;
		using Chirp = gam::Chirp<float>;

		template<unsigned int N> using ChebyPolynomial = gam::ChebyN<N,float>;
		using Chorus = gam::Chorus<float>;
		using FreqShifter = gam::FreqShift<float>;
		using MonoSynth = gam::MonoSynth;
		using Pan = gam::Pan<float>;
		using Pluck = gam::Pluck;
		using Quantizer = gam::Quantizer<float>;
	}

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

	namespace FFT
	{
		using CFFT = gam::CFFT<float>;
		using RFFT = gam::RFFT<float>;
	}

	namespace Filters
	{
		/// Filter types
		enum FilterType{
			LOW_PASS,			/**< Low-pass */
			HIGH_PASS,			/**< High-pass */
			BAND_PASS,			/**< Band-pass */
			RESONANT,			/**< Resonant band-pass */
			BAND_REJECT,		/**< Band-reject */
			ALL_PASS,			/**< All-pass */
			PEAKING,			/**< Peaking */
			LOW_SHELF,			/**< Low-shelf */
			HIGH_SHELF,			/**< High-shelf */
			SMOOTHING,			/**< Smoothing */
			THRU_PASS			/**< Thru-pass (no filter) */
		};

		using AllPass1 = gam::AllPass1<float,float>;
		using Biquad   = gam::Biquad<float,float>;
		using BlockDC  = gam::BlockDC<float,float>;
		using BlockNyquist  = gam::BlockNyq<float,float>;
		using AllPass2 = gam::AllPass2<float,float>;
		using Notch = gam::Notch<float,float>;
		using Reson = gam::Reson<float,float>;
		using Hilbert = gam::Hilbert<float,float>;
		using Integrator = gam::Integrator<float,float>;
		using Differencer = gam::Differencer<float>;
		using MovingAvg = gam::MovingAvg<float>;
		using OnePole   = gam::OnePole<float,float>;

		using Vowel = gam::Vowel;
	}
///using HRFilter = gam::HRFilter;
///template<int N> using HRScene  = gam::HRScene<N>;

	namespace Noise
	{
		using BrownNoise = gam::NoiseBrown<>;
		using PinkNoise  = gam::NoisePink<>;
		using WhiteNoise = gam::NoiseWhite<>;
		using VioletNoise = gam::NoiseViolet<>;
		using BinaryNoise = gam::NoiseBinary<>;
	}

	namespace Oscillators
	{
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
	}

	namespace Samples
	{
		using Recorder = gam::Recorder;
		using Sample = gam::Sample<float>;
		using SamplePlayer = gam::SamplePlayer<float>;
		using LinearSamplePlayer = gam::SamplePlayer<float, gam::ipl::Linear>;
		using CubicSamplePlayer = gam::SamplePlayer<float,gam::ipl::Cubic>;
	}

	namespace Scheduler
	{
		using Func = gam::Func;
		using SchedularAudioIOData = gam::SchedulerAudioIOData;
		using ProcessNode = gam::ProcessNode;
		template<class T> using Process = gam::Process<T>;
		using ControlFunc = gam::ControlFunc;
		using Scheduler = gam::Scheduler;
		using SoundFile = gam::SoundFile;
	}

	namespace Echo
	{
		using LoopGain = gam::LoopGain<float>;
		using Loop1P   = gam::Loop1P<float>;
		using Loop1P1Z = gam::Loop1P1Z<float>;
		using Echo     = gam::Echo<float>;
		using CubicEcho     = gam::Echo<float,gam::ipl::Cubic>;
		using EchoCSine     = gam::EchoCSine<float>;
		using CubicEchoCSine = gam::EchoCSine<float,gam::ipl::Cubic>;
	}

	namespace Reverb
	{
		using ReverbMS = gam::ReverbMS<float>;
		using ReverbMS1P1Z = gam::ReverbMS<float,gam::Loop1P1Z>;
		using Dist = gam::Dist<2,float>;
		template<int N> using DistN = gam::Dist<N,float>;
	}

	namespace Misc
	{
		using TransferFunction = gam::TransferFunc;
		using Polar = gam::Polar<float>;
		using Complex = gam::Complex<float>;
		template<int N> using Vec = gam::Vec<N,float>;
	}

	namespace Voice 
	{
		using IndexPool = gam::IndexPool;
		using Voice = gam::Voice<float>;
		using Param = gam::Param;
		template<class VoiceGen, unsigned N> using Voices = gam::Voices<VoiceGen,N>;

	}

}