/*
  ==============================================================================
	Delay.h
	Created: 14 Mar 2019 7:31:38am
	Author:  Joonas
  ==============================================================================
*/

#pragma once

#include "SoundObject.hpp"
#include "AudioDSP_Lfo.hpp"
#include "AudioDSP_DelayLine.hpp"
#include <vector>
#include <array>

namespace JoonasFX
{
	struct DelayEffect : public StereoFXProcessor
	{

		enum
		{
			DELAYTIME_PORT,
			WET_PORT,
			FEEDBACK_PORT,
			POWER_PORT,
			RANDOMIZE_PORT,
		};

		void setPort(int port, DspFloatType v)
		{
			switch(port)
			{
				case DELAYTIME_PORT: time = v; break;
				case WET_PORT: wet = v; break;
				case FEEDBACK_PORT: feedback = v; break;
				case POWER_PORT: power((bool)v); break;
				case RANDOMIZE_PORT: if(v != 0.0) randomize(); break;
			}
		}
		DelayEffect(DspFloatType sr);
		~DelayEffect();

		void ProcessBlock(size_t n, DspFloatType **input_buffer, DspFloatType **output_buffer);

		void power(bool on = true) { on_button = on; }
		bool on_button = true;

		DspFloatType getFeedback();
		DspFloatType getWetness();
		DspFloatType linearInterp(const DspFloatType &y0, const DspFloatType &yp1, const DspFloatType &frac);

		// Delay lines for right and left channel (stereo)
		std::array<DelayLine<DspFloatType>, 2> mDelayLines;

		DspFloatType mLastTime = 0.f;
		DspFloatType mSampleRate = 44100.f;
		int mDelayBufferLen = 0;
		int mNumChannels = 2;
		DspFloatType feedback, wet, FB, W, G, time;

		void randomize()
		{
			feedback = noise.rand();
			wet = noise.rand();
			time = noise.rand() * sampleRate;
		}
	};

	//==============================================================================
	inline DelayEffect::DelayEffect(DspFloatType sr)
		: StereoFXProcessor(), mSampleRate(sr)
	{
		// Empty constructor
		mNumChannels = 2;
		mDelayBufferLen = 2 * mSampleRate;

		// Prepare delay lines
		for (auto &dline : mDelayLines)
		{
			dline.resize(mDelayBufferLen);
			dline.clear();
		}
		// Prepare smoothed values
		time = 220;
		feedback = 50;
		wet = 50;
		FB = feedback / 100.f;
		W = wet / 100.f;
		G = 1 - W;
	}

	//==============================================================================
	inline DelayEffect::~DelayEffect()
	{
		// Emtyp destructor
	}

	//==============================================================================
	void DelayEffect::ProcessBlock(size_t n, DspFloatType **input_buffer, DspFloatType **output_buffer)
	{
		Undenormal dn;
		FB = feedback/100.0f;
		W = wet/100.0f;
		G = 1-wet/100.0f;
		if (on_button == false)
		{
			for (size_t i = 0; i < 2; i++)
				memcpy(output_buffer[i], input_buffer[i], n * sizeof(DspFloatType));
			return;
		}
		// delay in fraction and integer
		DspFloatType delayInSamplesFrac = mSampleRate * (time / 1000.f);
		int delayInSamplesInt = static_cast<int>(delayInSamplesFrac);
		DspFloatType frac = delayInSamplesFrac - delayInSamplesInt;

		// Add delay into buffer
		for (auto channel = 0; channel < mNumChannels; ++channel)
		{

			for (auto sample = 0; sample < n; ++sample)
			{
				DspFloatType delayedSample = 0.f;

				// Fractal delay with liner interpolation if neede
				if (frac != 0)
				{
					DspFloatType y0 = mDelayLines[channel].get(delayInSamplesInt);
					DspFloatType ym1 = mDelayLines[channel].get(delayInSamplesInt + 1);
					delayedSample = linearInterp(y0, ym1, frac);
				}
				else
				{
					delayedSample = mDelayLines[channel].get(delayInSamplesInt);
				}

				auto inputSample = input_buffer[channel][sample];
				mDelayLines[channel].push(inputSample + delayedSample * FB);
				output_buffer[channel][sample] = inputSample * G + delayedSample * W;
			}
		}
	}

	//==============================================================================
	// Linear interpolation used with fractal delays
	DspFloatType DelayEffect::linearInterp(const DspFloatType &y0, const DspFloatType &yp1, const DspFloatType &frac)
	{
		return yp1 * frac + y0 * (1 - frac);
	}

	//==============================================================================
	DspFloatType DelayEffect::getFeedback()
	{
		return feedback;
	}

	//==============================================================================
	DspFloatType DelayEffect::getWetness()
	{
		return wet;
	}

}