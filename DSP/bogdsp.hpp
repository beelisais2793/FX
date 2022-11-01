#pragma once
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <cassert>
#include <cstdio>
#include <vector>
#include <mutex>
#include <complex>

#include "ffft/FFTReal.h"
#include "ffft/FFTRealFixLen.h"

namespace DSP::BogAudio
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fixed Point
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename I, const int SCALE>
	struct Fixed {
		static constexpr I scale = ((I)1) << SCALE;
		static constexpr double inverseScale = 1.0 / (double)scale;
		typedef union { I v; } value_t;
		value_t _v;

		Fixed() { _v.v = 0; }
		Fixed(const value_t& o) { _v.v = o.v; }
		Fixed(const int32_t& o) { _v.v = o * scale; }
		Fixed(const int64_t& o) { _v.v = o * scale; }
		Fixed(const double& o) { _v.v = round(o * scale); }
		Fixed(const float& o) { _v.v = roundf(o * scale); }

		inline operator int32_t() const { return _v.v / scale; }
		inline operator int64_t() const { return _v.v / scale; }
		inline operator double() const { return inverseScale * (double)_v.v; }
		inline operator float() const { return inverseScale * (float)_v.v; }

		inline Fixed& operator+=(const Fixed& o) { _v.v += o._v.v; return *this; }
		inline Fixed operator+(const Fixed& o) { return Fixed(*this) += o; }
		inline Fixed& operator-=(const Fixed& o) { _v.v -= o._v.v; return *this; }
		inline Fixed operator-(const Fixed& o) { return Fixed(*this) -= o; }

		inline Fixed& operator+=(double o) { return *this += Fixed(o); }
		inline Fixed operator+(double o) { return Fixed(*this) += o; }
		inline Fixed& operator-=(double o) { return *this -= Fixed(o); }
		inline Fixed operator-(double o) { return Fixed(*this) -= o; }

		inline Fixed& operator+=(float o) { return *this += Fixed(o); }
		inline Fixed operator+(float o) { return Fixed(*this) += o; }
		inline Fixed& operator-=(float o) { return *this -= Fixed(o); }
		inline Fixed operator-(float o) { return Fixed(*this) -= o; }

		inline Fixed& operator+=(int o) { return *this += Fixed(o); }
		inline Fixed operator+(int o) { return Fixed(*this) += o; }
		inline Fixed& operator-=(int o) { return *this -= Fixed(o); }
		inline Fixed operator-(int o) { return Fixed(*this) -= o; }
	};

	typedef Fixed<int32_t, 16> fixed_16_16;
	typedef Fixed<int64_t, 32> fixed_32_32;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Base
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Generator : public GeneratorObject {
		float _current = 0.0;

		Generator() {}
		virtual ~Generator() {}

		float current() {
			return _current;
		}

		float next() {
			return _current = _next();
		}

		double Tick(double I=1, double A=1, double X=0, double Y=0) {
			return next();
		}
		virtual float _next() = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	class Table {
	protected:
		int _length = 0;
		float* _table = NULL;

	public:
		Table(int n = 10) {
			assert(n > 0);
			assert(n <= 16);
			_length = 1 << n;
		}
		virtual ~Table() {
			if (_table) {
				delete[] _table;
			}
		}

		inline int length() const { return _length; }

		inline float value(int i) const {
			assert(i >= 0 && i < _length);
			assert(_table);
			return _table[i];
		}

		void generate();

	protected:
		virtual void _generate() = 0;
	};

	template<class T, int N> class StaticTable {
	private:
		Table* _table = NULL;
		std::mutex _lock;

		StaticTable() {
		}
		~StaticTable() {
			if (_table) {
				delete _table;
			}
		}

	public:
		StaticTable(const StaticTable&) = delete;
		void operator=(const StaticTable&) = delete;

		static const Table& table() {
			static StaticTable<T, N> instance;
			std::lock_guard<std::mutex> lock(instance._lock);
			if (!instance._table) {
				instance._table = new T(N);
				instance._table->generate();
			}
			return *instance._table;
		}
	};

	struct SineTable : Table {
		SineTable(int n = 10) : Table(n) {}
		void _generate() override;
	};
	struct StaticSineTable : StaticTable<SineTable, 12> {};

	struct BlepTable : Table {
		BlepTable(int n = 10) : Table(n) {}
		void _generate() override;
	};
	struct StaticBlepTable : StaticTable<BlepTable, 12> {};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Math
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct FastTanhf : public FunctionProcessor {
		struct TanhfTable : Table {
			TanhfTable(int n) : Table(n) {}
			void _generate() override;
		};
		struct StaticTanhfTable : StaticTable<TanhfTable, 11> {};
		const Table& _table;

		FastTanhf() : FunctionProcessor(), _table(StaticTanhfTable::table())	{
		}

		float value(float radians);
		double Tick(double I, double A=1, double X=0, double Y=0) {
			return value(I);
		}
	};

	void FastTanhf::TanhfTable::_generate() {
		_table[0] = -1.0f;
		_table[_length - 1] = 1.0f;
		for (int i = 1, n = _length - 1; i < n; ++i) {
			_table[i] = tanhf((((i / (float)_length) * 2.0f) - 1.0f) * M_PI);
		}
	}

	float FastTanhf::value(float radians) {
		if (radians <= -M_PI) {
			return -1.0f;
		}
		if (radians >= M_PI) {
			return 1.0f;
		}
		return _table.value(((radians + M_PI) / (2.0f * M_PI)) * _table.length());
	}


	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pitch
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float referenceFrequency = 261.626; // C4; frequency at which Rack 1v/octave CVs are zero.
	const float referenceSemitone = 60.0; // C4; value of C4 in semitones is arbitrary here, so have it match midi note numbers when rounded to integer.
	const float twelfthRootTwo = 1.0594630943592953;
	const float logTwelfthRootTwo = logf(1.0594630943592953);

	inline float frequencyToSemitone(float frequency) {
		return logf(frequency / referenceFrequency) / logTwelfthRootTwo + referenceSemitone;
	}

	inline float semitoneToFrequency(float semitone) {
		return powf(twelfthRootTwo, semitone - referenceSemitone) * referenceFrequency;
	}

	inline float frequencyToCV(float frequency) {
		return log2f(frequency / referenceFrequency);
	}

	inline float cvToFrequency(float cv) {
		return powf(2.0, cv) * referenceFrequency;
	}

	inline float cvToSemitone(float cv) {
		return frequencyToSemitone(cvToFrequency(cv));
	}

	inline float semitoneToCV(float semitone) {
		return frequencyToCV(semitoneToFrequency(semitone));
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	struct OverlappingBuffer {
		const int _size;
		const int _overlap;
		const bool _autoProcess;
		const int _overlapN;
		const int _samplesN;
		T* _samples;
		int _sample;

		OverlappingBuffer(int size, int o, bool autoProcess = true)
		: _size(size)
		, _overlap(o)
		, _autoProcess(autoProcess)
		, _overlapN(_size / _overlap)
		, _samplesN(2*_size - _overlapN)
		, _samples(new T[_samplesN])
		, _sample(0)
		{
			assert(_size > 0);
			assert(_overlap > 0 && _overlap <= _size && _size % _overlap == 0);
		}
		virtual ~OverlappingBuffer() {
			delete[] _samples;
		}

		inline void process() { processBuffer(_samples + _sample - _size); }
		virtual void processBuffer(T* samples) = 0;
		void postProcess() {
			if (_overlap == 1) {
				_sample = 0;
			}
			else if (_sample == _samplesN) {
				std::copy(_samples + _size, _samples + _samplesN, _samples);
				_sample = _samplesN - _size;
			}
		}

		virtual bool step(T sample) {
			_samples[_sample++] = sample;
			assert(_sample <= _samplesN);

			if (_sample >= _size && _sample % _overlapN == 0) {
				if (_autoProcess) {
					process();
					postProcess();
				}
				return true;
			}
			return false;
		}
	};


	template<typename T>
	struct AveragingBuffer {
		const int _size;
		const int _framesN;
		const float _inverseFramesN;
		T* _sums;
		T* _averages;
		T* _frames;
		int _currentFrame;
		const int _resetsPerCommit;
		int _currentReset;

		AveragingBuffer(
			int size,
			int framesToAverage
		)
		: _size(size)
		, _framesN(framesToAverage)
		, _inverseFramesN(1.0 / (float)_framesN)
		, _sums(new T[_size] {})
		, _averages(new T[_size] {})
		, _frames(new T[_size * _framesN] {})
		, _currentFrame(0)
		, _resetsPerCommit(std::max(_size / 100, 10))
		, _currentReset(0)
		{
			assert(framesToAverage > 0);
		}
		~AveragingBuffer() {
			delete[] _sums;
			delete[] _averages;
			delete[] _frames;
		}

		T* getInputFrame() {
			float* frame = _frames + _currentFrame*_size;
			for (int i = 0; i < _size; ++i) {
				_sums[i] -= frame[i];
			}
			return frame;
		}

		void commitInputFrame() {
			float* frame = _frames + _currentFrame*_size;
			for (int i = 0; i < _size; ++i) {
				_sums[i] += frame[i];
				_averages[i] = _sums[i] * _inverseFramesN;
			}

			// Reset the average for some bins, such that reset overhead is even between calls -- avoids buildup of floating point error.
			for (int i = 0; i < _resetsPerCommit; ++i) {
				_sums[_currentReset] = 0.0;
				for (int j = 0; j < _framesN; ++j) {
					_sums[_currentReset] += _frames[j*_size + _currentReset];
				}
				_currentReset = (_currentReset + 1) % _size;
			}

			_currentFrame = (_currentFrame + 1) % _framesN;
		}

		const T* getAverages() {
			return _averages;
		}
	};

	template<typename T>
	struct HistoryBuffer {
		int _size, _i;
		T* _buf;

		HistoryBuffer(int size, T initialValue)
		: _size(size)
		, _i(0)
		{
			assert(size > 0);
			_buf = new T[size];
			std::fill(_buf, _buf + size, initialValue);
		}
		~HistoryBuffer() {
			delete[] _buf;
		}

		inline void push(T s) {
			++_i;
			_i %= _size;
			_buf[_i] = s;
		}

		inline T value(int i) {
			assert(i >= 0 && i < _size);
			int j = _i - i;
			if (j < 0) {
				j += _size;
			}
			return _buf[j];
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Signal
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// "amplitude" is 0-whatever here, with 1 (=0db) meaning unity gain.
	inline float decibelsToAmplitude(float db) {
		return powf(10.0f, db * 0.05f);
	}

	inline float amplitudeToDecibels(float amplitude) {
		if (amplitude < 0.000001f) {
			return -120.0f;
		}
		return 20.0f * log10f(amplitude);
	}

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

	struct PositiveZeroCrossing {
		const float positiveThreshold = 0.01f;
		const float negativeThreshold = -positiveThreshold;
		const int zeroesForReset = 20;

		enum State {
			NEGATIVE_STATE,
			POSITIVE_STATE,
			COUNT_ZEROES_STATE
		};

		State _state;
		bool _triggerable;
		int _zeroCount = 0;

		PositiveZeroCrossing(bool triggerable = true)
		: _triggerable(triggerable)
		{
			reset();
		}

		bool next(float sample);
		void reset();
	};

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

	struct Integrator {
		float _alpha = 0.0f;
		float _last = 0.0f;

		Integrator(float alpha = 1.0f) {
			setParams(alpha);
		}

		void setParams(float alpha);
		float next(float sample);
	};

	struct CrossFader {
		float _mix = 2.0f;
		float _curve = 1.0f;
		bool _linear = true;
		float _aMix;
		float _bMix;
		Amplifier _aAmp;
		Amplifier _bAmp;

		CrossFader() {
			setParams(0.0f);
		}

		void setParams(
			float mix, // -1 to 1, 0 for equal output of both inputs.
			float curve = 1.0f, // -1 to 1: at -1, A will cut fully as mix goes to 0; at 0, A cuts over full mix; at 1, A cuts from 0 to 1.  B symmetric.
			bool linear = true// cut is linear in amplitude if true; linear in decibels otherwise.
		);
		float next(float a, float b);
	};

	struct Panner {
		float _pan = 2.0f;
		float _lLevel = 0.0f;
		float _rLevel = 0.0f;
		const Table& _sineTable;

		Panner() : _sineTable(StaticSineTable::table()) {
			setPan(0.0f);
		}

		void setPan(float pan); // -1.0 full left, 0.0 even, 1.0f full right.
		void next(float sample, float& l, float& r);
	};

	struct DelayLine {
		const float _maxTimeMS;
		float _sampleRate = -1.0f;
		int _bufferN;
		float* _buffer = NULL;
		float _time = -1.0f;
		bool _initialized = false;
		int _delaySamples;
		int _leadI;
		int _trailI;

		DelayLine(float sampleRate = 1000.0f, float maxTimeMS = 1000.0f, float time = 1.0f) : _maxTimeMS(maxTimeMS) {
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
	};

	struct Limiter {
		float _shape;
		float _knee;
		float _limit;
		float _scale;
		float _normalization;
		FastTanhf _tanhf;

		Limiter() {}

		void setParams(float shape = 1.0f, float knee = 5.0f, float limit = 10.0f, float scale = 2.0f);
		float next(float sample);
	};

	struct Saturator {
		static const float limit;

		float next(float sample);
	};

	struct Compressor {
		static const float maxEffectiveRatio;
		float compressionDb(float detectorDb, float thresholdDb, float ratio, bool softKnee);
	};

	struct NoiseGate {
		static const float maxEffectiveRatio;
		float compressionDb(float detectorDb, float thresholdDb, float ratio, bool softKnee);
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


	bool PositiveZeroCrossing::next(float sample) {
		switch (_state) {
			case NEGATIVE_STATE: {
				if (sample > positiveThreshold) {
					_state = POSITIVE_STATE;
					return true;
				}
				break;
			}
			case POSITIVE_STATE: {
				if (sample < negativeThreshold) {
					_state = NEGATIVE_STATE;
				}
				else if (sample < positiveThreshold && _triggerable) {
					_state = COUNT_ZEROES_STATE;
					_zeroCount = 1;
				}
				break;
			}
			case COUNT_ZEROES_STATE: {
				if (sample >= negativeThreshold) {
					if (++_zeroCount >= zeroesForReset) {
						_state = NEGATIVE_STATE;
					}
				}
				else {
					_state = NEGATIVE_STATE;
				}
				break;
			}
		}
		return false;
	}

	void PositiveZeroCrossing::reset() {
		_state = NEGATIVE_STATE;
	}


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


	void Integrator::setParams(float alpha) {
		assert(alpha >= 0.0f);
		assert(alpha <= 1.0f);
		_alpha = alpha;
	}

	float Integrator::next(float sample) {
		// "leaky integrator"
		return _last = (1.0f - _alpha)*_last + _alpha*sample;
	}


	void CrossFader::setParams(float mix, float curve, bool linear) {
		assert(mix >= -1.0f && mix <= 1.0f);
		assert(curve >= -1.0f && curve <= 1.0f);
		if (_mix != mix || _curve != curve || _linear != linear) {
			_mix = mix;
			_curve = curve;
			_linear = linear;

			float aMax, aMin;
			float bMax, bMin;
			if (_curve < 0.0f) {
				aMax = 0.0f;
				aMin = _curve + 2.0f;
				bMax = 2.0f;
				bMin = 0.0f - _curve;
			}
			else {
				aMax = _curve;
				aMin = 2.0f;
				bMax = 2.0f - _curve;
				bMin = 0.0f;
			}

			float m = _mix + 1.0f;
			if (m < aMax) {
				_aMix = 1.0f;
			}
			else if (m > aMin) {
				_aMix = 0.0f;
			}
			else {
				_aMix = 1.0f - ((m - aMax) / (aMin - aMax));
			}

			if (m > bMax) {
				_bMix = 1.0f;
			}
			else if (m < bMin) {
				_bMix = 0.0f;
			}
			else {
				_bMix = (m - bMin) / (bMax - bMin);
			}

			if (!_linear) {
				_aAmp.setLevel((1.0f - _aMix) * Amplifier::minDecibels);
				_bAmp.setLevel((1.0f - _bMix) * Amplifier::minDecibels);
			}
		}
	}

	float CrossFader::next(float a, float b) {
		if (_linear) {
			return _aMix * a + _bMix * b;
		}
		return _aAmp.next(a) + _bAmp.next(b);
	}


	void Panner::setPan(float pan) {
		assert(pan >= -1.0f);
		assert(pan <= 1.0f);
		if (_pan != pan) {
			_pan = pan;
			_lLevel = _sineTable.value(((1.0f + _pan) / 8.0f + 0.25f) * _sineTable.length());
			_rLevel = _sineTable.value(((1.0f + _pan) / 8.0f) * _sineTable.length());
		}
	}

	void Panner::next(float sample, float& l, float& r) {
		l = _lLevel * sample;
		r = _rLevel * sample;
	}


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


	void Limiter::setParams(float shape, float knee, float limit, float scale) {
		assert(shape >= 0.0f);
		assert(knee >= 0.0f);
		assert(limit >= 0.0f);
		assert(scale >= 1.0f);
		_shape = shape;
		_knee = knee;
		_limit = std::max(knee, limit);
		_scale = scale;

		if (_shape >= 0.1f) {
			if (_shape < 1.0f) {
				_normalization = 1.0f / tanhf(_shape * M_PI);
			}
			else {
				_normalization = 1.0f;
			}
		}
	}

	float Limiter::next(float sample) {
		float out = fabsf(sample);
		if (out > _knee) {
			out -= _knee;
			out /= _scale;
			if (_shape >= 0.1f) {
				// out /= _limit - _knee;
				// out = _tanhf.value(out * _shape * M_PI) * _normalization;
				// out *= _limit - _knee;
				float x = out / (_limit - _knee);
				x = _tanhf.value(x * _shape * M_PI) * _normalization;
				x = std::min(x, 1.0f);
				x *= _limit - _knee;
				out = std::min(fabsf(sample) - _knee, x);
			}
			else {
				out = std::min(out, _limit - _knee);
			}
			out += _knee;
		}

		if (sample < 0.0f) {
			return -out;
		}
		return out;
	}


	const float Saturator::limit = 12.0f;

	// Zavalishin 2018, "The Art of VA Filter Design", http://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.0.0a.pdf
	static inline float saturation(float x) {
		const float y1 = 0.98765f; // (2*x - 1)/x**2 where x is 0.9.
		const float offset = 0.075f / Saturator::limit; // magic.
		float x1 = (x + 1.0f) * 0.5f;
		return Saturator::limit * (offset + x1 - sqrtf(x1 * x1 - y1 * x) * (1.0f / y1));
	}

	float Saturator::next(float sample) {
		float x = sample * (1.0f / limit);
		if (sample < 0.0f) {
			return -saturation(-x);
		}
		return saturation(x);
	}


	const float Compressor::maxEffectiveRatio = 1000.0f;

	float Compressor::compressionDb(float detectorDb, float thresholdDb, float ratio, bool softKnee) {
		const float softKneeDb = 3.0f;

		if (softKnee) {
			float sDb = thresholdDb - softKneeDb;
			if (detectorDb <= sDb) {
				return 0.0f;
			}

			float ix = softKneeDb * std::min(ratio, maxEffectiveRatio) + thresholdDb;
			float iy = softKneeDb + thresholdDb;
			float t = (detectorDb - sDb) / (ix - thresholdDb);
			float px = t * (ix - thresholdDb) + thresholdDb;
			float py = t * (iy - thresholdDb) + thresholdDb;
			float s = (py - sDb) / (px - sDb);
			float compressionDb = detectorDb - sDb;
			compressionDb -= s * (detectorDb - sDb);
			return compressionDb;
		}

		if (detectorDb <= thresholdDb) {
			return 0.0f;
		}
		float compressionDb = detectorDb - thresholdDb;
		compressionDb -= compressionDb / ratio;
		return compressionDb;
	}


	const float NoiseGate::maxEffectiveRatio = Compressor::maxEffectiveRatio;

	float NoiseGate::compressionDb(float detectorDb, float thresholdDb, float ratio, bool softKnee) {
		const float softKneeDb = 6.0f;

		if (softKnee) {
			// FIXME: this achieves nothing.
			float range = thresholdDb - Amplifier::minDecibels;
			float ix = thresholdDb + softKneeDb;
			float iy = 0;
			if (detectorDb >= ix) {
				return 0.0f;
			}
			float ox = thresholdDb - range / ratio;
			if (detectorDb <= ox) {
				return -Amplifier::minDecibels;
			}
			const float oy = Amplifier::minDecibels;
			float t = (detectorDb - ox) / (ix - ox);
			float px = t * (ix - thresholdDb) + thresholdDb;
			float py = t * (iy - thresholdDb) + thresholdDb;
			float s = (py - oy) / (px - ox);
			return -(oy + s * (detectorDb - ox));
		}

		if (detectorDb >= thresholdDb) {
			return 0.0f;
		}
		float differenceDb = thresholdDb - detectorDb;
		float compressionDb = differenceDb * ratio - differenceDb;
		return std::min(compressionDb, -Amplifier::minDecibels);
	}



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Filters
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Filter {
		virtual ~Filter() {}

		virtual float next(float sample) = 0;
	};

	struct ResetableFilter : Filter {
		virtual void reset() = 0;
	};

	template<typename T>
	struct BiquadFilter : ResetableFilter {
		T _a0 = 0.0;
		T _a1 = 0.0;
		T _a2 = 0.0;
		T _b1 = 0.0;
		T _b2 = 0.0 ;

		T _x[3] {};
		T _y[3] {};

		void setParams(T a0, T a1, T a2, T b0, T b1, T b2) {
			T ib0 = 1.0 / b0;
			_a0 = a0 * ib0;
			_a1 = a1 * ib0;
			_a2 = a2 * ib0;
			_b1 = b1 * ib0;
			_b2 = b2 * ib0;
		}

		void reset() override {
			_x[0] = _x[1] = _x[2] = 0.0;
			_y[0] = _y[1] = _y[2] = 0.0;
		}

		float next(float sample) override {
			_x[2] = _x[1];
			_x[1] = _x[0];
			_x[0] = sample;

			_y[2] = _y[1];
			_y[1] = _y[0];
			_y[0] = _a0 * _x[0];
			_y[0] += _a1 * _x[1];
			_y[0] += _a2 * _x[2];
			_y[0] -= _b1 * _y[1];
			_y[0] -= _b2 * _y[2];

			return _y[0];
		}
	};

	struct LowPassFilter : ResetableFilter {
		float _sampleRate = 0.0f;
		float _cutoff = 0.0f;
		float _q = 0.0f;

		BiquadFilter<double> _biquad; // double is necessary here to make low cutoffs work at high sample rates.

		LowPassFilter(float sampleRate = 1000.0f, float cutoff = 100.0f, float q = 0.001f) {
			setParams(sampleRate, cutoff, q);
		}

		void setParams(float sampleRate, float cutoff, float q = 0.001f);
		void reset() override { _biquad.reset(); }
		float next(float sample) override {
			return _biquad.next(sample);
		}
	};

	// See: http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
	void LowPassFilter::setParams(float sampleRate, float cutoff, float q) {
		if (_sampleRate == sampleRate && _cutoff == cutoff && _q == q) {
			return;
		}
		_sampleRate = sampleRate;
		_cutoff = cutoff;
		_q = q;
		// printf("\nLPF set param: sr=%f c=%f q=%f\n", _sampleRate, _cutoff, _q);

		double w0 = 2.0 * M_PI * (double)(_cutoff / _sampleRate);
		double cosw0 = cos(w0);
		double alpha = sin(w0) / (2.0 * _q);

		_biquad.setParams(
			(1.0 - cosw0) / 2.0,
			1.0 - cosw0,
			(1.0 - cosw0) / 2.0,
			1.0 + alpha,
			-2.0 * cosw0,
			1.0 - alpha
		);
	}


	template<typename T, int N>
	struct BiquadBank : ResetableFilter {
	#ifdef RACK_SIMD
		Biquad4 _biquads[N / 4];
	#else
		BiquadFilter<T> _biquads[N];
		int _n = N;
	#endif

		void setParams(int i, T a0, T a1, T a2, T b0, T b1, T b2);
		void reset() override;
		void setN(int n, bool minDelay = false);
		float next(float sample) override;
	};

	struct MultimodeTypes {
		typedef float T;
		typedef std::complex<T> TC;

		enum Type {
			UNKNOWN_TYPE,
			BUTTERWORTH_TYPE,
			CHEBYSHEV_TYPE
		};

		enum Mode {
			UNKNOWN_MODE,
			LOWPASS_MODE,
			HIGHPASS_MODE,
			BANDPASS_MODE,
			BANDREJECT_MODE
		};

		enum BandwidthMode {
			UNKNOWN_BANDWIDTH_MODE,
			LINEAR_BANDWIDTH_MODE,
			PITCH_BANDWIDTH_MODE
		};

		enum DelayMode {
			UNKNOWN_DELAY_MODE,
			FIXED_DELAY_MODE,
			MINIMUM_DELAY_MODE
		};

		static constexpr int minPoles = 1;
		static constexpr int maxPoles = 16;
		static constexpr int modPoles = 1;
		static constexpr float minFrequency = 3.0f; // FIXME: this can go down to at least 1.0f if T is double.
		static constexpr float maxFrequency = 21000.0f;
		static constexpr float minQbw = 0.0f;
		static constexpr float maxQbw = 1.0f;
		static constexpr float minBWLinear = 10.0f;
		static constexpr float maxBWLinear = 5000.0f;
		static constexpr float minBWPitch = 1.0f / (1.0f * 12.0f * 100.0f / 25.0f);
		static constexpr float maxBWPitch = 2.0f;
	};

	template<int N>
	struct MultimodeDesigner : MultimodeTypes {
		struct Pole {
			TC p;
			T x = 0.0;
			T y = 0.0;
			TC pc;
			TC p2;
			TC i2p;
			TC i2pc;
			T r = 0.0;

			Pole() {}
			Pole(T re, T im, T x, T y) : p(TC(re, im)), x(x), y(y) {
				pc = std::conj(p);
				p2 = p * p;
				i2p = (T)1.0 / ((T)2.0 * p);
				i2pc = (T)1.0 / ((T)2.0 * pc);
				r = std::abs(p);
			}
		};

		float _sampleRate = 44100.0f;
		float _half2PiST = 0.0f;
		Type _type = UNKNOWN_TYPE;
		Mode _mode = UNKNOWN_MODE;
		int _nPoles = 0;
		float _frequency = -1.0f;
		float _qbw = -1.0f;
		BandwidthMode _bandwidthMode = UNKNOWN_BANDWIDTH_MODE;
		DelayMode _delayMode = UNKNOWN_DELAY_MODE;
		Pole _poles[maxPoles / 2];
		int _nBiquads = 0;

		float effectiveMinimumFrequency();
		void setParams(
			BiquadBank<T, N>& biquads,
			float& outGain,
			float sampleRate,
			Type type,
			int poles,
			Mode mode,
			float frequency,
			float qbw,
			BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
			DelayMode dm = FIXED_DELAY_MODE
		);
	};

	struct MultimodeFilter : MultimodeTypes, ResetableFilter {
		virtual void setParams(
			float sampleRate,
			Type type,
			int poles,
			Mode mode,
			float frequency,
			float qbw,
			BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
			DelayMode dm = FIXED_DELAY_MODE
		) = 0;
	};

	template<int N>
	struct MultimodeBase : MultimodeFilter {
		MultimodeDesigner<N> _designer;
		BiquadBank<T, N> _biquads;
		float _outGain = 1.0f;

		inline float effectiveMinimumFrequency() { return _designer.effectiveMinimumFrequency(); }
		void setParams(
			float sampleRate,
			Type type,
			int poles,
			Mode mode,
			float frequency,
			float qbw,
			BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
			DelayMode dm = FIXED_DELAY_MODE
		) override;
		float next(float sample) override;
		void reset() override;
	};

	typedef MultimodeBase<16> MultimodeFilter16;
	typedef MultimodeBase<8> MultimodeFilter8;
	typedef MultimodeBase<4> MultimodeFilter4;

	struct FourPoleButtworthLowpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			float sampleRate,
			float frequency,
			float q
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				4,
				MultimodeFilter::LOWPASS_MODE,
				frequency,
				q
			);
		}
		inline float next(float sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};

	struct FourPoleButtworthHighpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			float sampleRate,
			float frequency,
			float q
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				4,
				MultimodeFilter::HIGHPASS_MODE,
				frequency,
				q
			);
		}
		inline float next(float sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};

	struct TwoPoleButtworthBandpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			float sampleRate,
			float frequency,
			float bw,
			MultimodeFilter::BandwidthMode bwm = MultimodeFilter::PITCH_BANDWIDTH_MODE
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				2,
				MultimodeFilter::BANDPASS_MODE,
				frequency,
				bw,
				bwm
			);
		}
		inline float next(float sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};

	struct FourPoleButtworthBandpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			float sampleRate,
			float frequency,
			float bw,
			MultimodeFilter::BandwidthMode bwm = MultimodeFilter::PITCH_BANDWIDTH_MODE
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				4,
				MultimodeFilter::BANDPASS_MODE,
				frequency,
				bw,
				bwm
			);
		}
		inline float next(float sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};

	template<typename T, int N> void BiquadBank<T, N>::setParams(int i, T a0, T a1, T a2, T b0, T b1, T b2) {
		assert(i >= 0 && i < N);
		_biquads[i].setParams(a0, a1, a2, b0, b1, b2);
	}

	template<typename T, int N> void BiquadBank<T, N>::reset() {
		for (int i = 0; i < N; ++i) {
			_biquads[i].reset();
		}
	}

	template<typename T, int N> void BiquadBank<T, N>::setN(int n, bool _minDelay) {
		assert(n <= N);
		_n = n;
		for (; n < N; ++n) {
			_biquads[n].reset();
		}
	}

	template<typename T, int N> float BiquadBank<T, N>::next(float sample) {
		for (int i = 0; i < _n; ++i) {
			sample = _biquads[i].next(sample);
		}
		return sample;
	}



	template struct BiquadBank<MultimodeTypes::T, 4>;
	template struct BiquadBank<MultimodeTypes::T, 8>;
	template struct BiquadBank<MultimodeTypes::T, 16>;


	///////////////////////////////////////////////////////////////////////////////
	// Multimode
	//////////////////////////////////////////////////////////////////////////////
	constexpr int MultimodeTypes::minPoles;
	constexpr int MultimodeTypes::maxPoles;
	constexpr int MultimodeTypes::modPoles;
	constexpr float MultimodeTypes::minFrequency;
	constexpr float MultimodeTypes::maxFrequency;
	constexpr float MultimodeTypes::minQbw;
	constexpr float MultimodeTypes::maxQbw;
	constexpr float MultimodeTypes::minBWLinear;
	constexpr float MultimodeTypes::maxBWLinear;
	constexpr float MultimodeTypes::minBWPitch;
	constexpr float MultimodeTypes::maxBWPitch;


	template<int N> float MultimodeDesigner<N>::effectiveMinimumFrequency() {
		return minFrequency * std::max(1.0f, roundf(_sampleRate / 44100.0f));
	}

	template<int N> void MultimodeDesigner<N>::setParams(
		BiquadBank<T, N>& biquads,
		float& outGain,
		float sampleRate,
		Type type,
		int poles,
		Mode mode,
		float frequency,
		float qbw,
		BandwidthMode bwm,
		DelayMode dm
	) {
		assert(N >= minPoles && N <= maxPoles);
		assert(poles >= minPoles && (poles <= N || (poles <= 2*N && (mode == LOWPASS_MODE || mode == HIGHPASS_MODE))));
		assert(poles % modPoles == 0);
		assert(frequency >= minFrequency - 0.00001f && frequency <= maxFrequency);
		frequency = std::max(frequency, effectiveMinimumFrequency());
		frequency = std::min(frequency, 0.49f * sampleRate);
		assert(qbw >= minQbw && qbw <= maxQbw);

		bool repole = _type != type || _mode != mode || _nPoles != poles || (type == CHEBYSHEV_TYPE && (mode == LOWPASS_MODE || mode == HIGHPASS_MODE) && _qbw != qbw);
		bool redesign = repole || _frequency != frequency || _qbw != qbw || _sampleRate != sampleRate || _bandwidthMode != bwm || _delayMode != dm;
		_sampleRate = sampleRate;
		_half2PiST = M_PI * (1.0f / sampleRate);
		_type = type;
		_nPoles = poles;
		_mode = mode;
		_frequency = frequency;
		_qbw = qbw;
		_bandwidthMode = bwm;
		_delayMode = dm;

		if (repole) {
			switch (_type) {
				case BUTTERWORTH_TYPE: {
					int np = _nPoles / 2 + (_nPoles % 2 == 1);
					for (int k = 1, j = np - 1; k <= np; ++k, --j) {
						T a = (T)(2 * k + _nPoles - 1) * M_PI / (T)(2 * _nPoles);
						T re = std::cos(a);
						T im = std::sin(a);
						_poles[j] = Pole(-re, im, re + re, re * re + im * im);
					}

					outGain = 1.0f;
					break;
				}

				case CHEBYSHEV_TYPE: {
					T ripple = 3.0;
					if (mode == LOWPASS_MODE || mode == HIGHPASS_MODE) {
						ripple += std::max(0.0f, 6.0f * qbw);
					}
					T e = ripple / (T)10.0;
					e = std::pow((T)10.0, e);
					e -= (T)1.0;
					e = std::sqrt(e);
					T ef = std::asinh((T)1.0 / e) / (float)_nPoles;
					T efr = -std::sinh(ef);
					T efi = std::cosh(ef);

					int np = _nPoles / 2 + (_nPoles % 2 == 1);
					for (int k = 1, j = np - 1; k <= np; ++k, --j) {
						T a = (T)(2 * k - 1) * M_PI / (T)(2 * _nPoles);
						T re = efr * std::sin(a);
						T im = efi * std::cos(a);
						_poles[j] = Pole(-re, im, re + re, re * re + im * im);
					}

					outGain = 1.0 / (e * std::pow(2.0, (T)(_nPoles - 1)));
					// outGain = 1.0f / std::pow(2.0f, (T)(_nPoles - 1));
					break;
				}

				default: {
					assert(false);
				}
			}
		}

		if (redesign) {
			switch (_mode) {
				case LOWPASS_MODE:
				case HIGHPASS_MODE: {
					_nBiquads = _nPoles / 2 + _nPoles % 2;
					biquads.setN(_nBiquads, _delayMode == MINIMUM_DELAY_MODE);

					// T iq = (1.0 / std::sqrt(2.0)) - 0.65 * _qbw;
					T iq = (T)0.8 - (T)0.6 * _qbw;
					T wa = std::tan(_frequency * _half2PiST);
					T wa2 = wa * wa;

					if (_mode == LOWPASS_MODE) {
						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T wap = wa * std::real(_poles[0].p);
							biquads.setParams(0, wa, wa, 0.0, wap + (T)1.0, wap - (T)1.0, (T)0.0);
						}
						T a0 = wa2;
						T a1 = wa2 + wa2;
						T a2 = wa2;
						for (int i = 0; i < nb; ++i) {
							Pole& pole = _poles[ni + i];
							T ywa2 = pole.y * wa2;
							T ywa21 = ywa2 + (T)1.0;
							T x = (((T)(i == nb / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
							T xwa = x * wa;
							T b0 = ywa21 - xwa;
							T b1 = (T)-2.0 + (ywa2 + ywa2);
							T b2 = ywa21 + xwa;
							biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
						}
					}
					else {
						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T rp = std::real(_poles[0].p);
							biquads.setParams(0, 1.0, -1.0, 0.0, wa + rp, wa - rp, 0.0);
						}
						T a0 = 1.0;
						T a1 = -2.0f;
						T a2 = 1.0;
						for (int i = 0; i < nb; ++i) {
							Pole& pole = _poles[ni + i];
							T wa2y = wa2 + pole.y;
							T x = (((T)(i == nb / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
							T xwa = x * wa;
							T b0 = wa2y - xwa;
							T b1 = (wa2 + wa2) - (pole.y + pole.y);
							T b2 = wa2y + xwa;
							biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
						}
					}
					break;
				}

				case BANDPASS_MODE:
				case BANDREJECT_MODE: {
					_nBiquads = ((_nPoles / 2) * 2) + (_nPoles % 2);
					biquads.setN(_nBiquads, _delayMode == MINIMUM_DELAY_MODE);

					T wdl = 0.0;
					T wdh = 0.0;
					switch (_bandwidthMode) {
						case LINEAR_BANDWIDTH_MODE: {
							float bandwidth = std::max(minBWLinear, maxBWLinear * _qbw);
							wdl = std::max(minFrequency, _frequency - 0.5f * bandwidth);
							wdh = std::min(maxFrequency, std::max((float)wdl + 10.0f, _frequency + 0.5f * bandwidth));
							break;
						}
						case PITCH_BANDWIDTH_MODE: {
							float bandwidth = std::max(minBWPitch, maxBWPitch * _qbw);
							wdl = std::max(minFrequency, powf(2.0f, -bandwidth) * _frequency);
							wdh = std::min(maxFrequency, std::max((float)wdl + 10.0f, powf(2.0f, bandwidth) * _frequency));
							break;
						}
						default: {
							assert(false);
						}
					}
					T wal = std::tan(wdl * _half2PiST);
					T wah = std::tan(wdh * _half2PiST);
					T w = wah - wal;
					T w2 = w * w;
					T w02 = wah * wal;

					if (_mode == BANDPASS_MODE) {
						T a0 = w;
						T a1 = 0.0;
						T a2 = -w;

						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T wp = w * std::real(_poles[0].p);
							biquads.setParams(
								0,
								a0,
								a1,
								a2,
								(T)1.0 + wp + w02,
								(T)-2.0 + (w02 + w02),
								(T)1.0 - wp + w02
							);
						}
						for (int i = 0; i < nb; i += 2) {
							Pole& pole = _poles[ni + i / 2];
							TC x = pole.p2;
							x *= w2;
							x -= (T)4.0 * w02;
							x = std::sqrt(x);
							TC xc = std::conj(x);
							TC wp = w * pole.p;
							TC wpc = w * pole.pc;
							TC y1 = (x - wp) * (T)0.5;
							TC y1c = (xc - wpc) * (T)0.5;
							TC y2 = (-x - wp) * (T)0.5;
							TC y2c = (-xc - wpc) * (T)0.5;
							TC cf1a = -(y1 + y1c);
							TC cf2a = y1 * y1c;
							TC cf1b = -(y2 + y2c);
							TC cf2b = y2 * y2c;
							T f1a = std::real(cf1a);
							T f1b = std::real(cf1b);
							T f2a = std::real(cf2a);
							T f2b = std::real(cf2b);

							{
								T b0 = (T)1.0 + f1a + f2a;
								T b1 = (T)-2.0 + (f2a + f2a);
								T b2 = (T)1.0 - f1a + f2a;
								biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
							}
							{
								T b0 = (T)1.0 + f1b + f2b;
								T b1 = (T)-2.0 + (f2b + f2b);
								T b2 = (T)1.0 - f1b + f2b;
								biquads.setParams(ni + i + 1, a0, a1, a2, b0, b1, b2);
							}
						}
					}
					else {
						T a0 = (T)1.0 + w02;
						T a1 = (T)-2.0 + (w02 + w02);
						T a2 = a0;

						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T rp = std::real(_poles[0].p);
							T rpw02 = rp * w02;
							biquads.setParams(
								0,
								a0,
								a1,
								a2,
								rp + w + rpw02,
								(T)-2.0 * rp + (rpw02 + rpw02),
								rp - w + rpw02
							);
						}
						for (int i = 0; i < nb; i += 2) {
							Pole& pole = _poles[ni + i / 2];
							TC x = pole.p2;
							x *= (T)-4.0 * w02;
							x += w2;
							x = std::sqrt(x);
							TC xc = std::conj(x);
							TC y1 = (x - w) * pole.i2p;
							TC y1c = (xc - w) * pole.i2pc;
							TC y2 = (-x - w) * pole.i2p;
							TC y2c = (-xc - w) * pole.i2pc;
							TC cf1a = -pole.r * (y1 + y1c);
							TC cf2a = pole.r * y1 * y1c;
							TC cf1b = -pole.r * (y2 + y2c);
							TC cf2b = pole.r * y2 * y2c;
							T f1a = std::real(cf1a);
							T f1b = std::real(cf1b);
							T f2a = std::real(cf2a);
							T f2b = std::real(cf2b);

							{
								T b0 = pole.r + f1a + f2a;
								T b1 = (T)-2.0 * pole.r + (f2a + f2a);
								T b2 = pole.r - f1a + f2a;
								biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
							}
							{
								T b0 = pole.r + f1b + f2b;
								T b1 = (T)-2.0 * pole.r + (f2b + f2b);
								T b2 = pole.r - f1b + f2b;
								biquads.setParams(ni + i + 1, a0, a1, a2, b0, b1, b2);
							}
						}
					}
					break;
				}

				default: {
					assert(false);
				}
			}
		}
	}

	template struct MultimodeDesigner<4>;
	template struct MultimodeDesigner<8>;
	template struct MultimodeDesigner<16>;


	template<int N> void MultimodeBase<N>::setParams(
		float sampleRate,
		Type type,
		int poles,
		Mode mode,
		float frequency,
		float qbw,
		BandwidthMode bwm,
		DelayMode dm
	) {
		_designer.setParams(
			_biquads,
			_outGain,
			sampleRate,
			type,
			poles,
			mode,
			frequency,
			qbw,
			bwm,
			dm
		);
	}

	template<int N> float MultimodeBase<N>::next(float sample) {
		return _outGain * _biquads.next(sample);
	}

	template<int N> void MultimodeBase<N>::reset() {
		_biquads.reset();
	}

	template struct MultimodeBase<4>;
	template struct MultimodeBase<8>;
	template struct MultimodeBase<16>;



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Experiments
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct ComplexBiquadFilter : BiquadFilter<float> {
		float _gain = 1.0f;
		float _zeroRadius = 1.0f;
		float _zeroTheta = M_PI;
		float _poleRadius = 0.9f;
		float _poleTheta = 0.0f;

		ComplexBiquadFilter() {
			updateParams();
		}

		void setComplexParams(
			float gain,
			float zeroRadius,
			float zeroTheta,
			float poleRadius,
			float poleTheta
		);
		void updateParams();
	};

	struct MultipoleFilter : Filter {
		enum Type {
			LP_TYPE,
			HP_TYPE
		};

		static constexpr int maxPoles = 20;
		static constexpr float maxRipple = 0.29f;
		Type _type = LP_TYPE;
		int _poles = 1;
		float _sampleRate = 0.0f;
		float _cutoff = 0.0f;
		float _ripple = 0.0f;
		BiquadFilter<double> _biquads[maxPoles / 2];

		MultipoleFilter() {}

		void setParams(
			Type type,
			int poles,
			float sampleRate,
			float cutoff,
			float ripple // FIXME: using this with more than two poles creates large gain, need compensation.
		);
		float next(float sample) override;
	};

	void ComplexBiquadFilter::setComplexParams(
		float gain,
		float zeroRadius,
		float zeroTheta,
		float poleRadius,
		float poleTheta
	) {
		if (
			_gain != gain ||
			_zeroRadius != zeroRadius ||
			_zeroTheta != zeroTheta ||
			_poleRadius != poleRadius ||
			_poleTheta != poleTheta
		) {
			assert(gain >= 0.0f && gain <= 1.0f);
			assert(zeroRadius >= 0.0f && zeroRadius <= 1.0f);
			assert(zeroTheta >= 0.0f && zeroTheta <= 2.0f*M_PI);
			assert(poleRadius >= 0.0f && poleRadius <= 1.0f);
			assert(poleTheta >= 0.0f && poleTheta <= 2.0f*M_PI);
			_gain = gain;
			_zeroRadius = zeroRadius;
			_zeroTheta = zeroTheta;
			_poleRadius = poleRadius;
			_poleTheta = poleTheta;
			updateParams();
		}
	}

	void ComplexBiquadFilter::updateParams() {
		setParams(
			_gain,
			-2.0f * _zeroRadius * cosf(_zeroTheta) * _gain,
			_zeroRadius * _zeroRadius * _gain,
			1.0f,
			-2.0f * _poleRadius * cosf(_poleTheta),
			_poleRadius * _poleRadius
		);
	}


	// Adapted from Smith 1997, "The Scientist and Engineer's Guide to DSP"
	void MultipoleFilter::setParams(
		Type type,
		int poles,
		float sampleRate,
		float cutoff,
		float ripple
	) {
		if (
			_type == type &&
			_poles == poles &&
			_sampleRate == sampleRate &&
			_cutoff == cutoff &&
			_ripple == ripple
		) {
			return;
		}
		assert(poles >= 1 && poles <= maxPoles);
		assert(poles % 2 == 0); // relax this later?
		assert(sampleRate >= 0.0f);
		assert(cutoff >= 0.0f && cutoff <= sampleRate / 2.0f);
		assert(ripple >= 0.0f && ripple <= maxRipple);
		_type = type;
		_poles = poles;
		_sampleRate = sampleRate;
		_cutoff = cutoff;
		_ripple = ripple;

		for (int p = 0, pn = _poles / 2; p < pn; ++p) {
			double a0 = 0.0;
			double a1 = 0.0;
			double a2 = 0.0;
			double b1 = 0.0;
			double b2 = 0.0;
			{
				double angle = M_PI/(_poles*2.0) + p*M_PI/_poles;
				double rp = -cos(angle);
				double ip = sin(angle);

				if (_ripple > 0.01f) {
					double es = sqrt(pow(1.0 / (1.0 - _ripple), 2.0) - 1.0);
					double esi = 1.0 / es;
					double esis = esi * esi;
					double polesi = 1.0 / (float)_poles;
					double vx = polesi * log(esi + sqrt(esis + 1.0));
					double kx = polesi * log(esi + sqrt(esis - 1.0));
					kx = (exp(kx) + exp(-kx)) / 2.0;
					rp *= ((exp(vx) - exp(-vx)) / 2.0) / kx;
					ip *= ((exp(vx) + exp(-vx)) / 2.0) / kx;
					// printf("\n\n\ncheb p=%d pn=%d rip=%f rp=%f ip=%f es=%f vx=%f kx=%f\n", p, pn, _ripple, rp, ip, es, vx, kx);
				}

				const double t = 2.0 * tan(0.5);
				const double ts = t * t;
				// printf("\n\nco=%f sr=%f fc=%f\n", _cutoff, _sampleRate, _cutoff / _sampleRate);
				double m = rp*rp + ip*ip;
				double mts = m * ts;
				double d = 4.0 - 4.0*rp*t + mts;
				double x0 = ts / d;
				double x1 = 2.0 * x0;
				double x2 = x0;
				double y1 = (8.0 - 2.0*mts) / d;
				double y2 = (-4.0 - 4.0*rp*t - mts) / d;
				// printf("vs p=%d t=%f w=%f m=%f d=%f x0=%f x1=%f x2=%f y1=%f y2=%f\n", p, t, w, m, d, x0, x1, x2, y1, y2);

				// FIXME: optimization: everything above here only depends on type, poles and ripple.

				double w = 2.0 * M_PI * (_cutoff / _sampleRate);
				double w2 = w / 2.0;
				double k = 0.0;
				switch (_type) {
					case LP_TYPE: {
						k = sin(0.5 - w2) / sin(0.5 + w2);
						break;
					}
					case HP_TYPE: {
						k = -cos(w2 + 0.5) / cos(w2 - 0.5);
						break;
					}
				}
				double ks = k * k;
				d = 1.0 + y1*k - y2*ks;
				a0 = (x0 - x1*k + x2*ks) / d;
				a1 = (-2.0*x0*k + x1 + x1*ks - 2.0*x2*k) / d;
				a2 = (x0*ks - x1*k + x2) / d;
				b1 = (2.0*k + y1 + y1*ks - 2.0*y2*k) / d;
				b2 = (-ks - y1*k + y2) / d;
				if (_type == HP_TYPE) {
					a1 = -a1;
					b1 = -b1;
				}

				// printf("pole %d: rp=%f ip=%f a0=%f a1=%f a2=%f b1=%f b2=%f\n\n\n", p, rp, ip, a0, a1, a2, b1, b2);
				_biquads[p].setParams(a0, a1, a2, 1.0, -b1, -b2);
			}
		}
	}

	float MultipoleFilter::next(float sample) {
		for (int p = 0, pn = _poles / 2; p < pn; ++p) {
			sample = _biquads[p].next(sample);
		}
		return sample;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Resample
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Decimator {
		Decimator() {}
		virtual ~Decimator() {}

		virtual void setParams(float sampleRate, int factor) = 0;
		virtual float next(const float* buf) = 0;
	};

	struct LPFDecimator : Decimator {
		int _factor;
		MultipoleFilter _filter;

		LPFDecimator(float sampleRate = 1000.0f, int factor = 8) {
			setParams(sampleRate, factor);
		}
		void setParams(float sampleRate, int factor) override;
		float next(const float* buf) override;
	};

	struct CICDecimator : Decimator {
		typedef int64_t T;
		static constexpr T scale = ((T)1) << 32;
		int _stages;
		T* _integrators;
		T* _combs;
		int _factor = 0;
		float _gainCorrection;

		CICDecimator(int stages = 4, int factor = 8);
		virtual ~CICDecimator();

		void setParams(float sampleRate, int factor) override;
		float next(const float* buf) override;
	};

	struct Interpolator {
		Interpolator() {}
		virtual ~Interpolator() {}

		virtual void setParams(float sampleRate, int factor) = 0;
		virtual void next(float sample, float* buf) = 0;
	};

	struct CICInterpolator : Interpolator {
		typedef int64_t T;
		static constexpr T scale = ((T)1) << 32;
		int _stages;
		T* _integrators;
		T* _combs;
		T* _buffer;
		int _factor = 0;
		float _gainCorrection;

		CICInterpolator(int stages = 4, int factor = 8);
		virtual ~CICInterpolator();

		void setParams(float sampleRate, int factor) override;
		void next(float sample, float* buf) override;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Equalizer
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Equalizer : Filter {
		static constexpr float gainDb = 12.0f;
		static constexpr float cutDb = -36.0f;

		Amplifier _lowAmp;
		Amplifier _midAmp;
		Amplifier _highAmp;
		FourPoleButtworthLowpassFilter _lowFilter;
		TwoPoleButtworthBandpassFilter _midFilter;
		FourPoleButtworthHighpassFilter _highFilter;

		void setParams(
			float sampleRate,
			float lowDb,
			float midDb,
			float highDb
		);
		float next(float sample) override;
	};

	void Equalizer::setParams(
		float sampleRate,
		float lowDb,
		float midDb,
		float highDb
	) {
		assert(lowDb >= cutDb && lowDb <= gainDb);
		assert(midDb >= cutDb && midDb <= gainDb);
		assert(highDb >= cutDb && highDb <= gainDb);

		_lowAmp.setLevel(lowDb);
		_lowFilter.setParams(sampleRate, 100.0f, 0.0f);

		_midAmp.setLevel(midDb);
		_midFilter.setParams(sampleRate, 350.0f, 0.55f, MultimodeFilter::PITCH_BANDWIDTH_MODE);

		_highAmp.setLevel(highDb);
		_highFilter.setParams(sampleRate, 1000.0f, 0.0f);
	}

	float Equalizer::next(float sample) {
		float low = _lowAmp.next(_lowFilter.next(sample));
		float mid = _midAmp.next(_midFilter.next(sample));
		float high = _highAmp.next(_highFilter.next(sample));
		return low + mid + high;
	}




	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Timer {
		bool _expired;
		int _countSteps;
		int _durationSteps;

		Timer(float sampleRate = 1000.0f, float time = 1.0f) {
			setParams(sampleRate, time);
			reset();
		}

		void setParams(float sampleRate = 1000.0f, float time = 1.0f);
		void reset();
		bool next();
	};

	void Timer::setParams(float sampleRate, float time) {
		assert(sampleRate > 0.0f);
		assert(time >= 0.0f);
		// FIXME: if the timer is running, should set the duration to reflect the time elapsed so far, adjusting it for the delta samplerate.
		_durationSteps = time * sampleRate;
	}

	void Timer::reset() {
		_expired = false;
		_countSteps = 0;
	}

	bool Timer::next() {
		++_countSteps;
		_expired = _expired || _countSteps >= _durationSteps;
		return !_expired;
	}



	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Analyzer - needs libffft
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Window {
		int _size;
		float* _window;
		float _sum;

		Window(int size)
		: _size(size)
		, _window(new float[_size] {})
		, _sum(0.0)
		{}
		virtual ~Window() {
			delete[] _window;
		}

		inline int size() { return _size; }
		inline float at(int i) { assert(i >= 0 && i < _size); return _window[i]; }
		inline float sum() { return _sum; }
		void apply(float* in, float* out);
	};

	struct HanningWindow : Window {
		HanningWindow(int size, float alpha = 0.5);
	};

	struct HammingWindow : HanningWindow {
		HammingWindow(int size) : HanningWindow(size, 0.54) {}
	};

	struct KaiserWindow : Window {
		KaiserWindow(int size, float alpha = 7.865f);

		float i0(float x);
	};

	struct PlanckTaperWindow : Window {
		PlanckTaperWindow(int size, int taperSamples);
	};

	struct FFT1024 {
		void* _fft = NULL;
		FFT1024();
		~FFT1024();

		void do_fft(float* out, float* in);
	};

	struct FFT4096 {
		void* _fft = NULL;
		FFT4096();
		~FFT4096();

		void do_fft(float* out, float* in);
	};

	struct FFT8192 {
		void* _fft = NULL;
		FFT8192();
		~FFT8192();

		void do_fft(float* out, float* in);
	};

	struct FFT16384 {
		void* _fft = NULL;
		FFT16384();
		~FFT16384();

		void do_fft(float* out, float* in);
	};

	struct FFT32768 {
		void* _fft = NULL;
		FFT32768();
		~FFT32768();

		void do_fft(float* out, float* in);
	};

	struct SpectrumAnalyzer : OverlappingBuffer<float> {
		enum Size {
			SIZE_128 = 128,
			SIZE_256 = 256,
			SIZE_512 = 512,
			SIZE_1024 = 1024,
			SIZE_2048 = 2048,
			SIZE_4096 = 4096,
			SIZE_8192 = 8192,
			SIZE_16384 = 16384,
			SIZE_32768 = 32768
		};
		static constexpr Size maxSize = SIZE_32768;

		enum Overlap {
			OVERLAP_1 = 1,
			OVERLAP_2 = 2,
			OVERLAP_4 = 4,
			OVERLAP_8 = 8
		};

		enum WindowType {
			WINDOW_NONE,
			WINDOW_HANNING,
			WINDOW_HAMMING,
			WINDOW_KAISER
		};

		const float _sampleRate;
		ffft::FFTReal<float>* _fft = NULL;
		FFT1024* _fft1024 = NULL;
		FFT4096* _fft4096 = NULL;
		FFT8192* _fft8192 = NULL;
		FFT16384* _fft16384 = NULL;
		FFT32768* _fft32768 = NULL;
		Window* _window = NULL;
		float* _windowOut = NULL;
		float* _fftOut = NULL;

		SpectrumAnalyzer(
			Size size,
			Overlap overlap,
			WindowType windowType,
			float sampleRate,
			bool autoProcess = true
		);
		virtual ~SpectrumAnalyzer();

		void processBuffer(float* samples) override;
		void getMagnitudes(float* bins, int nBins);
	};


	void Table::generate() {
		if (!_table) {
			_table = new float[_length] {};
			_generate();
		}
	}


	void SineTable::_generate() {
		const float twoPI = 2.0f * M_PI;
		for (int i = 0, j = _length / 4; i <= j; ++i) {
			_table[i] = sinf(twoPI * (i / (float)_length));
		}
		for (int i = 1, j = _length / 4; i < j; ++i) {
			_table[i + j] = _table[j - i];
		}
		for (int i = 0, j = _length / 2; i < j; ++i) {
			_table[i + j] = -_table[i];
		}
	}


	void BlepTable::_generate() {
		// some amount of a sinc function.
		const float scaledPi = M_PI * 10.0f;
		_table[_length / 2] = 0.0f;
		for (int i = 1, j = _length / 2; i < j; ++i) {
			float radians = scaledPi * (i / (float)j);
			_table[j + i] = sinf(radians) / radians;
		}

		// "integrate": FIXME: avoid magic normalization value.
		const float norm = _length / 40.0f;
		float sum = 0.0f;
		for (int i = _length / 2; i < _length; ++i) {
			sum += _table[i];
			_table[i] = sum / norm;
		}

		// offset.
		for (int i = _length / 2; i < _length; ++i) {
			_table[i] -= 1.0f; // assumes successful normalization to 1-ish.
		}

		// copy to first half of table.
		for (int i = 0, j = _length / 2; i < j; ++i) {
			_table[i] = -_table[_length - 1 - i];
		}

		// smooth it out even more.
		HammingWindow hw(_length);
		hw.apply(_table, _table);
	}




	////////////////////////////////////////////////////////////////////////////////////////
	// Noise
	////////////////////////////////////////////////////////////////////////////////////////
	class Seeds {
	private:
		std::mt19937 _generator;
		Seeds();
		unsigned int _next();

	public:
		Seeds(const Seeds&) = delete;
		void operator=(const Seeds&) = delete;
		static Seeds& getInstance();

		static unsigned int next();
	};

	struct NoiseGenerator : Generator {
		std::minstd_rand _generator; // one of the faster options.

		NoiseGenerator() : _generator(Seeds::next()) {}
	};

	struct WhiteNoiseGenerator : NoiseGenerator {
		std::uniform_real_distribution<float> _uniform;

		WhiteNoiseGenerator() : _uniform(-1.0, 1.0) {}

		float _next() override {
			return _uniform(_generator);
		}
	};

	template<typename G>
	struct BasePinkNoiseGenerator : NoiseGenerator {
		static const int _n = 7;
		G _g;
		G _gs[_n];
		uint32_t _count = _g.next();

		float _next() override {
			// See: http://www.firstpr.com.au/dsp/pink-noise/
			float sum = _g.next();
			for (int i = 0, bit = 1; i < _n; ++i, bit <<= 1) {
				if (_count & bit) {
					sum += _gs[i].next();
				}
				else {
					sum += _gs[i].current();
				}
			}
			++_count;
			return sum / (float)(_n + 1);
		}
	};

	struct PinkNoiseGenerator : BasePinkNoiseGenerator<WhiteNoiseGenerator> {};

	struct RedNoiseGenerator : BasePinkNoiseGenerator<PinkNoiseGenerator> {};

	struct BlueNoiseGenerator : NoiseGenerator {
		PinkNoiseGenerator _pink;
		float _last = 0.0f;

		float _next() override {
			float t = _last;
			_last = _pink.next();
			return _last - t;
		}
	};

	struct GaussianNoiseGenerator : NoiseGenerator {
		std::normal_distribution<float> _normal;

		GaussianNoiseGenerator(float mean = 0.0f, float stdDev = 1.0f) : _normal(mean, stdDev) {}

		float _next() override {
			return _normal(_generator);
		}
	};

	struct RandomWalk : Generator {
		float _min;
		float _max;
		float _last = 0.0f;
		float _lastOut = 0.0f;
		float _damp;
		float _bias = 0.0f;
		float _biasDamp = 1.0f;
		WhiteNoiseGenerator _noise;
		LowPassFilter _filter;

		RandomWalk(
			float min = -5.0f,
			float max = 5.0f,
			float sampleRate = 1000.0f,
			float change = 0.5f
		)
		: _min(min)
		, _max(max)
		{
			assert(_min < _max);
			setParams(sampleRate, change);
		}

		void setParams(float sampleRate = 1000.0f, float change = 0.5f);
		void jump();
		void tell(float v);
		float _next() override;
	};



	////////////////////////////////////////////////////////////////////////////////////////
	// Envelope
	////////////////////////////////////////////////////////////////////////////////////////
	struct EnvelopeGenerator : Generator {
		float _sampleRate = -1.0f;
		float _sampleTime;

		EnvelopeGenerator(float sampleRate = 1000.0f) {
			setSampleRate(std::max(1.0f, sampleRate));
		}

		void setSampleRate(float sampleRate);
		virtual void _sampleRateChanged() {}
	};

	struct ADSR : EnvelopeGenerator {
		enum Stage {
			STOPPED_STAGE,
			ATTACK_STAGE,
			DECAY_STAGE,
			SUSTAIN_STAGE,
			RELEASE_STAGE
		};

		Stage _stage = STOPPED_STAGE;
		bool _gated = false;
		float _attack = 0.0f;
		float _decay = 0.0f;
		float _sustain = 1.0f;
		float _release = 0.0f;
		float _attackShape;
		float _decayShape;
		float _releaseShape;
		float _stageProgress = 0.0f;
		float _releaseLevel = 0.0f;
		float _envelope = 0.0f;

		ADSR(bool linear = false, float sampleRate = 1000.0f) : EnvelopeGenerator(sampleRate) {
			setLinearShape(linear);
		}

		void reset();
		void setGate(bool high);
		void setAttack(float seconds);
		void setDecay(float seconds);
		void setSustain(float level);
		void setRelease(float seconds);
		void setLinearShape(bool linear);
		void setShapes(float attackShape, float decayShape, float releaseShape);
		bool isStage(Stage stage) { return _stage == stage; }
		void retrigger();

		float _next() override;
	};


	void EnvelopeGenerator::setSampleRate(float sampleRate) {
		assert(sampleRate >= 1.0f);
		if (_sampleRate != sampleRate) {
			_sampleRate = sampleRate;
			_sampleTime = 1.0f / sampleRate;
			_sampleRateChanged();
		}
	}


	void ADSR::reset() {
		_stage = STOPPED_STAGE;
		_gated = false;
		_envelope = 0.0f;
	}

	void ADSR::setGate(bool high) {
		_gated = high;
	}

	void ADSR::setAttack(float seconds) {
		assert(_attack >= 0.0f);
		_attack = std::max(seconds, 0.001f);
	}

	void ADSR::setDecay(float seconds) {
		assert(_decay >= 0.0f);
		_decay = std::max(seconds, 0.001f);
	}

	void ADSR::setSustain(float level) {
		assert(_sustain >= 0.0f);
		assert(_sustain <= 1.0f);
		_sustain = level;
	}

	void ADSR::setRelease(float seconds) {
		assert(_release >= 0.0f);
		_release = std::max(seconds, 0.001f);
	}

	void ADSR::setLinearShape(bool linear) {
		if (linear) {
			setShapes(1.0f, 1.0f, 1.0f);
		}
		else {
			setShapes(0.5f, 2.0f, 2.0f);
		}
	}

	void ADSR::setShapes(float attackShape, float decayShape, float releaseShape) {
		assert(attackShape >= 0.1f && attackShape <= 10.0f);
		assert(decayShape >= 0.0f && decayShape <= 10.0f);
		assert(releaseShape >= 0.0f && releaseShape <= 10.0f);
		_attackShape = attackShape;
		_decayShape = decayShape;
		_releaseShape = releaseShape;
	}

	void ADSR::retrigger() {
		switch (_stage) {
			case STOPPED_STAGE: {
				_stage = ATTACK_STAGE;
				_stageProgress = 0.0f;
				break;
			}
			default: {
				_stage = ATTACK_STAGE;
				float e = powf(_envelope, 1.0f / _attackShape);
				_stageProgress = e * _attack;
			}
		}
	}

	float ADSR::_next() {
		if (_gated) {
			switch (_stage) {
				case STOPPED_STAGE: {
					_stage = ATTACK_STAGE;
					_stageProgress = 0.0f;
					break;
				}
				case ATTACK_STAGE: {
					if (_envelope >= 1.0) {
						_stage = DECAY_STAGE;
						_stageProgress = 0.0f;
					}
					break;
				}
				case DECAY_STAGE: {
					if (_stageProgress >= _decay) {
						_stage = SUSTAIN_STAGE;
						_stageProgress = 0.0f;
					}
					break;
				}
				case SUSTAIN_STAGE: {
					break;
				}
				case RELEASE_STAGE: {
					_stage = ATTACK_STAGE;
					_stageProgress = _attack * powf(_envelope, _releaseShape);
					break;
				}
			}
		}
		else {
			switch (_stage) {
				case STOPPED_STAGE: {
					break;
				}
				case ATTACK_STAGE:
				case DECAY_STAGE:
				case SUSTAIN_STAGE: {
					_stage = RELEASE_STAGE;
					_stageProgress = 0.0f;
					_releaseLevel = _envelope;
					break;
				}
				case RELEASE_STAGE: {
					if (_stageProgress >= _release) {
						_stage = STOPPED_STAGE;
					}
					break;
				}
			}
		}

		switch (_stage) {
			case STOPPED_STAGE: {
				_envelope = 0.0f;
				break;
			}
			case ATTACK_STAGE: {
				_stageProgress += _sampleTime;
				_envelope = std::min(1.0f, _stageProgress / _attack);
				_envelope = powf(_envelope, _attackShape);
				break;
			}
			case DECAY_STAGE: {
				_stageProgress += _sampleTime;
				_envelope = std::min(1.0f, _stageProgress / _decay);
				_envelope = powf(1.0f - _envelope, _decayShape);
				_envelope *= 1.0f - _sustain;
				_envelope += _sustain;
				break;
			}
			case SUSTAIN_STAGE: {
				_envelope = _sustain;
				break;
			}
			case RELEASE_STAGE: {
				_stageProgress += _sampleTime;
				_envelope = std::min(1.0f, _stageProgress / _release);
				_envelope = powf(1.0f - _envelope, _releaseShape);
				_envelope *= _releaseLevel;
				break;
			}
		}

		return _envelope;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// Oscillator
	////////////////////////////////////////////////////////////////////////////////////////
	struct Oscillator {
		float _sampleRate;
		float _frequency;

		Oscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: _sampleRate(sampleRate > 1.0 ? sampleRate : 1.0)
		, _frequency(frequency)
		{
		}
		virtual ~Oscillator() {}

		void setSampleRate(float sampleRate) {
			if (_sampleRate != sampleRate && sampleRate >= 1.0) {
				_sampleRate = sampleRate;
				_sampleRateChanged();
			}
		}

		virtual void _sampleRateChanged() {}

		void setFrequency(float frequency) {
			if (_frequency != frequency) {
				_frequency = frequency;
				_frequencyChanged();
			}
		}

		virtual void _frequencyChanged() {}
	};

	struct OscillatorGenerator : Oscillator, Generator {
		OscillatorGenerator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: Oscillator(sampleRate, frequency)
		{
		}
	};

	struct Phasor : OscillatorGenerator {
		typedef uint64_t phase_t;
		typedef int64_t phase_delta_t;
		static constexpr phase_t cyclePhase = UINT32_MAX;
		static constexpr float twoPI = 2.0f * M_PI;
		static constexpr float maxSampleWidth = 0.25f;

		phase_delta_t _delta;
		phase_t _phase = 0;
		float _sampleWidth = 0.0f;
		phase_t _samplePhase = 0;

		Phasor(
			float sampleRate = 1000.0f,
			float frequency = 100.0f,
			float initialPhase = 0.0f
		)
		: OscillatorGenerator(sampleRate, frequency)
		{
			setPhase(initialPhase);
			_update();
		}

		void _sampleRateChanged() override {
			_update();
		}

		void _frequencyChanged() override {
			_update();
		}

		void setSampleWidth(float sw);
		virtual void resetPhase();
		void setPhase(float radians);
		void syncPhase(const Phasor& phasor);
		float nextFromPhasor(const Phasor& phasor, phase_delta_t offset = 0);
		virtual float nextForPhase(phase_t phase);
		virtual void _update();
		inline void advancePhase() { _phase += _delta; }
		inline void advancePhase(int n) { assert(n > 0); _phase += n * _delta; }
		float _next() override final;

		inline static phase_delta_t radiansToPhase(float radians) { return (radians / twoPI) * cyclePhase; }
		inline static float phaseToRadians(phase_t phase) { return (phase / (float)cyclePhase) * twoPI; }
	};

	struct TablePhasor : Phasor {
		const Table& _table;
		int _tableLength;

		TablePhasor(
			const Table& table,
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		, _table(table)
		, _tableLength(table.length())
		{
		}

		float nextForPhase(phase_t phase) override;
	};

	struct SineOscillator : OscillatorGenerator {
		double _k1, _k2;
		double _x;
		double _y;

		SineOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f,
			double initialPhase = 0.0
		)
		: OscillatorGenerator(sampleRate, frequency)
		{
			setPhase(initialPhase);
			update();
		}

		void _sampleRateChanged() override {
			update();
		}

		void _frequencyChanged() override {
			update();
		}

		void setPhase(double phase);
		void update();
		float _next() override;
	};

	struct SineTableOscillator : TablePhasor {
		SineTableOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: TablePhasor(StaticSineTable::table(), sampleRate, frequency)
		{
		}
	};

	struct SawOscillator : Phasor {
		SawOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		{
		}

		float nextForPhase(phase_t phase) override;
	};

	struct SaturatingSawOscillator : SawOscillator {
		float _saturation;
		float _saturationNormalization;
		FastTanhf _tanhf;

		SaturatingSawOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: SawOscillator(sampleRate, frequency)
		, _saturation(0.0f)
		, _saturationNormalization(1.0f)
		{
		}

		void setSaturation(float saturation);
		float nextForPhase(phase_t phase) override;
	};

	struct BandLimitedSawOscillator : SaturatingSawOscillator {
		int _quality;
		const Table& _table;
		phase_t _qd = 0;
		float _halfTableLen;

		BandLimitedSawOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f,
			int quality = 5,
			const Table& table = StaticBlepTable::table()
		)
		: SaturatingSawOscillator(sampleRate, frequency)
		, _quality(quality)
		, _table(table)
		, _halfTableLen(_table.length() / 2)
		{
			setQuality(quality);
		}

		void setQuality(int quality);
		void _update() override;
		float nextForPhase(phase_t phase) override;
	};

	struct SquareOscillator : Phasor {
		static constexpr float minPulseWidth = 0.03f;
		static constexpr float maxPulseWidth = 1.0f - minPulseWidth;
		static constexpr float defaultPulseWidth = 0.5f;
		float _pulseWidthInput = -1.0f;
		phase_t _lastCycle = -1;
		phase_t _pulseWidth = cyclePhase * defaultPulseWidth, _nextPulseWidth = cyclePhase * defaultPulseWidth;
		bool positive = true;

		SquareOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		{
		}

		void setPulseWidth(float pw);
		float nextForPhase(phase_t phase) override;
	};

	struct BandLimitedSquareOscillator : BandLimitedSawOscillator {
		const float minPulseWidth = 0.03f;
		const float maxPulseWidth = 1.0f - minPulseWidth;
		float _pulseWidthInput = -1.0f;
		bool _dcCorrection = false;
		phase_t _lastCycle = -1;
		phase_delta_t _pulseWidth = 0, _nextPulseWidth = 0;
		float _offset = 0.0f, _nextOffset = 0.0f;
		float _dcOffset = 0.0f, _nextDcOffset = 0.0f;

		BandLimitedSquareOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f,
			int quality = 5,
			const Table& table = StaticBlepTable::table()
		)
		: BandLimitedSawOscillator(sampleRate, frequency, quality, table)
		{
			setPulseWidth(0.5f);
		}

		void setPulseWidth(float pw, bool dcCorrection = true);
		float nextForPhase(phase_t phase) override;
	};

	struct TriangleOscillator : Phasor {
		const phase_t quarterCyclePhase = cyclePhase * 0.25f;
		const phase_t threeQuartersCyclePhase = cyclePhase * 0.75f;

		TriangleOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		{
		}

		float nextForPhase(phase_t phase) override;
	};

	struct SteppedRandomOscillator : Phasor {
		const phase_t _n;
		const phase_t _k;
		float* _t = NULL;
		phase_t _seed;

		SteppedRandomOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f,
			phase_t seed = 0
		);
		~SteppedRandomOscillator();

		void resetPhase() override;
		float nextForPhase(phase_t phase) override;
	};

	struct SineBankOscillator : Oscillator {
		struct Partial {
			float frequency;
			float frequencyRatio;
			float amplitude;
			float amplitudeTarget;
			float amplitudeStepDelta;
			int amplitudeSteps;
			SineTableOscillator sine;

			Partial()
			: frequency(0.0)
			, frequencyRatio(0.0)
			, amplitude(0.0)
			, amplitudeTarget(0.0)
			, amplitudeStepDelta(0.0)
			, amplitudeSteps(0)
			, sine(0.0, 0.0)
			{}
		};

		const float _maxPartialFrequencySRRatio = 0.48;
		float _maxPartialFrequency = 0.0;
		const int _amplitudeEnvelopeMS = 10;
		int _amplitudeEnvelopeSamples = 0;
		std::vector<Partial> _partials;

		SineBankOscillator(
			float sampleRate = 1000.0f,
			float frequency = 100.0f,
			int partialCount = 20
		)
		: Oscillator(sampleRate, frequency)
		, _partials(partialCount)
		{
			_sampleRateChanged();
			_frequencyChanged();
		}

		int partialCount() {
			return _partials.size();
		}

		// one-based indexes.
		void setPartial(int i, float frequencyRatio, float amplitude);
		bool setPartialFrequencyRatio(int i, float frequencyRatio);
		void setPartialAmplitude(int i, float amplitude, bool envelope = false);

		void syncToPhase(float phase);
		void syncTo(const SineBankOscillator& other);

		void _sampleRateChanged() override;
		void _frequencyChanged() override;
		float next(Phasor::phase_t phaseOffset = 0.0f);
	};

	struct ChirpOscillator : OscillatorGenerator {
		static constexpr float minFrequency = 1.0f;
		static constexpr float minTimeSeconds = 0.01f;

		SineTableOscillator _oscillator;
		float _f1 = -1.0f;
		float _f2 = -1.0f;
		float _Time = -1.0f;
		bool _linear = false;

		float _sampleTime = 0.0f;
		float _time = 0.0f;
		bool _complete = false;
		double _k = 0.0;

		ChirpOscillator(
			float sampleRate = 1000.0f,
			float frequency1 = 100.0f,
			float frequency2 = 300.0f,
			float time = 1.0f,
			bool linear = true
		)
		: _oscillator(sampleRate)
		{
			setParams(frequency1, frequency2, time, linear);
		}

		inline bool isCycleComplete() { return _complete; }
		inline bool isCycleNearlyComplete(float seconds) { return _time > _Time - seconds; }
		void setParams(float frequency1, float frequency2, float time, bool linear);
		void _sampleRateChanged() override;
		float _next() override;
		void reset();
	};

	struct PureChirpOscillator : OscillatorGenerator {
		static constexpr float minFrequency = 1.0f;
		static constexpr float minTimeSeconds = 0.01f;

		float _f1 = -1.0f;
		float _f2 = -1.0f;
		double _Time = -1.0;
		bool _linear = false;

		double _sampleTime = 0.0f;
		double _time = 0.0f;
		bool _complete = false;
		double _c = 0.0;
		double _k = 0.0;
		double _invlogk = 0.0;

		PureChirpOscillator(
			float sampleRate = 1000.0f,
			float frequency1 = 100.0f,
			float frequency2 = 300.0f,
			float time = 1.0f,
			bool linear = true
		)
		{
			setParams(frequency1, frequency2, time, linear);
		}

		inline bool isCycleComplete() { return _complete; }
		inline bool isCycleNearlyComplete(float seconds) { return _time > _Time - seconds; }
		void setParams(float frequency1, float frequency2, double time, bool linear);
		void _sampleRateChanged() override;
		void update();
		float _next() override;
		void reset();
	};

	void Phasor::setSampleWidth(float sw) {
		if (sw < 0.0f) {
			sw = 0.0f;
		}
		else if (sw > maxSampleWidth) {
			sw = maxSampleWidth;
		}
		if (_sampleWidth != sw) {
			_sampleWidth = sw;
			if (_sampleWidth > 0.001f) {
				_samplePhase = _sampleWidth * (float)cyclePhase;
			}
			else {
				_samplePhase = 0;
			}
		}
	}

	void Phasor::resetPhase() {
		_phase = 0;
	}

	void Phasor::setPhase(float radians) {
		_phase = radiansToPhase(radians);
	}

	void Phasor::syncPhase(const Phasor& phasor) {
		_phase = phasor._phase;
	}

	float Phasor::nextFromPhasor(const Phasor& phasor, phase_delta_t offset) {
		offset += phasor._phase;
		if (_samplePhase > 0) {
			offset -= offset % _samplePhase;
		}
		return nextForPhase(offset);
	}

	void Phasor::_update() {
		_delta = ((phase_delta_t)((_frequency / _sampleRate) * cyclePhase)) % cyclePhase;
	}

	float Phasor::_next() {
		advancePhase();
		if (_samplePhase > 0) {
			return nextForPhase(_phase - (_phase % _samplePhase));
		}
		return nextForPhase(_phase);
	}

	float Phasor::nextForPhase(phase_t phase) {
		return phase;
	}


	float TablePhasor::nextForPhase(phase_t phase) {
		phase %= cyclePhase;
		if (_tableLength >= 1024) {
			int i = (((((uint64_t)phase) << 16) / cyclePhase) * _tableLength) >> 16;
			i %= _tableLength;
			return _table.value(i);
		}

		float fi = phase / (float)cyclePhase;
		assert(fi >= 0.0f && fi < 1.0f);
		fi *= _tableLength;
		int i = fi;
		float v1 = _table.value(i);
		float v2 = _table.value(i + 1 == _tableLength ? 0 : i + 1);
		return v1 + (fi - i)*(v2 - v1);
	}


	// A New Recursive Quadrature Oscillator, Martin Vicanek, 2015 - http://vicanek.de/articles/QuadOsc.pdf
	void SineOscillator::setPhase(double phase) {
		_x = cos(phase);
		_y = sin(phase);
	}

	void SineOscillator::update() {
		double w = (_frequency / _sampleRate) * 2.0 * M_PI;
		_k1 = tan(w / 2.0);
		_k2 = 2.0 * _k1 / (1 + _k1*_k1);
	}

	float SineOscillator::_next() {
		double t = _x - _k1*_y;
		_y = _y + _k2*t;
		_x = t - _k1*_y;
		return _y;
	}


	float SawOscillator::nextForPhase(phase_t phase) {
		return ((phase % cyclePhase) / (float)cyclePhase) * 2.0f - 1.0f;
	}


	void SaturatingSawOscillator::setSaturation(float saturation) {
		if (_saturation != saturation) {
			assert(saturation >= 0.0f);
			_saturation = saturation;
			if (_saturation >= 0.1f) {
				if (_saturation < 1.0f) {
					_saturationNormalization = 1.0f / tanhf(_saturation * M_PI);
				}
				else {
					_saturationNormalization = 1.0f;
				}
			}
		}
	}

	float SaturatingSawOscillator::nextForPhase(phase_t phase) {
		float sample = SawOscillator::nextForPhase(phase);
		if (_saturation >= 0.1f) {
			sample = _tanhf.value(sample * _saturation * M_PI) * _saturationNormalization;
		}
		return sample;
	}


	void BandLimitedSawOscillator::setQuality(int quality) {
		if (_quality != quality) {
			assert(quality >= 0);
			_quality = quality;
			_update();
		}
	}

	void BandLimitedSawOscillator::_update() {
		Phasor::_update();
		int q = std::min(_quality, (int)(0.5f * (_sampleRate / _frequency)));
		_qd = q * _delta;
	}

	float BandLimitedSawOscillator::nextForPhase(phase_t phase) {
		phase %= cyclePhase;

		float sample = SaturatingSawOscillator::nextForPhase(phase);
		if (phase > cyclePhase - _qd) {
			float i = (cyclePhase - phase) / (float)_qd;
			i = (1.0f - i) * _halfTableLen;
			sample -= _table.value((int)i);
		}
		else if (phase < _qd) {
			float i = phase / (float)_qd;
			i *= _halfTableLen - 1;
			i += _halfTableLen;
			sample -= _table.value((int)i);
		}
		return sample;
	}


	void SquareOscillator::setPulseWidth(float pw) {
		if (_pulseWidthInput == pw) {
			return;
		}
		_pulseWidthInput = pw;

		if (pw >= maxPulseWidth) {
			pw = maxPulseWidth;
		}
		else if (pw <= minPulseWidth) {
			pw = minPulseWidth;
		}
		_nextPulseWidth = cyclePhase * pw;
	}

	float SquareOscillator::nextForPhase(phase_t phase) {
		phase_t cycle = phase / cyclePhase;
		if (_lastCycle != cycle) {
			_lastCycle = cycle;
			_pulseWidth = _nextPulseWidth;
		}
		phase %= cyclePhase;

		if (positive) {
			if (phase >= _pulseWidth) {
				positive = false;
				return -1.0f;
			}
			return 1.0f;
		}
		if (phase < _pulseWidth) {
			positive = true;
			return 1.0f;
		}
		return -1.0f;
	}


	void BandLimitedSquareOscillator::setPulseWidth(float pw, bool dcCorrection) {
		if (_pulseWidthInput == pw && _dcCorrection == dcCorrection) {
			return;
		}
		_pulseWidthInput = pw;
		_dcCorrection = dcCorrection;

		if (pw >= maxPulseWidth) {
			pw = maxPulseWidth;
		}
		else if (pw <= minPulseWidth) {
			pw = minPulseWidth;
		}
		_nextPulseWidth = cyclePhase * pw;

		if (pw > 0.5) {
			_nextOffset = 2.0f * pw - 1.0f;
		}
		else {
			_nextOffset = -(1.0f - 2.0f * pw);
		}

		_nextDcOffset = _dcCorrection ? 1.0f - 2.0f * pw : 0.0f;
	}

	float BandLimitedSquareOscillator::nextForPhase(phase_t phase) {
		phase_t cycle = phase / cyclePhase;
		if (_lastCycle != cycle) {
			_lastCycle = cycle;
			_pulseWidth = _nextPulseWidth;
			_offset = _nextOffset;
			_dcOffset = _nextDcOffset;
		}

		float sample = -BandLimitedSawOscillator::nextForPhase(phase);
		sample += BandLimitedSawOscillator::nextForPhase(phase - _pulseWidth);
		return sample + _offset + _dcOffset;
	}


	float TriangleOscillator::nextForPhase(phase_t phase) {
		phase %= cyclePhase;

		float p = (phase / (float)cyclePhase) * 4.0f;
		if (phase < quarterCyclePhase) {
			return p;
		}
		if (phase < threeQuartersCyclePhase) {
			return 2.0f - p;
		}
		return p - 4.0f;
	}


	SteppedRandomOscillator::SteppedRandomOscillator(
		float sampleRate,
		float frequency,
		phase_t seed
	)
	: Phasor(sampleRate, frequency)
	, _n(4096)
	, _k(4093) // prime less than _n
	{
		if (seed == 0) {
			_seed = Seeds::getInstance().next();
		}
		else {
			_seed = seed;
		}

		WhiteNoiseGenerator noise;
		_t = new float[_n];
		for (phase_t i = 0; i < _n; ++i) {
			_t[i] = noise.next();
		}
	}

	SteppedRandomOscillator::~SteppedRandomOscillator() {
		delete[] _t;
	}

	void SteppedRandomOscillator::resetPhase() {
		_phase -= _phase % cyclePhase;
		_phase += cyclePhase;
	}

	float SteppedRandomOscillator::nextForPhase(phase_t phase) {
		phase_t i = phase / cyclePhase;
		return _t[(_seed + i + (_seed + i) % _k) % _n];
	}


	void SineBankOscillator::setPartial(int i, float frequencyRatio, float amplitude) {
		setPartialFrequencyRatio(i, frequencyRatio);
		setPartialAmplitude(i, amplitude);
	}

	bool SineBankOscillator::setPartialFrequencyRatio(int i, float frequencyRatio) {
		if (i <= (int)_partials.size()) {
			Partial& p = _partials[i - 1];
			p.frequencyRatio = frequencyRatio;
			double f = (double)_frequency * (double)frequencyRatio;
			p.frequency = f;
			p.sine.setFrequency(f);
			return f < _maxPartialFrequency;
		}
		return false;
	}

	void SineBankOscillator::setPartialAmplitude(int i, float amplitude, bool envelope) {
		if (i <= (int)_partials.size()) {
			Partial& p = _partials[i - 1];
			if (envelope) {
				p.amplitudeTarget = amplitude;
				p.amplitudeStepDelta = (amplitude - p.amplitude) / (float)_amplitudeEnvelopeSamples;
				p.amplitudeSteps = _amplitudeEnvelopeSamples;
			}
			else if (p.amplitudeSteps > 0) {
				p.amplitudeTarget = amplitude;
				p.amplitudeStepDelta = (amplitude - p.amplitude) / (float)p.amplitudeSteps;
			}
			else {
				p.amplitude = amplitude;
			}
		}
	}

	void SineBankOscillator::syncToPhase(float phase) {
		for (Partial& p : _partials) {
			p.sine.setPhase(phase);
		}
	}

	void SineBankOscillator::syncTo(const SineBankOscillator& other) {
		for (int i = 0, n = std::min(_partials.size(), other._partials.size()); i < n; ++i) {
			_partials[i].sine.syncPhase(other._partials[i].sine);
		}
	}

	void SineBankOscillator::_sampleRateChanged() {
		_maxPartialFrequency = _maxPartialFrequencySRRatio * _sampleRate;
		_amplitudeEnvelopeSamples = _sampleRate * (_amplitudeEnvelopeMS / 1000.0f);
		for (Partial& p : _partials) {
			p.sine.setSampleRate(_sampleRate);
		}
	}

	void SineBankOscillator::_frequencyChanged() {
		for (Partial& p : _partials) {
			p.frequency = _frequency * p.frequencyRatio;
			p.sine.setFrequency(_frequency * p.frequencyRatio);
		}
	}

	float SineBankOscillator::next(Phasor::phase_t phaseOffset) {
		float next = 0.0;
		for (Partial& p : _partials) {
			p.sine.advancePhase();
			if (p.frequency < _maxPartialFrequency && (p.amplitude > 0.001 || p.amplitude < -0.001 || p.amplitudeSteps > 0)) {
				if (p.amplitudeSteps > 0) {
					if (p.amplitudeSteps == 1) {
						p.amplitude = p.amplitudeTarget;
					}
					else {
						p.amplitude += p.amplitudeStepDelta;
					}
					--p.amplitudeSteps;
				}
				next += p.sine.nextFromPhasor(p.sine, phaseOffset) * p.amplitude;
			}
		}
		return next;
	}


	constexpr float ChirpOscillator::minFrequency;
	constexpr float ChirpOscillator::minTimeSeconds;

	void ChirpOscillator::setParams(float frequency1, float frequency2, float time, bool linear) {
		frequency1 = std::max(minFrequency, std::min(frequency1, 0.99f * 0.5f * _sampleRate));
		frequency2 = std::max(minFrequency, std::min(frequency2, 0.99f * 0.5f * _sampleRate));
		assert(time >= minTimeSeconds);

		if (_f1 != frequency1 || _f2 != frequency2 || _Time != time || _linear != linear) {
			_f1 = frequency1;
			_f2 = frequency2;
			_Time = time;
			_linear = linear;

			_k = pow((double)(_f2 / _f1), 1.0f / (double)_Time);
		}
	}

	void ChirpOscillator::_sampleRateChanged() {
		_oscillator.setSampleRate(_sampleRate);
		_sampleTime = 1.0f / _sampleRate;
	}

	float ChirpOscillator::_next() {
		_complete = false;
		if (_time > _Time) {
			_time = 0.0f;
			_complete = true;
		}
		else {
			_time += _sampleTime;
		}

		if (_linear) {
			_oscillator.setFrequency(_f1 + (_time / _Time) * (_f2 - _f1));
		}
		else {
			_oscillator.setFrequency((double)_f1 * pow(_k, (double)_time));
		}
		return _oscillator.next();
	}

	void ChirpOscillator::reset() {
		_time = 0.0f;
		_oscillator.resetPhase();
	}


	constexpr float PureChirpOscillator::minFrequency;
	constexpr float PureChirpOscillator::minTimeSeconds;

	void PureChirpOscillator::setParams(float frequency1, float frequency2, double time, bool linear) {
		frequency1 = std::max(minFrequency, std::min(frequency1, 0.99f * 0.5f * _sampleRate));
		frequency2 = std::max(minFrequency, std::min(frequency2, 0.99f * 0.5f * _sampleRate));
		assert(time >= minTimeSeconds);

		if (_f1 != frequency1 || _f2 != frequency2 || _Time != time || _linear != linear) {
			_f1 = frequency1;
			_f2 = frequency2;
			_Time = time;
			_linear = linear;
			update();
		}
	}

	void PureChirpOscillator::_sampleRateChanged() {
		_sampleTime = 1.0 / (double)_sampleRate;
		update();
	}

	void PureChirpOscillator::update() {
		_Time = std::max(2.0f * _sampleTime, _Time);
		_c = (double)(_f2 - _f1) / (double)_Time;
		_k = pow((double)(_f2 / _f1), 1.0f / (double)_Time);
		_invlogk = 1.0 / log(_k);
	}

	float PureChirpOscillator::_next() {
		// formulas from https://en.wikipedia.org/wiki/Chirp
		double phase = 0.0f;
		if (_linear) {
			phase = 2.0 * M_PI * (0.5 * _c * (double)(_time * _time) + (double)(_f1 * _time));
		}
		else {
			phase = 2.0 * M_PI * (double)_f1 * ((pow(_k, (double)_time) - 1.0) * _invlogk);
		}

		_complete = false;
		if (_Time - _time < _sampleTime) {
			_time = 0.0f;
			_complete = true;
		}
		else {
			_time += _sampleTime;
		}

		return sin(phase);
	}

	void PureChirpOscillator::reset() {
		_time = 0.0f;
	}


void Window::apply(float* in, float* out) {
	for (int i = 0; i < _size; ++i) {
		out[i] = in[i] * _window[i];
	}
}


HanningWindow::HanningWindow(int size, float alpha) : Window(size) {
	const float invAlpha = 1.0 - alpha;
	const float twoPIEtc = 2.0 * M_PI / (float)size;
	for (int i = 0; i < _size; ++i) {
		_sum += _window[i] = invAlpha*cos(twoPIEtc*(float)i + M_PI) + alpha;
	}
}


KaiserWindow::KaiserWindow(int size, float alpha) : Window(size) {
	float ii0a = 1.0f / i0(alpha);
	float ism1 = 1.0f / (float)(size - 1);
	for (int i = 0; i < _size; ++i) {
		float x = i * 2.0f;
		x *= ism1;
		x -= 1.0f;
		x *= x;
		x = 1.0f - x;
		x = sqrtf(x);
		x *= alpha;
		_sum += _window[i] = i0(x) * ii0a;
	}
}

// Rabiner, Gold: "The Theory and Application of Digital Signal Processing", 1975, page 103.
float KaiserWindow::i0(float x) {
	assert(x >= 0.0f);
	assert(x < 20.0f);
	float y = 0.5f * x;
	float t = .1e-8f;
	float e = 1.0f;
	float de = 1.0f;
	for (int i = 1; i <= 25; ++i) {
		de = de * y / (float)i;
		float sde = de * de;
		e += sde;
		if (e * t - sde > 0.0f) {
			break;
		}
	}
	return e;
}


PlanckTaperWindow::PlanckTaperWindow(int size, int taperSamples) : Window(size) {
	_window[0] = 0.0f;
	_sum += _window[size - 1] = 1.0f;

	for (int i = 1; i < taperSamples; ++i) {
		float x = ((float)taperSamples / (float)i) - ((float)taperSamples / (float)(taperSamples - i));
		x = 1.0f + exp(x);
		x = 1.0f / x;
		_sum += _window[i] = x;
	}
	int nOnes = size - 2 * taperSamples;
	std::fill_n(_window + taperSamples, nOnes, 1.0f);
	_sum += nOnes;
	for (int i = 0; i < taperSamples; ++i) {
		_sum += _window[size - 1 - i] = _window[i];
	}
}


typedef ffft::FFTRealFixLen<10> FIXED_FFT1024;

FFT1024::FFT1024() {
	_fft = new FIXED_FFT1024();
}

FFT1024::~FFT1024() {
	delete (FIXED_FFT1024*)_fft;
}

void FFT1024::do_fft(float* out, float* in) {
	((FIXED_FFT1024*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<12> FIXED_FFT4096;

FFT4096::FFT4096() {
	_fft = new FIXED_FFT4096();
}

FFT4096::~FFT4096() {
	delete (FIXED_FFT4096*)_fft;
}

void FFT4096::do_fft(float* out, float* in) {
	((FIXED_FFT4096*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<13> FIXED_FFT8192;

FFT8192::FFT8192() {
	_fft = new FIXED_FFT8192();
}

FFT8192::~FFT8192() {
	delete (FIXED_FFT8192*)_fft;
}

void FFT8192::do_fft(float* out, float* in) {
	((FIXED_FFT8192*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<14> FIXED_FFT16384;

FFT16384::FFT16384() {
	_fft = new FIXED_FFT16384();
}

FFT16384::~FFT16384() {
	delete (FIXED_FFT16384*)_fft;
}

void FFT16384::do_fft(float* out, float* in) {
	((FIXED_FFT16384*)_fft)->do_fft(out, in);
}


typedef ffft::FFTRealFixLen<15> FIXED_FFT32768;

FFT32768::FFT32768() {
	_fft = new FIXED_FFT32768();
}

FFT32768::~FFT32768() {
	delete (FIXED_FFT32768*)_fft;
}

void FFT32768::do_fft(float* out, float* in) {
	((FIXED_FFT32768*)_fft)->do_fft(out, in);
}


SpectrumAnalyzer::SpectrumAnalyzer(
	Size size,
	Overlap overlap,
	WindowType windowType,
	float sampleRate,
	bool autoProcess
)
: OverlappingBuffer(size, overlap, autoProcess)
, _sampleRate(sampleRate)
{
	assert(size <= maxSize);
	assert(_sampleRate > size);

	switch (size) {
		case SIZE_1024: {
			_fft1024 = new FFT1024();
			break;
		}
		case SIZE_4096: {
			_fft4096 = new FFT4096();
			break;
		}
		case SIZE_8192: {
			_fft8192 = new FFT8192();
			break;
		}
		case SIZE_16384: {
			_fft16384 = new FFT16384();
			break;
		}
		case SIZE_32768: {
			_fft32768 = new FFT32768();
			break;
		}
		default: {
			_fft = new ffft::FFTReal<float>(size);
		}
	}

	switch (windowType) {
		case WINDOW_NONE: {
			break;
		}
		case WINDOW_HANNING: {
			_window = new HanningWindow(size);
			_windowOut = new float[size];
			break;
		}
		case WINDOW_HAMMING: {
			_window = new HammingWindow(size);
			_windowOut = new float[size];
			break;
		}
		case WINDOW_KAISER: {
			_window = new KaiserWindow(size);
			_windowOut = new float[size];
			break;
		}
	}

	_fftOut = new float[_size];
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
	if (_fft) {
		delete _fft;
	}
	if (_fft1024) {
		delete _fft1024;
	}
	if (_fft4096) {
		delete _fft4096;
	}
	if (_fft8192) {
		delete _fft8192;
	}
	if (_fft16384) {
		delete _fft16384;
	}
	if (_fft32768) {
		delete _fft32768;
	}

	if (_window) {
		delete _window;
		delete[] _windowOut;
	}

	delete[] _fftOut;
}

void SpectrumAnalyzer::processBuffer(float* samples) {
	float* input = samples;
	if (_window) {
		_window->apply(samples, _windowOut);
		input = _windowOut;
	}
	if (_fft1024) {
		_fft1024->do_fft(_fftOut, input);
	}
	else if (_fft4096) {
		_fft4096->do_fft(_fftOut, input);
	}
	else if (_fft8192) {
		_fft8192->do_fft(_fftOut, input);
	}
	else if (_fft16384) {
		_fft16384->do_fft(_fftOut, input);
	}
	else if (_fft32768) {
		_fft32768->do_fft(_fftOut, input);
	}
	else {
		_fft->do_fft(_fftOut, input);
	}
}

void SpectrumAnalyzer::getMagnitudes(float* bins, int nBins) {
	assert(nBins <= _size / 2);
	assert(_size % nBins == 0);

	const int bands = _size / 2;
	const int binWidth = bands / nBins;
	const float invBinWidth = 1.0 / (float)binWidth;
	const float normalization = 2.0 / powf(_window ? _window->sum() : _size, 2.0);
	for (int bin = 0; bin < nBins; ++bin) {
		float sum = 0.0;
		int binEnd = bin * binWidth + binWidth;
		for (int i = binEnd - binWidth; i < binEnd; ++i) {
			sum += (_fftOut[i]*_fftOut[i] + _fftOut[i + bands]*_fftOut[i + bands]) * normalization;
		}
		bins[bin] = sum * invBinWidth;
	}
}

Seeds::Seeds() {
#ifdef ARCH_WIN
  _generator.seed(time(0));
#else
  std::random_device rd;
  _generator.seed(rd());
#endif
}

unsigned int Seeds::_next() {
  return _generator();
}

Seeds& Seeds::getInstance() {
  static Seeds instance;
  return instance;
}

unsigned int Seeds::next() {
  return getInstance()._next();
};


void RandomWalk::setParams(float sampleRate, float change) {
	assert(sampleRate > 0.0f);
	assert(change >= 0.0f);
	assert(change <= 1.0f);

	_filter.setParams(sampleRate, std::max(2.0f, change * 0.49f * std::min(44100.0f, sampleRate)));

	const float maxDamp = 0.98;
	const float minDamp = 0.9999;
	_damp = maxDamp + (1.0f - change)*(minDamp - maxDamp);

	_biasDamp = 1.0f - change*(2.0f / sampleRate);
}

void RandomWalk::jump() {
	float x = fabsf(_noise.next()) * (_max - _min);
	x += _min;
	tell(x);
}

void RandomWalk::tell(float v) {
	assert(v >= _min && v <= _max);
	_last = _bias = v;
	_filter.reset();
}

float RandomWalk::_next() {
	float delta = _noise.next();
	if ((_lastOut >= _max && delta > 0.0f) || (_lastOut <= _min && delta < 0.0f)) {
		delta = -delta;
	}
	_last = _damp*_last + delta;
	_bias *= _biasDamp;
	return _lastOut = std::min(std::max(_bias + _filter.next(_last), _min), _max);
}
}

