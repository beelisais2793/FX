#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{
	struct DelayLine : public FunctionProcessor
	{
		const float _maxTimeMS;
		float _sampleRate = -1.0f;
		int _bufferN;
		float* _buffer = NULL;
		float _time = -1.0f;
		bool _initialized = false;
		int _delaySamples;
		int _leadI;
		int _trailI;
		double last_out = 0;

		DelayLine(float sampleRate = 1000.0f, float maxTimeMS = 1000.0f, float time = 1.0f) : 			
		FunctionProcessor(),
		_maxTimeMS(maxTimeMS) {
			setSampleRate(sampleRate);
			setTime(time);
		}
		~DelayLine() {
			delete[] _buffer;
		}

		void setSampleRate(float sampleRate);
		void setTime(float time);
		float next(float sample);
		int delaySamples();

		enum {
			PORT_TIME,
		};
		void setPort(int port, double v) {
			if(port == PORT_TIME) setTime(v);
		}
		double Tick(double I, double A =1 , double X=1, double Y=1) {
			int t = _time;
			setTime(t * fabs(X));
			double r = next(fabs(Y)*I + (1.0-fabs(Y))*last_out);
			last_out = r;
			setTime(t);
			return A*r;
		}
	};

  	void DelayLine::setSampleRate(float sampleRate) {
		assert(sampleRate > 0.0f);
		if (_sampleRate != sampleRate) {
			_sampleRate = sampleRate;
			if (_buffer) {
				delete[] _buffer;
			}
			_bufferN = ceil((_maxTimeMS / 1000.0f) * _sampleRate);
			_buffer = new float[_bufferN] {};
			if (_initialized) {
				_initialized = false;
				setTime(_time);
			}
		}
	}

	void DelayLine::setTime(float time) {
		assert(time >= 0.0f);
		assert(time <= 1.0f);
		if (_initialized) {
			if (_time != time) {
				_time = time;
				int newDelaySamples = delaySamples();
				int i = newDelaySamples;
				while (i > _delaySamples) {
					--_trailI;
					if (_trailI < 0) {
						_trailI = _bufferN - 1;
					}
					--i;
				}
				while (i < _delaySamples) {
					++_trailI;
					_trailI %= _bufferN;
					++i;
				}
				_delaySamples = newDelaySamples;
			}
		}
		else {
			_initialized = true;
			_time = time;
			_delaySamples = delaySamples();
			_leadI = 0;
			_trailI = _bufferN - _delaySamples;
		}
	}

	float DelayLine::next(float sample) {
		float delayed = _buffer[_trailI];
		++_trailI;
		_trailI %= _bufferN;
		_buffer[_leadI] = sample;
		++_leadI;
		_leadI %= _bufferN;
		return delayed;
	}

	int DelayLine::delaySamples() {
		return std::max((_time * _maxTimeMS / 1000.0f) * _sampleRate, 1.0f);
	}


}