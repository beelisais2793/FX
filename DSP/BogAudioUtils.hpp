#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{

	// Utilities
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct DCBlocker : Filter {
		float _lastIn = 0.0f;
		float _lastOut = 0.0f;

		float next(float sample) override;
	};

    
	struct AverageRectifiedValue : RunningAverage {
		AverageRectifiedValue(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f)
		: RunningAverage(sampleRate, sensitivity, maxDelayMS)
		{
		}

		float next(float sample) override;
	};

	struct FastRootMeanSquare : AverageRectifiedValue {
		DCBlocker _dcBlocker;

		FastRootMeanSquare(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f)
		: AverageRectifiedValue(sampleRate, sensitivity, maxDelayMS)
		{
		}

		float next(float sample) override;
	};



	struct PureRootMeanSquare : RunningAverage {
		PureRootMeanSquare(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f)
		: RunningAverage(sampleRate, sensitivity, maxDelayMS)
		{
		}

		float next(float sample) override;
	};

	typedef FastRootMeanSquare RootMeanSquare;

	// Puckette 2007, "Theory and Technique"
	struct PucketteEnvelopeFollower {
		DCBlocker _dcBlocker;
		LowPassFilter _filter;

		void setParams(float sampleRate, float sensitivity);
		float next(float sample);
	};

	typedef PucketteEnvelopeFollower EnvelopeFollower;

	float DCBlocker::next(float sample) {
		const float r = 0.999f;
		_lastOut = sample - _lastIn + r * _lastOut;
		_lastIn = sample;
		return _lastOut;
	}


	float AverageRectifiedValue::next(float sample) {
		return RunningAverage::next(fabsf(sample));
	}


	float FastRootMeanSquare::next(float sample) {
		return AverageRectifiedValue::next(_dcBlocker.next(sample));
	}


	float PureRootMeanSquare::next(float sample) {
		float a = RunningAverage::next(sample * sample);
		if (_sum <= 0.0) {
			return 0.0f;
		}
		return sqrtf(a);
	}


	void PucketteEnvelopeFollower::setParams(float sampleRate, float sensitivity) {
		const float maxCutoff = 10000.0f;
		const float minCutoff = 100.0f;
		assert(sensitivity >= 0.0f && sensitivity <= 1.0f);
		float cutoff = minCutoff + sensitivity * (maxCutoff - minCutoff);
		_filter.setParams(sampleRate, cutoff);
	}

	float PucketteEnvelopeFollower::next(float sample) {
		return _filter.next(fabsf(_dcBlocker.next(sample)));
	}
}