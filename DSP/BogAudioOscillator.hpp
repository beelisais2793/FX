#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{

	////////////////////////////////////////////////////////////////////////////////////////
	// Oscillator
	////////////////////////////////////////////////////////////////////////////////////////
	struct Oscillator : public OscillatorProcessor {
		double _sampleRate;
		double _frequency;

		Oscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		:
		OscillatorProcessor() 
		,_sampleRate(sampleRate > 1.0 ? sampleRate : 1.0)
		, _frequency(frequency)
		{
		}
		virtual ~Oscillator() {}

		void setSampleRate(double sampleRate) {
			if (_sampleRate != sampleRate && sampleRate >= 1.0) {
				_sampleRate = sampleRate;
				_sampleRateChanged();
			}
		}

		virtual void _sampleRateChanged() {}

		void setFrequency(double frequency) {
			if (_frequency != frequency) {
				_frequency = frequency;
				_frequencyChanged();
			}
		}

		virtual void _frequencyChanged() {}
	};


	struct OscillatorGenerator : Oscillator, Generator {
		OscillatorGenerator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: Oscillator(sampleRate, frequency)
		{
		}
	};


	struct Phasor : OscillatorGenerator {
		typedef uint64_t phase_t;
		typedef int64_t phase_delta_t;
		static constexpr phase_t cyclePhase = UINT32_MAX;
		static constexpr double twoPI = 2.0f * M_PI;
		static constexpr double maxSampleWidth = 0.25f;

		phase_delta_t _delta;
		phase_t _phase = 0;
		double _sampleWidth = 0.0f;
		phase_t _samplePhase = 0;

		Phasor(
			double sampleRate = 1000.0f,
			double frequency = 100.0f,
			double initialPhase = 0.0f
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

		void setSampleWidth(double sw);
		virtual void resetPhase();
		void setPhase(double radians);
		void syncPhase(const Phasor& phasor);
		double nextFromPhasor(const Phasor& phasor, phase_delta_t offset = 0);
		virtual double nextForPhase(phase_t phase);
		virtual void _update();
		inline void advancePhase() { _phase += _delta; }
		inline void advancePhase(int n) { assert(n > 0); _phase += n * _delta; }
		double _next() override final;

		inline static phase_delta_t radiansToPhase(double radians) { return (radians / twoPI) * cyclePhase; }
		inline static double phaseToRadians(phase_t phase) { return (phase / (double)cyclePhase) * twoPI; }
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

		double nextForPhase(phase_t phase) override;
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
		double _next() override;
	};

    
	struct SineTableOscillator : TablePhasor {
		SineTableOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: TablePhasor(StaticSineTable::table(), sampleRate, frequency)
		{
		}
	};

	struct SawOscillator : Phasor {
		SawOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		{
		}

		double nextForPhase(phase_t phase) override;
	};

	struct SaturatingSawOscillator : SawOscillator {
		double _saturation;
		double _saturationNormalization;
		FastTanhf _tanhf;

		SaturatingSawOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: SawOscillator(sampleRate, frequency)
		, _saturation(0.0f)
		, _saturationNormalization(1.0f)
		{
		}

		void setSaturation(double saturation);
		double nextForPhase(phase_t phase) override;
	};

	struct BandLimitedSawOscillator : SaturatingSawOscillator {
		int _quality;
		const Table& _table;
		phase_t _qd = 0;
		double _halfTableLen;

		BandLimitedSawOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f,
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
		double nextForPhase(phase_t phase) override;
	};


	struct SquareOscillator : Phasor {
		static constexpr double minPulseWidth = 0.03f;
		static constexpr double maxPulseWidth = 1.0f - minPulseWidth;
		static constexpr double defaultPulseWidth = 0.5f;
		double _pulseWidthInput = -1.0f;
		phase_t _lastCycle = -1;
		phase_t _pulseWidth = cyclePhase * defaultPulseWidth, _nextPulseWidth = cyclePhase * defaultPulseWidth;
		bool positive = true;

		SquareOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		{
		}

		void setPulseWidth(double pw);
		double nextForPhase(phase_t phase) override;
	};


	struct BandLimitedSquareOscillator : BandLimitedSawOscillator {
		const double minPulseWidth = 0.03f;
		const double maxPulseWidth = 1.0f - minPulseWidth;
		double _pulseWidthInput = -1.0f;
		bool _dcCorrection = false;
		phase_t _lastCycle = -1;
		phase_delta_t _pulseWidth = 0, _nextPulseWidth = 0;
		double _offset = 0.0f, _nextOffset = 0.0f;
		double _dcOffset = 0.0f, _nextDcOffset = 0.0f;

		BandLimitedSquareOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f,
			int quality = 5,
			const Table& table = StaticBlepTable::table()
		)
		: BandLimitedSawOscillator(sampleRate, frequency, quality, table)
		{
			setPulseWidth(0.5f);
		}

		void setPulseWidth(double pw, bool dcCorrection = true);
		double nextForPhase(phase_t phase) override;
	};


    
	struct TriangleOscillator : Phasor {
		const phase_t quarterCyclePhase = cyclePhase * 0.25f;
		const phase_t threeQuartersCyclePhase = cyclePhase * 0.75f;

		TriangleOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f
		)
		: Phasor(sampleRate, frequency)
		{
		}

		double nextForPhase(phase_t phase) override;
	};

    
	struct SteppedRandomOscillator : Phasor {
		const phase_t _n;
		const phase_t _k;
		double* _t = NULL;
		phase_t _seed;

		SteppedRandomOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f,
			phase_t seed = 0
		);
		~SteppedRandomOscillator();

		void resetPhase() override;
		double nextForPhase(phase_t phase) override;
	};

    
	struct SineBankOscillator : Oscillator {
		struct Partial {
			double frequency;
			double frequencyRatio;
			double amplitude;
			double amplitudeTarget;
			double amplitudeStepDelta;
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

		const double _maxPartialFrequencySRRatio = 0.48;
		double _maxPartialFrequency = 0.0;
		const int _amplitudeEnvelopeMS = 10;
		int _amplitudeEnvelopeSamples = 0;
		std::vector<Partial> _partials;

		SineBankOscillator(
			double sampleRate = 1000.0f,
			double frequency = 100.0f,
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
		void setPartial(int i, double frequencyRatio, double amplitude);
		bool setPartialFrequencyRatio(int i, double frequencyRatio);
		void setPartialAmplitude(int i, double amplitude, bool envelope = false);

		void syncToPhase(double phase);
		void syncTo(const SineBankOscillator& other);

		void _sampleRateChanged() override;
		void _frequencyChanged() override;
		double next(Phasor::phase_t phaseOffset = 0.0f);
	};


    
	struct ChirpOscillator : OscillatorGenerator {
		static constexpr double minFrequency = 1.0f;
		static constexpr double minTimeSeconds = 0.01f;

		SineTableOscillator _oscillator;
		double _f1 = -1.0f;
		double _f2 = -1.0f;
		double _Time = -1.0f;
		bool _linear = false;

		double _sampleTime = 0.0f;
		double _time = 0.0f;
		bool _complete = false;
		double _k = 0.0;

		ChirpOscillator(
			double sampleRate = 1000.0f,
			double frequency1 = 100.0f,
			double frequency2 = 300.0f,
			double time = 1.0f,
			bool linear = true
		)
		: _oscillator(sampleRate)
		{
			setParams(frequency1, frequency2, time, linear);
		}

		inline bool isCycleComplete() { return _complete; }
		inline bool isCycleNearlyComplete(double seconds) { return _time > _Time - seconds; }
		void setParams(double frequency1, double frequency2, double time, bool linear);
		void _sampleRateChanged() override;
		double _next() override;
		void reset();
	};

    
	struct PureChirpOscillator : OscillatorGenerator {
		static constexpr double minFrequency = 1.0f;
		static constexpr double minTimeSeconds = 0.01f;

		double _f1 = -1.0f;
		double _f2 = -1.0f;
		double _Time = -1.0;
		bool _linear = false;

		double _sampleTime = 0.0f;
		double _time = 0.0f;
		bool _complete = false;
		double _c = 0.0;
		double _k = 0.0;
		double _invlogk = 0.0;

		PureChirpOscillator(
			double sampleRate = 1000.0f,
			double frequency1 = 100.0f,
			double frequency2 = 300.0f,
			double time = 1.0f,
			bool linear = true
		)
		{
			setParams(frequency1, frequency2, time, linear);
		}

		inline bool isCycleComplete() { return _complete; }
		inline bool isCycleNearlyComplete(double seconds) { return _time > _Time - seconds; }
		void setParams(double frequency1, double frequency2, double time, bool linear);
		void _sampleRateChanged() override;
		void update();
		double _next() override;
		void reset();
	};

	void Phasor::setSampleWidth(double sw) {
		if (sw < 0.0f) {
			sw = 0.0f;
		}
		else if (sw > maxSampleWidth) {
			sw = maxSampleWidth;
		}
		if (_sampleWidth != sw) {
			_sampleWidth = sw;
			if (_sampleWidth > 0.001f) {
				_samplePhase = _sampleWidth * (double)cyclePhase;
			}
			else {
				_samplePhase = 0;
			}
		}
	}

	void Phasor::resetPhase() {
		_phase = 0;
	}

	void Phasor::setPhase(double radians) {
		_phase = radiansToPhase(radians);
	}

	void Phasor::syncPhase(const Phasor& phasor) {
		_phase = phasor._phase;
	}

	double Phasor::nextFromPhasor(const Phasor& phasor, phase_delta_t offset) {
		offset += phasor._phase;
		if (_samplePhase > 0) {
			offset -= offset % _samplePhase;
		}
		return nextForPhase(offset);
	}

	void Phasor::_update() {
		_delta = ((phase_delta_t)((_frequency / _sampleRate) * cyclePhase)) % cyclePhase;
	}

	double Phasor::_next() {
		advancePhase();
		if (_samplePhase > 0) {
			return nextForPhase(_phase - (_phase % _samplePhase));
		}
		return nextForPhase(_phase);
	}

	double Phasor::nextForPhase(phase_t phase) {
		return phase;
	}


	double TablePhasor::nextForPhase(phase_t phase) {
		phase %= cyclePhase;
		if (_tableLength >= 1024) {
			int i = (((((uint64_t)phase) << 16) / cyclePhase) * _tableLength) >> 16;
			i %= _tableLength;
			return _table.value(i);
		}

		double fi = phase / (double)cyclePhase;
		assert(fi >= 0.0f && fi < 1.0f);
		fi *= _tableLength;
		int i = fi;
		double v1 = _table.value(i);
		double v2 = _table.value(i + 1 == _tableLength ? 0 : i + 1);
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

	double SineOscillator::_next() {
		double t = _x - _k1*_y;
		_y = _y + _k2*t;
		_x = t - _k1*_y;
		return _y;
	}


	double SawOscillator::nextForPhase(phase_t phase) {
		return ((phase % cyclePhase) / (double)cyclePhase) * 2.0f - 1.0f;
	}


	void SaturatingSawOscillator::setSaturation(double saturation) {
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

	double SaturatingSawOscillator::nextForPhase(phase_t phase) {
		double sample = SawOscillator::nextForPhase(phase);
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

	double BandLimitedSawOscillator::nextForPhase(phase_t phase) {
		phase %= cyclePhase;

		double sample = SaturatingSawOscillator::nextForPhase(phase);
		if (phase > cyclePhase - _qd) {
			double i = (cyclePhase - phase) / (double)_qd;
			i = (1.0f - i) * _halfTableLen;
			sample -= _table.value((int)i);
		}
		else if (phase < _qd) {
			double i = phase / (double)_qd;
			i *= _halfTableLen - 1;
			i += _halfTableLen;
			sample -= _table.value((int)i);
		}
		return sample;
	}


	void SquareOscillator::setPulseWidth(double pw) {
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

	double SquareOscillator::nextForPhase(phase_t phase) {
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


	void BandLimitedSquareOscillator::setPulseWidth(double pw, bool dcCorrection) {
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

	double BandLimitedSquareOscillator::nextForPhase(phase_t phase) {
		phase_t cycle = phase / cyclePhase;
		if (_lastCycle != cycle) {
			_lastCycle = cycle;
			_pulseWidth = _nextPulseWidth;
			_offset = _nextOffset;
			_dcOffset = _nextDcOffset;
		}

		double sample = -BandLimitedSawOscillator::nextForPhase(phase);
		sample += BandLimitedSawOscillator::nextForPhase(phase - _pulseWidth);
		return sample + _offset + _dcOffset;
	}


	double TriangleOscillator::nextForPhase(phase_t phase) {
		phase %= cyclePhase;

		double p = (phase / (double)cyclePhase) * 4.0f;
		if (phase < quarterCyclePhase) {
			return p;
		}
		if (phase < threeQuartersCyclePhase) {
			return 2.0f - p;
		}
		return p - 4.0f;
	}


	SteppedRandomOscillator::SteppedRandomOscillator(
		double sampleRate,
		double frequency,
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
		_t = new double[_n];
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

	double SteppedRandomOscillator::nextForPhase(phase_t phase) {
		phase_t i = phase / cyclePhase;
		return _t[(_seed + i + (_seed + i) % _k) % _n];
	}


	void SineBankOscillator::setPartial(int i, double frequencyRatio, double amplitude) {
		setPartialFrequencyRatio(i, frequencyRatio);
		setPartialAmplitude(i, amplitude);
	}

	bool SineBankOscillator::setPartialFrequencyRatio(int i, double frequencyRatio) {
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

	void SineBankOscillator::setPartialAmplitude(int i, double amplitude, bool envelope) {
		if (i <= (int)_partials.size()) {
			Partial& p = _partials[i - 1];
			if (envelope) {
				p.amplitudeTarget = amplitude;
				p.amplitudeStepDelta = (amplitude - p.amplitude) / (double)_amplitudeEnvelopeSamples;
				p.amplitudeSteps = _amplitudeEnvelopeSamples;
			}
			else if (p.amplitudeSteps > 0) {
				p.amplitudeTarget = amplitude;
				p.amplitudeStepDelta = (amplitude - p.amplitude) / (double)p.amplitudeSteps;
			}
			else {
				p.amplitude = amplitude;
			}
		}
	}

	void SineBankOscillator::syncToPhase(double phase) {
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

	double SineBankOscillator::next(Phasor::phase_t phaseOffset) {
		double next = 0.0;
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


	constexpr double ChirpOscillator::minFrequency;
	constexpr double ChirpOscillator::minTimeSeconds;

	void ChirpOscillator::setParams(double frequency1, double frequency2, double time, bool linear) {
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

	double ChirpOscillator::_next() {
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


	constexpr double PureChirpOscillator::minFrequency;
	constexpr double PureChirpOscillator::minTimeSeconds;

	void PureChirpOscillator::setParams(double frequency1, double frequency2, double time, bool linear) {
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

	double PureChirpOscillator::_next() {
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


}