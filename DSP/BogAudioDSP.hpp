/*
    Fixed
    Table
    FastTanhf
    frequencyToSemitone
    semitoneToFrequency
    frequencyToCV
    cvToFrequency
    cvToSemitone
    semitoneToCV
    OverlappingBuffer
    AveragingBuffer
    HistoryBuffer
    decibelToAmplitude
    amplitudeToDecibels
    Amplifier
    RunningAverage
    PositiveZeroCrossing
    SlewLimiter
    ShapedSlewLimiter
    Integrator
    CrossFader
    Panner
    DelayLine
    Limiter
    Saturator
    Compressor
    NoiseGate
    Filter
    BiquadFilter
    LowPassFilter
    BiquadBank
    MultimodeDesigner
    MultimodeFilter
    FourPoleButterworthLowpassFilter
    FourPoleButterworthHighpassFilter
    TwoPoleButterworthBandpassFilter
    FourPoleButterwortBandpassFilter
    ComplexBiquadFilter
    DCBlocker
    PureRootMeanSquare
    PucketteEnvelopeFollower
    AveragRecitifiedValue
    FastRootMeanSquare
    Decimator
    LPFDecimator
    CICDecimator
    Interpolator
    CICInterpolator
    Equalizer
    Timer
    Window
    FFT1024
    FFT4096
    FFT8192
    FFT16384
    FFT32768
    Window
    HanningWindow
    HammingWindow
    KaiserWindow
    PlanckTaperWindow
    SpectrumAnalyzer
    BlepTable
    Seeds
    BasePinkNoiseGenerator
    PinkNoiseGenerator
    RedNoiseGenerator
    BlueNoiseGenerator
    GaussianNoiseGenerator
    RandomWalk
    EnvelopeGenerator
    ADSR
    Oscillator
    OscillatorGenerator
    Phasor
    TablePhasor
    SineOscillator
    SineTableOScillator
    SawOScillator    
    SaturatingSawOscillator
    BandlimitedSawOscillator
    SquareOscillator
    BnadlimitedSquareOscillator
    TriangleOscillator    
    SteppedRandomOscillator
    SineBankOscillator
    ChirpOscillator
    PureChirpOscillator
*/
#pragma once
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include <vector>
#include <mutex>
#include <complex>
#include <chrono>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>

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
	struct Generator : public GeneratorProcessor {
		float _current = 0.0;

		Generator() : GeneratorProcessor() {}
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
}



#include "BogAudioDelayLine.hpp"
#include "BogAudioMath.hpp"
#include "BogAudioSignal.hpp"
#include "BogAudioDynamics.hpp"
#include "BogAudioCrossfaderPanner.hpp"
#include "BogAudioEnvelope.hpp"
#include "BogAudioFilters.hpp"
#include "BogAudioEqualizer.hpp"
#include "BogAudioIntegrator.hpp"
#include "BogAudioNoise.hpp"
#include "BogAudioOscillator.hpp"
#include "BogAudioPitch.hpp"
#include "BogAudioResample.hpp"
#include "BogAudioSlewLimiter.hpp"
#include "BogAudioSpectrumAnalyzer.hpp"
#include "BogAudioUtils.hpp"
#include "BogAudioZeroCrossing.hpp"