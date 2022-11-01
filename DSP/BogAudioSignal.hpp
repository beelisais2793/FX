#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Signal
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	struct Amplifier {
		static const float minDecibels;
		static const float maxDecibels;
		static const float decibelsRange;
		struct LevelTable : Table {
			LevelTable(int n) : Table(n) {}
			void _generate() override;
		};
		struct StaticLevelTable : StaticTable<LevelTable, 13> {};

		float _db = 0.0f;
		float _level;
		const Table& _table;

		Amplifier() : _table(StaticLevelTable::table())	{
			setLevel(minDecibels);
		}

		void setLevel(float db);
		float next(float s);
	};



	struct RunningAverage {
		float _maxDelayMS;
		float _sampleRate = -1.0f;
		float _sensitivity = -1.0f;

		bool _initialized = false;
		float* _buffer = NULL;
		int _bufferN = 0;
		int _sumN = 0;
		float _invSumN = 0.0f;
		int _leadI = 0;
		int _trailI = 0;
		double _sum = 0;

		RunningAverage(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f) : _maxDelayMS(maxDelayMS) {
			setSampleRate(sampleRate);
			setSensitivity(sensitivity);
		}
		virtual ~RunningAverage() {
			if (_buffer) {
				delete[] _buffer;
			}
		}

		void setSampleRate(float sampleRate);
		void setSensitivity(float sensitivity);
		void reset();
		virtual float next(float sample);
	};


        
	const float Amplifier::minDecibels = -60.0f;
	const float Amplifier::maxDecibels = 20.0f;
	const float Amplifier::decibelsRange = maxDecibels - minDecibels;

	void Amplifier::LevelTable::_generate() {
		const float rdb = 6.0f;
		const float tdb = Amplifier::minDecibels + rdb;
		const float ta = decibelsToAmplitude(tdb);
		_table[0] = 0.0f;
		for (int i = 1; i < _length; ++i) {
			float db = Amplifier::minDecibels + (i / (float)_length) * Amplifier::decibelsRange;
			if (db <= tdb) {
				_table[i] = ((db - minDecibels) / rdb) * ta;
			}
			else {
				_table[i] = decibelsToAmplitude(db);
			}
		}
	}

	void Amplifier::setLevel(float db) {
		if (_db != db) {
			_db = db;
			if (_db > minDecibels) {
				if (_db < maxDecibels) {
					_level = _table.value(((_db - minDecibels) / decibelsRange) * _table.length());
				}
				else {
					_level = decibelsToAmplitude(_db);
				}
			}
			else {
				_level = 0.0f;
			}
		}
	}

	float Amplifier::next(float s) {
		return _level * s;
	}


	void RunningAverage::setSampleRate(float sampleRate) {
		assert(sampleRate > 0.0f);
		if (_sampleRate != sampleRate) {
			_sampleRate = sampleRate;
			if (_buffer) {
				delete[] _buffer;
			}
			_bufferN = (_maxDelayMS / 1000.0f) * _sampleRate;
			_buffer = new float[_bufferN] {};
			if (_initialized) {
				_initialized = false;
				setSensitivity(_sensitivity);
			}
		}
	}

	void RunningAverage::setSensitivity(float sensitivity) {
		assert(sensitivity >= 0.0f);
		assert(sensitivity <= 1.0f);
		if (_initialized) {
			if (_sensitivity != sensitivity) {
				_sensitivity = sensitivity;
				int newSumN = std::max(_sensitivity * _bufferN, 1.0f);
				int i = newSumN;
				while (i > _sumN) {
					--_trailI;
					if (_trailI < 0) {
						_trailI = _bufferN - 1;
					}
					_sum += _buffer[_trailI];
					--i;
				}
				while (i < _sumN) {
					_sum -= _buffer[_trailI];
					++_trailI;
					_trailI %= _bufferN;
					++i;
				}
				_sumN = newSumN;
			}
		}
		else {
			_initialized = true;
			_sensitivity = sensitivity;
			_sumN = std::max(_sensitivity * _bufferN, 1.0f);
			_leadI = 0;
			_trailI = _bufferN - _sumN;
			_sum = 0.0;
		}
		_invSumN = 1.0f / (float)_sumN;
	}

	void RunningAverage::reset() {
		_sum = 0.0;
		std::fill(_buffer, _buffer + _bufferN, 0.0);
	}

	float RunningAverage::next(float sample) {
		_sum -= _buffer[_trailI];
		++_trailI;
		_trailI %= _bufferN;
		_sum += _buffer[_leadI] = sample;
		++_leadI;
		_leadI %= _bufferN;
		return (float)_sum * _invSumN;
	}
}