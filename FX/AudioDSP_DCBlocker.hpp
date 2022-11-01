
/*
  ==============================================================================
    DCBlocker.h
    Created: 17 Mar 2019 10:47:42pm
    Author:  Joonas
  ==============================================================================
*/

#pragma once


#include <vector>
#include "SoundObject.hpp"

namespace JoonasFX
{
	struct DCBlocker : public StereoFXProcessor
	{
	
		enum
		{
			PORT_COEFFICIENT,
		};

		DCBlocker();
		~DCBlocker();
		void prepare(const int& numChannels);
		
		void ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs);

		void setPort(int port, DspFloatType v) {
			if(port == PORT_COEFFICIENT) p = v;
		}
		std::vector<std::vector<DspFloatType>> mXh;
		DspFloatType p = 0.992f;		
	};


	inline DCBlocker::DCBlocker() : StereoFXProcessor()
	{
	}

	inline DCBlocker::~DCBlocker()
	{
		// Empty destructor
	}

	inline void DCBlocker::prepare(const int& numChannels)
	{
		for (auto channel = 0; channel < numChannels; ++channel)
		{
			mXh.push_back({ 0.0f, 0.0f });
		}

	}

	inline void DCBlocker::ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
	{
		DspFloatType inputValue = 0.f;

		for (auto channel = 0; channel < 2; ++channel)
		{
			DspFloatType* input  = inputs[channel];
			DspFloatType* output = outputs[channel];

			for (auto sample = 0; sample < numSamples; ++sample)
			{

				inputValue = input[sample];
				DspFloatType b0 = (1.f + p) / 2.f;
				DspFloatType b2 = -1.f * b0;
				DspFloatType a2 = p;
				output[sample] = b0 * inputValue + mXh[channel][1];
				mXh[channel][1] = mXh[channel][0];
				mXh[channel][0] = b2 * inputValue + a2 * input[sample];
			}
		}
	}
}