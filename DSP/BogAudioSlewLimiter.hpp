#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{


	struct SlewLimiter {
		float _delta;
		float _last = 0.0f;

		SlewLimiter(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f) {
			setParams(sampleRate, milliseconds, range);
		}

		void setParams(float sampleRate = 1000.0f, float milliseconds = 1.0f, float range = 10.0f);
		inline void setLast(float last) { _last = last; }
		inline float next(float sample) {
			return _last = next(sample, _last);
		}
		float next(float sample, float last);
	};

	struct ShapedSlewLimiter {
		const float range = 10.0f;
		const float minShape = 0.1f;
		const float maxShape = 5.0f;
		float _sampleTime;
		float _time;
		float _shapeExponent;
		float _inverseShapeExponent;
		float _last = 0.0;

		ShapedSlewLimiter(float sampleRate = 1000.0f, float milliseconds = 1.0f, float shape = 1.0f) {
			setParams(sampleRate, milliseconds, shape);
		}

		void setParams(float sampleRate, float milliseconds, float shape);
		float next(float sample);
	};

    	void SlewLimiter::setParams(float sampleRate, float milliseconds, float range) {
		assert(sampleRate > 0.0f);
		assert(milliseconds >= 0.0f);
		assert(range > 0.0f);
		_delta = range / ((milliseconds / 1000.0f) * sampleRate);
	}

	float SlewLimiter::next(float sample, float last) {
		if (sample > last) {
			return std::min(last + _delta, sample);
		}
		return std::max(last - _delta, sample);
	}


	void ShapedSlewLimiter::setParams(float sampleRate, float milliseconds, float shape) {
		assert(sampleRate > 0.0f);
		assert(milliseconds >= 0.0f);
		assert(shape >= minShape);
		assert(shape <= maxShape);
		_sampleTime = 1.0f / sampleRate;
		_time = milliseconds / 1000.0f;
		_shapeExponent = (shape > -0.05f && shape < 0.05f) ? 0.0f : shape;
		_inverseShapeExponent = 1.0f / _shapeExponent;
	}

	float ShapedSlewLimiter::next(float sample) {
		float difference = sample - _last;
		float ttg = fabsf(difference) / range;
		if (_time < 0.0001f || ttg < 0.0001f) {
			return _last = sample;
		}
		if (_shapeExponent != 0.0f) {
			ttg = powf(ttg, _shapeExponent);
		}
		ttg *= _time;
		ttg = std::max(0.0f, ttg - _sampleTime);
		ttg /= _time;
		if (_shapeExponent != 0.0f) {
			ttg = powf(ttg, _inverseShapeExponent);
		}
		float y = fabsf(difference) - ttg * range;
		if (difference < 0.0f) {
			return _last = std::max(_last - y, sample);
		}
		return _last = std::min(_last + y, sample);
	}

}