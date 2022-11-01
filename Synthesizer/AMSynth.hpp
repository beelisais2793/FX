#pragma once

#include <cstdlib>
#include <climits>
#include <algorithm>
#include <cmath>


typedef enum {
	kAmsynthParameter_AmpEnvAttack             = 0,
	kAmsynthParameter_AmpEnvDecay              = 1,
	kAmsynthParameter_AmpEnvSustain            = 2,
	kAmsynthParameter_AmpEnvRelease            = 3,
	
	kAmsynthParameter_Oscillator1Waveform      = 4,
	
	kAmsynthParameter_FilterEnvAttack          = 5,
	kAmsynthParameter_FilterEnvDecay           = 6,
	kAmsynthParameter_FilterEnvSustain         = 7,
	kAmsynthParameter_FilterEnvRelease         = 8,
	kAmsynthParameter_FilterResonance          = 9,
	kAmsynthParameter_FilterEnvAmount          = 10,
	kAmsynthParameter_FilterCutoff             = 11,
	
	kAmsynthParameter_Oscillator2Detune        = 12,
	kAmsynthParameter_Oscillator2Waveform      = 13,
	
	kAmsynthParameter_MasterVolume             = 14,
	
	kAmsynthParameter_LFOFreq                  = 15,
	kAmsynthParameter_LFOWaveform              = 16,
	
	kAmsynthParameter_Oscillator2Octave        = 17,
	kAmsynthParameter_OscillatorMix            = 18,
	
	kAmsynthParameter_LFOToOscillators         = 19,
	kAmsynthParameter_LFOToFilterCutoff        = 20,
	kAmsynthParameter_LFOToAmp                 = 21,
	
	kAmsynthParameter_OscillatorMixRingMod     = 22,
	
	kAmsynthParameter_Oscillator1Pulsewidth    = 23,
	kAmsynthParameter_Oscillator2Pulsewidth    = 24,
	
	kAmsynthParameter_ReverbRoomsize           = 25,
	kAmsynthParameter_ReverbDamp               = 26,
	kAmsynthParameter_ReverbWet                = 27,
	kAmsynthParameter_ReverbWidth              = 28,
	
	kAmsynthParameter_AmpDistortion            = 29,
	
	kAmsynthParameter_Oscillator2Sync          = 30,

	kAmsynthParameter_PortamentoTime           = 31,
	
	kAmsynthParameter_KeyboardMode             = 32,

	kAmsynthParameter_Oscillator2Pitch         = 33,
	kAmsynthParameter_FilterType               = 34,
	kAmsynthParameter_FilterSlope              = 35,

	kAmsynthParameter_LFOOscillatorSelect      = 36,

	kAmsynthParameter_FilterKeyTrackAmount     = 37,
	kAmsynthParameter_FilterKeyVelocityAmount  = 38,
	
	kAmsynthParameter_AmpVelocityAmount        = 39,
	
	kAmsynthParameter_PortamentoMode           = 40,

	kAmsynthParameterCount
} Param;

typedef enum {
	KeyboardModePoly,
	KeyboardModeMono,
	KeyboardModeLegato,
} KeyboardMode;

typedef enum {
	PortamentoModeAlways,
	PortamentoModeLegato
} PortamentoMode;

#ifdef __cplusplus
extern "C" {
#endif

void get_parameter_properties(int parameter_index, double *minimum, double *maximum, double *default_value, double *step_size);

const char *parameter_name_from_index (int param_index);
int parameter_index_from_name (const char *param_name);

int parameter_get_display (int parameter_index, DspFloatType parameter_value, char *buffer, size_t maxlen);
const char **parameter_get_value_strings (int parameter_index);

#ifdef __cplusplus
}
#endif



namespace m {
	// The mathematical constant e
	static const DspFloatType e = 2.7182818284590452354F;
	// The mathematical constant pi
	static const DspFloatType pi = 3.14159265358979323846F;
	// The mathematical constant pi * 2
	static const DspFloatType twoPi = 6.28318530717958647692F;
	// The mathematical constant pi / 2
	static const DspFloatType halfPi = 1.57079632679489661923F;
	// A quiet NaN (“not a number”)
	static const DspFloatType nan = std::nanf("");
}

class Lerper
{
public:

	void configure(DspFloatType startValue, DspFloatType finalValue, unsigned int numSteps)
	{
		_start = startValue;
		_final = finalValue;
		_steps = numSteps;
		if (0 < _steps) {
			_inc = (_final - _start) / (DspFloatType)_steps;
		} else {
			_inc = 0.0F;
			_start = finalValue;
		}
		_i = 0;
	}

	inline DspFloatType getValue() const
	{
		return _start + _inc * static_cast<DspFloatType>(_i);
	}
	
	inline DspFloatType nextValue()
	{
		DspFloatType y = getValue();
		_i = std::min(_i + 1, _steps);
		return y;
	}

	inline DspFloatType getFinalValue() const
	{
		return _final;
	}
	
private:

	DspFloatType _start = 0;
	DspFloatType _final = 0;
	DspFloatType _inc = 0;
	unsigned int _steps = 0;
	unsigned int _i = 0;
};

struct IIRFilterFirstOrder
{
	enum class Mode
	{
		kLowPass,
		kHighPass,
	};
	
	void setCoefficients(DspFloatType sampleRate, DspFloatType cutoffFreq, Mode mode)
	{
		DspFloatType x = powf(m::e, -m::halfPi * std::min(cutoffFreq / sampleRate, 0.5F));
		if (mode == Mode::kLowPass) {
			_a0 = 1.0F - x;
			_a1 = 0.0F;
			_b1 = x;
		} else {
			_a0 =  (1 + x) / 2.0F;
			_a1 = -(1 + x) / 2.0F;
			_b1 = x;
		}
	}
	
	inline DspFloatType processSample(DspFloatType x)
	{
		DspFloatType y = (x * _a0) + _z;
		_z = (x * _a1) + (y * _b1);
		return y;
	}
	
	void processBuffer(DspFloatType *samples, unsigned numSamples)
	{
		for (unsigned i=0; i<numSamples; i++) {
			samples[i] = processSample(samples[i]);
		}
	}
	
	DspFloatType _a0 = 0;
	DspFloatType _a1 = 0;
	DspFloatType _b1 = 0;
	DspFloatType _z = 0;
};

class ParamSmoother
{
public:
	ParamSmoother(DspFloatType z): _z(z) {}
	
	inline DspFloatType processSample(DspFloatType x)
	{
		return (_z += ((x - _z) * 0.005F));
	}
	
	inline void set(DspFloatType z)
	{
		_z = z;
	}
	
private:
	DspFloatType _z;
};

class SmoothedParam
{
public:
	
	SmoothedParam(DspFloatType rawValue = 0.f): _rawValue(rawValue), _smoother(rawValue) {}
	~SmoothedParam() = default;
	
	SmoothedParam(const SmoothedParam&) = delete;
	SmoothedParam& operator=(const SmoothedParam&) = delete;
	
	SmoothedParam(SmoothedParam&&) = delete;
	SmoothedParam& operator=(SmoothedParam&&) = delete;
	
	SmoothedParam& operator=(DspFloatType rawValue)
	{
		_rawValue = rawValue;
		return *this;
	}
	
	DspFloatType getRawValue()
	{
		return _rawValue;
	}

	void reset()
	{
		_smoother.set(_rawValue);
	}
	
	inline DspFloatType tick()
	{
		return _smoother.processSample(_rawValue);
	}
	
private:
	
	DspFloatType _rawValue;
	ParamSmoother _smoother;
};

class ADSR
{
public:
	enum class State {
		kAttack,
		kDecay,
		kSustain,
		kRelease,
		kOff
	};

	void	SetSampleRate	(int value) { m_sample_rate = value; }

	void	SetAttack	(DspFloatType value) { m_attack = value; }
	void	SetDecay	(DspFloatType value) { m_decay = value; }
	void	SetSustain	(DspFloatType value) { m_sustain = value; if (m_state == State::kSustain) m_value = value; }
	void	SetRelease	(DspFloatType value) { m_release = value; }
	
	void	process		(DspFloatType *buffer, unsigned frames);
	
	void	triggerOn	();
	void	triggerOff	();

	int		getState	() { return (m_state == State::kOff) ? 0 : 1; };

	/**
	 * puts the envelope directly into the off (ADSR_OFF) state, without
	 * going through the usual stages (ADSR_R).
	 */
	void reset();

private:
	DspFloatType			m_attack = 0;
	DspFloatType			m_decay = 0;
	DspFloatType			m_sustain = 1;
	ParamSmoother	m_sustain_smoother{1.f};
	DspFloatType			m_release = 0;

	DspFloatType			m_sample_rate = 44100;
	State			m_state = State::kOff;

	DspFloatType			m_value = 0.0F;
	DspFloatType			m_inc = 0.0F;
	unsigned		m_frames_left_in_state = UINT_MAX;
};

/**
 * @brief An Audio Oscillator unit.
 * 
 * Provides several different output waveforms (sine, saw, square, noise, 
 * random).
 */
class Oscillator
{
public:
	enum class Waveform { 
		kSine,
		kPulse,
		kSaw,
		kNoise,
		kRandom
	};

	void	SetSampleRate	(int rateIn);
	
	void	ProcessSamples		(DspFloatType*, int, DspFloatType freq_hz, DspFloatType pw, DspFloatType sync_freq = 0);

	void	SetWaveform		(Waveform);
	Waveform GetWaveform() { return waveform; }

	void reset();
	
	void	setSyncEnabled(bool sync) { mSyncEnabled = sync; }
	void	setPolarity (DspFloatType polarity); // +1 or -1

private:
    DspFloatType rads = 0;
	DspFloatType twopi_rate = 0;
	DspFloatType random = 0;
    int rate = 44100;
	int random_count = 0;

	Waveform waveform = Waveform::kSine;
	Lerper	mFrequency;
	DspFloatType	mPulseWidth = 0;
	DspFloatType	mPolarity = 1;
	
	DspFloatType	mSyncFrequency = 0;
	bool	mSyncEnabled = false;
	double	mSyncRads = 0;
	
    void doSine(DspFloatType*, int nFrames);
    void doSquare(DspFloatType*, int nFrames);
    void doSaw(DspFloatType*, int nFrames);
    void doNoise(DspFloatType*, int nFrames);
	void doRandom(DspFloatType*, int nFrames);
};

class SynthFilter
{
public:

	enum class Type {
		kLowPass,
		kHighPass,
		kBandPass,
		kBandStop,
		kBypass
	};

	enum class Slope {
		k12,
		k24,
	};

	void SetSampleRate(int rateIn) { rate = (DspFloatType)rateIn; nyquist = rate / 2.0f; }

	void reset();

	void ProcessSamples(DspFloatType *, int, DspFloatType cutoff, DspFloatType res, Type type, Slope slope);

private:

	DspFloatType rate = 44100;
	DspFloatType nyquist = 22050.0;
	double d1 = 0;
	double d2 = 0;
	double d3 = 0;
	double d4 = 0;
};

/**
 * the VoiceBoard is what makes the nice noises... ;-)
 *
 * one VoiceBoard is a 'voice' in its own right, and play only one note at a 
 * time. the VoiceAllocationUnit decides which voices do what etc...
 **/

class VoiceBoard
{
public:

	static constexpr int kMaxProcessBufferSize = 64;

	bool	isSilent		();
	void	triggerOn		(bool reset);
	void	triggerOff		();
	void	setVelocity		(DspFloatType velocity);
	
	void	setFrequency	(DspFloatType startFrequency, DspFloatType targetFrequency, DspFloatType time = 0.0f);
	DspFloatType	getFrequency	() { return mFrequency.getValue(); }
	
	void	SetPitchBend	(DspFloatType);
	void	reset			();

	void	UpdateParameter		(Param, DspFloatType);

	void	ProcessSamplesMix	(DspFloatType *buffer, int numSamples, DspFloatType vol);

	void	SetSampleRate		(int);

private:

	ParamSmoother	mVolume{0.f};

	Lerper			mFrequency;
	bool			mFrequencyDirty = false;
	DspFloatType			mFrequencyStart = 0;
	DspFloatType			mFrequencyTarget = 0;
	DspFloatType			mFrequencyTime = 0;

	DspFloatType			mSampleRate = 44100;
	DspFloatType			mKeyVelocity = 1;
	DspFloatType			mPitchBend = 1;
	
	// modulation section
	Oscillator 		lfo1;
	DspFloatType			mLFO1Freq = 0;
	DspFloatType			mLFOPulseWidth = 0;
	
	// oscillator section
	Oscillator 		osc1, osc2;
	DspFloatType			mFreqModAmount = 0;
	int				mFreqModDestination = 0;
	DspFloatType			mOsc1PulseWidth = 0;
	DspFloatType			mOsc2PulseWidth = 0;
	SmoothedParam	mOscMix{0.f};
	SmoothedParam	mRingModAmt{0.f};
	DspFloatType			mOsc2Octave = 1;
	DspFloatType			mOsc2Detune = 1;
	DspFloatType			mOsc2Pitch = 0;
	bool			mOsc2Sync = false;
	
	// filter section
	DspFloatType			mFilterEnvAmt = 0;
	DspFloatType			mFilterModAmt = 0;
	DspFloatType			mFilterCutoff = 16;
	DspFloatType			mFilterRes = 0;
	DspFloatType			mFilterKbdTrack = 0;
	DspFloatType			mFilterVelSens = 0;
	SynthFilter 	filter;
	SynthFilter::Type mFilterType;
	SynthFilter::Slope mFilterSlope;
	ADSR 			mFilterADSR;
	
	// amp section
	IIRFilterFirstOrder _vcaFilter;
	SmoothedParam	mAmpModAmount{-1.f};
	SmoothedParam	mAmpVelSens{1.f};
	ADSR 			mAmpADSR;

	struct {
		DspFloatType osc_1[kMaxProcessBufferSize];
		DspFloatType osc_2[kMaxProcessBufferSize];
		DspFloatType lfo_osc_1[kMaxProcessBufferSize];
		DspFloatType filter_env[kMaxProcessBufferSize];
		DspFloatType amp_env[kMaxProcessBufferSize];
	} mProcessBuffers;
};


static const DspFloatType kMinimumTime = 0.0005f;

void
ADSR::triggerOn()
{
	m_state = State::kAttack;
	m_frames_left_in_state = (int) (m_attack * m_sample_rate);
	const DspFloatType target = m_decay <= kMinimumTime ? m_sustain : 1.0;
	m_inc = (target - m_value) / (DspFloatType) m_frames_left_in_state;
}

void 
ADSR::triggerOff()
{
	m_state = State::kRelease;
	m_frames_left_in_state = (int) (m_release * m_sample_rate);
	m_inc = (0.f - m_value) / (DspFloatType) m_frames_left_in_state;
}

void
ADSR::reset()
{
	m_state = State::kOff;
	m_value = 0;
	m_inc = 0;
	m_frames_left_in_state = UINT_MAX;
}

void
ADSR::process(DspFloatType *buffer, unsigned frames)
{
	while (frames) {

		const unsigned int count = std::min(frames, m_frames_left_in_state);

		if (m_state == State::kSustain) {
			for (unsigned i = 0; i < count; i++) {
				*buffer = m_value;
				m_value = m_sustain_smoother.processSample(m_sustain);
				buffer++;
			}
		} else {
			for (unsigned i = 0; i < count; i++) {
				*buffer = m_value;
				m_value += m_inc;
				buffer++;
			}
		}

		m_frames_left_in_state -= count;

		if (m_frames_left_in_state == 0) {
			switch (m_state) {
				case State::kAttack:
					m_state = State::kDecay;
					m_frames_left_in_state = (int) (m_decay * m_sample_rate);
					m_inc = (m_sustain - m_value) / (DspFloatType) m_frames_left_in_state;
					break;
				case State::kDecay:
					m_sustain_smoother.set(m_value);
					m_state = State::kSustain;
					m_frames_left_in_state = UINT_MAX;
					m_inc = 0;
					break;
				case State::kSustain:
					m_frames_left_in_state = UINT_MAX;
					break;
				case State::kRelease:
				case State::kOff:
					m_state = State::kOff;
					m_value = 0;
					m_frames_left_in_state = UINT_MAX;
					m_inc = 0;
					break;
				default:
					assert(nullptr == "invalid state");
			}
		}

		frames -= count;
	}
}

void
SynthFilter::reset()
{
	d1 = d2 = d3 = d4 = 0;
}

void
SynthFilter::ProcessSamples(DspFloatType *buffer, int numSamples, DspFloatType cutoff, DspFloatType res, Type type, Slope slope)
{
	if (type == Type::kBypass) {
		return;
	}
	
	cutoff = std::min(cutoff, nyquist * 0.99f); // filter is unstable at PI
	cutoff = std::max(cutoff, 10.0f);

	const double w = (cutoff / rate); // cutoff freq [ 0 <= w <= 0.5 ]
	const double r = std::max(0.001, 2.0 * (1.0 - res)); // r is 1/Q (sqrt(2) for a butterworth response)

	const double k = tan(w * m::pi);
	const double k2 = k * k;
	const double rk = r * k;
	const double bh = 1.0 + rk + k2;

	double a0, a1, a2, b1, b2;

	switch (type) {
		case Type::kLowPass:
			//
			// Bilinear transformation of H(s) = 1 / (s^2 + s/Q + 1)
			// See "Digital Audio Signal Processing" by Udo Zölzer
			//
			a0 = k2 / bh;
			a1 = a0 * 2.0;
			a2 = a0;
			b1 = (2.0 * (k2 - 1.0)) / bh;
			b2 = (1.0 - rk + k2) / bh;
			break;

		case Type::kHighPass:
			//
			// Bilinear transformation of H(s) = s^2 / (s^2 + s/Q + 1)
			// See "Digital Audio Signal Processing" by Udo Zölzer
			//
			a0 =  1.0 / bh;
			a1 = -2.0 / bh;
			a2 =  a0;
			b1 = (2.0 * (k2 - 1.0)) / bh;
			b2 = (1.0 - rk + k2) / bh;
			break;
		
		case Type::kBandPass:
			//
			// Bilinear transformation of H(s) = (s/Q) / (s^2 + s/Q + 1)
			// See "Digital Audio Signal Processing" by Udo Zölzer
			//
			a0 =  rk / bh;
			a1 =  0.0;
			a2 = -rk / bh;
			b1 = (2.0 * (k2 - 1.0)) / bh;
			b2 = (1.0 - rk + k2) / bh;
			break;
			
		case Type::kBandStop:
			//
			// "Digital Audio Signal Processing" by Udo Zölzer does not provide z-transform
			// coefficients for the bandstop filter, so these were derived by studying
			// http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
			//
			a0 = (1.0 + k2) / bh;
			a1 = (2.0 * (k2 - 1.0)) / bh;
			a2 =  a0;
			b1 =  a1;
			b2 = (1.0 - rk + k2) / bh;
			break;

		case Type::kBypass:
			return;

		default:
			assert(nullptr == "invalid FilterType");
			return;
	}

	switch (slope) {
		case Slope::k12:
			for (int i=0; i<numSamples; i++) { double y, x = buffer[i];

				y  =      (a0 * x) + d1;
				d1 = d2 + (a1 * x) - (b1 * y);
				d2 =      (a2 * x) - (b2 * y);

				buffer[i] = (DspFloatType) y;
			}
			break;

		case Slope::k24:
			for (int i=0; i<numSamples; i++) { double y, x = buffer[i];

				y  =      (a0 * x) + d1;
				d1 = d2 + (a1 * x) - (b1 * y);
				d2 =      (a2 * x) - (b2 * y);

				x = y;

				y  =      (a0 * x) + d3;
				d3 = d4 + (a1 * x) - (b1 * y);
				d4 =      (a2 * x) - (b2 * y);

				buffer[i] = (DspFloatType) y;
			}
			break;

		default:
			assert(nullptr == "invalid FilterSlope");
			break;
	}
}

#define ALIAS_REDUCTION

static inline DspFloatType ffmodf(DspFloatType x, DspFloatType y) {
	return (x - y * (int)(x / y));
}

#define DO_OSC_SYNC(__osc_rads__) \
	if (mSyncEnabled) { \
		mSyncRads = mSyncRads + twopi_rate * mSyncFrequency; \
		if (mSyncRads >= m::twoPi) { \
			mSyncRads -= m::twoPi; \
			__osc_rads__ = 0; \
		} \
	}

void Oscillator::SetWaveform	(Waveform w)			{ waveform = w; }
void Oscillator::reset			()						{ rads = 0.0; }

void
Oscillator::SetSampleRate(int rateIn)
{
	rate = rateIn;
	twopi_rate = m::twoPi / rate;
}

void
Oscillator::setPolarity(DspFloatType polarity)
{
	assert(polarity == 1.0 || polarity == -1.0);
	mPolarity = polarity;
}

void
Oscillator::ProcessSamples	(DspFloatType *buffer, int nFrames, DspFloatType freq_hz, DspFloatType pw, DspFloatType sync_freq)
{
	DspFloatType maxFreq = rate / 2.f;
	mFrequency.configure(mFrequency.getFinalValue(), std::min(freq_hz, maxFreq), nFrames);
	mPulseWidth = pw;
	mSyncFrequency = sync_freq;
	
	switch (waveform) {
	case Waveform::kSine:     doSine      (buffer, nFrames); break;
	case Waveform::kPulse:    doSquare    (buffer, nFrames); break;
	case Waveform::kSaw:      doSaw       (buffer, nFrames); break;
	case Waveform::kNoise:    doNoise     (buffer, nFrames); break;
	case Waveform::kRandom:   doRandom    (buffer, nFrames); break;
	default: assert(nullptr == "invalid waveform");
	}
}

void
Oscillator::doSine(DspFloatType *buffer, int nFrames)
{
    for (int i = 0; i < nFrames; i++) {
		DO_OSC_SYNC(rads);
		buffer[i] = sinf(rads += twopi_rate * mFrequency.nextValue());
	}
	rads = ffmodf(rads, m::twoPi);			// overflows are bad!
}

void 
Oscillator::doSquare(DspFloatType *buffer, int nFrames)
{
	const DspFloatType radsper = twopi_rate * mFrequency.getFinalValue();
	const DspFloatType pwscale = radsper < 0.3f ? 1.0f : 1.0f - ((radsper - 0.3f) / 2); assert(pwscale <= 1.0f); // reduces aliasing at high freq
	const DspFloatType pwrads = m::pi + pwscale * m::pi * std::min(mPulseWidth, 0.9f);

	DspFloatType lrads = rads;

    for (int i = 0; i < nFrames; i++) {
		DO_OSC_SYNC(lrads);

		DspFloatType radinc = twopi_rate * mFrequency.nextValue();
		DspFloatType nrads = lrads + radinc;
		DspFloatType y = 0.0f;

		//
		// aliasing is reduced by computing accurate values at crossing points (rather than always forcing -1.0 or 1.0.)
		// cpu performance is surprisingly good on x86 (better than saw or sine wave), probably due to its sophisticated branch prediction.
		//
		if (nrads >= m::twoPi) // transition from -1 --> 1
		{
			nrads -= m::twoPi;
			DspFloatType amt = nrads / radinc; assert(amt <= 1.001f);
			y = (2.0f * amt) - 1.0f;
		}
		else if (nrads <= pwrads)
		{
			y = 1.0f;
		}
		else if (lrads <= pwrads) // transition from 1 --> -1
		{
			DspFloatType amt = (nrads - pwrads) / radinc; assert(amt <= 1.001f);
			y = 1.0f - (2.0f * amt);
		}
		else
		{
			y = -1.0f;
		}

		buffer[i] = y;
		lrads = nrads; assert(lrads < m::twoPi);
	}
	rads = lrads;
}

static inline DspFloatType saw(DspFloatType rads, DspFloatType shape)
{
    rads = ffmodf(rads, m::twoPi);

    DspFloatType t = rads / m::twoPi;
    DspFloatType a = (shape + 1.0f) / 2.0f;

    if (t < a / 2)
		return 2 * t / a;

    if (t > (1 - (a / 2)))
		return (2 * t - 2) / a;

	return (1 - 2 * t) / (1 - a);
}

void 
Oscillator::doSaw(DspFloatType *buffer, int nFrames)
{
#ifdef ALIAS_REDUCTION
	// Clamp the maximum slope to reduce amount of aliasing in high octaves.
	// This is not proper anti-aliasing ;-)
	const DspFloatType requestedPW = mPulseWidth;
	const DspFloatType kAliasReductionAmount = 2.0f;
	const DspFloatType f = requestedPW - (kAliasReductionAmount * mFrequency.getFinalValue() / (DspFloatType)rate);
	if (mPulseWidth > f)
		mPulseWidth = f;
#endif

    for (int i = 0; i < nFrames; i++) {
		DO_OSC_SYNC(rads);
		buffer[i] = saw(rads += (twopi_rate * mFrequency.nextValue()), mPulseWidth) * mPolarity;
	}
    rads = ffmodf(rads, m::twoPi);

#ifdef ALIAS_REDUCTION
	mPulseWidth = requestedPW;
#endif
}

static const DspFloatType kTwoOverUlongMax = 2.0f / (DspFloatType)ULONG_MAX;

static inline DspFloatType randf()
{
	// Calculate pseudo-random 32 bit number based on linear congruential method.
	// http://www.musicdsp.org/showone.php?id=59
	static unsigned long random = 22222;
	random = (random * 196314165) + 907633515;
	return (DspFloatType)random * kTwoOverUlongMax - 1.0f;
}

void 
Oscillator::doRandom(DspFloatType *buffer, int nFrames)
{
    int period = (int) (rate / mFrequency.getFinalValue());
    for (int i = 0; i < nFrames; i++) {
	if (random_count > period) {
	    random_count = 0;
		random = randf();
	}
	random_count++;
	buffer[i] = random;
    }
}

void 
Oscillator::doNoise(DspFloatType *buffer, int nFrames)
{
    for (int i = 0; i < nFrames; i++)
		buffer[i] = randf();
}

#define BLEND(x0, x1, m) (((x0) * (1.f - (m))) + ((x1) * (m)))

// Low-pass filter the VCA control signal to prevent nasty clicking sounds
const DspFloatType kVCALowPassFreq = 4000.0f;

const DspFloatType kKeyTrackBaseFreq = 261.626f; // Middle C

enum class LFOWaveform {
	kSine,
	kSquare,
	kTriangle,
	kNoise,
	kRandomize,
	kSawtoothUp,
	kSawtoothDown
};

void
VoiceBoard::UpdateParameter	(Param param, DspFloatType value)
{
	switch (param)
	{
	case kAmsynthParameter_LFOToAmp:	mAmpModAmount = (value+1.0f)/2.0f;break;
	case kAmsynthParameter_LFOFreq:		mLFO1Freq = value; 		break;
	case kAmsynthParameter_LFOWaveform: {
		switch ((LFOWaveform)(int)value) {
			case LFOWaveform::kSine:         mLFOPulseWidth = 0.0; lfo1.SetWaveform(Oscillator::Waveform::kSine);   break;
			case LFOWaveform::kSquare:       mLFOPulseWidth = 0.0; lfo1.SetWaveform(Oscillator::Waveform::kPulse);  break;
			case LFOWaveform::kTriangle:     mLFOPulseWidth = 0.0; lfo1.SetWaveform(Oscillator::Waveform::kSaw);    break;
			case LFOWaveform::kNoise:        mLFOPulseWidth = 0.0; lfo1.SetWaveform(Oscillator::Waveform::kNoise);  break;
			case LFOWaveform::kRandomize:    mLFOPulseWidth = 0.0; lfo1.SetWaveform(Oscillator::Waveform::kRandom); break;
			case LFOWaveform::kSawtoothUp:   mLFOPulseWidth = 1.0; lfo1.SetWaveform(Oscillator::Waveform::kSaw);    lfo1.setPolarity(+1.0); break;
			case LFOWaveform::kSawtoothDown: mLFOPulseWidth = 1.0; lfo1.SetWaveform(Oscillator::Waveform::kSaw);    lfo1.setPolarity(-1.0); break;
			default: assert(nullptr == "invalid LFO waveform"); break;
		}
		break;
	}
	case kAmsynthParameter_LFOToOscillators:	mFreqModAmount=(value/2.0f)+0.5f;	break;
    case kAmsynthParameter_LFOOscillatorSelect: mFreqModDestination = (int)roundf(value); break;
	
	case kAmsynthParameter_Oscillator1Waveform:	osc1.SetWaveform ((Oscillator::Waveform) (int)value);
				break;
	case kAmsynthParameter_Oscillator1Pulsewidth:	mOsc1PulseWidth = value;	break;
	case kAmsynthParameter_Oscillator2Waveform:	osc2.SetWaveform ((Oscillator::Waveform) (int)value);
				break;
	case kAmsynthParameter_Oscillator2Pulsewidth:	mOsc2PulseWidth = value;	break;
	case kAmsynthParameter_Oscillator2Octave:	mOsc2Octave = value;		break;
	case kAmsynthParameter_Oscillator2Detune:	mOsc2Detune = value;		break;
	case kAmsynthParameter_Oscillator2Pitch:	mOsc2Pitch = ::powf(2, value / 12); break;
	case kAmsynthParameter_Oscillator2Sync:		mOsc2Sync  = roundf(value) != 0.f; break;

	case kAmsynthParameter_LFOToFilterCutoff:	mFilterModAmt = (value+1.0f)/2.0f;break;
	case kAmsynthParameter_FilterEnvAmount:	mFilterEnvAmt = value;		break;
	case kAmsynthParameter_FilterCutoff:	mFilterCutoff = value;		break;
	case kAmsynthParameter_FilterResonance:	mFilterRes = value;		break;
	case kAmsynthParameter_FilterEnvAttack:	mFilterADSR.SetAttack (value);	break;
	case kAmsynthParameter_FilterEnvDecay:	mFilterADSR.SetDecay (value);	break;
	case kAmsynthParameter_FilterEnvSustain:	mFilterADSR.SetSustain (value);	break;
	case kAmsynthParameter_FilterEnvRelease:	mFilterADSR.SetRelease (value);	break;
	case kAmsynthParameter_FilterType: mFilterType = (SynthFilter::Type) (int)value; break;
	case kAmsynthParameter_FilterSlope: mFilterSlope = (SynthFilter::Slope) (int)value; break;
	case kAmsynthParameter_FilterKeyTrackAmount: mFilterKbdTrack = value; break;
	case kAmsynthParameter_FilterKeyVelocityAmount: mFilterVelSens = value; break;

	case kAmsynthParameter_OscillatorMixRingMod:	mRingModAmt = value;		break;
	case kAmsynthParameter_OscillatorMix:			mOscMix = value;			break;
	
	case kAmsynthParameter_AmpEnvAttack:			mAmpADSR.SetAttack(value);	break;
	case kAmsynthParameter_AmpEnvDecay:				mAmpADSR.SetDecay(value);	break;
	case kAmsynthParameter_AmpEnvSustain:			mAmpADSR.SetSustain(value);	break;
	case kAmsynthParameter_AmpEnvRelease:			mAmpADSR.SetRelease(value);	break;
	case kAmsynthParameter_AmpVelocityAmount: mAmpVelSens = value; break;
		
	case kAmsynthParameter_MasterVolume:
	case kAmsynthParameter_ReverbRoomsize:
	case kAmsynthParameter_ReverbDamp:
	case kAmsynthParameter_ReverbWet:
	case kAmsynthParameter_ReverbWidth:
	case kAmsynthParameter_AmpDistortion:
	case kAmsynthParameter_PortamentoTime:
	case kAmsynthParameter_KeyboardMode:
	case kAmsynthParameter_PortamentoMode:
		break;
	case kAmsynthParameterCount:
	default:
		assert(nullptr == "Invalid parameter");
	}
}

void
VoiceBoard::SetPitchBend	(DspFloatType val)
{	
	mPitchBend = val;
}

void
VoiceBoard::ProcessSamplesMix	(DspFloatType *buffer, int numSamples, DspFloatType vol)
{
	assert(numSamples <= kMaxProcessBufferSize);

	if (mFrequencyDirty) {
		mFrequencyDirty = false;
		mFrequency.configure(mFrequencyStart, mFrequencyTarget, (int) (mFrequencyTime * mSampleRate));
	}

	//
	// Control Signals
	//
	DspFloatType *lfo1buf = mProcessBuffers.lfo_osc_1;
	lfo1.ProcessSamples (lfo1buf, numSamples, mLFO1Freq, mLFOPulseWidth);

	const DspFloatType frequency = mFrequency.nextValue();
	for (int i=1; i<numSamples; i++) { mFrequency.nextValue(); }

	DspFloatType baseFreq = mPitchBend * frequency;

	DspFloatType osc1freq = baseFreq;
	if (mFreqModDestination == 0 || mFreqModDestination == 1) {
		osc1freq = osc1freq * ( mFreqModAmount * (lfo1buf[0] + 1.0f) + 1.0f - mFreqModAmount );
	}
	DspFloatType osc1pw = mOsc1PulseWidth;

	DspFloatType osc2freq = baseFreq * mOsc2Detune * mOsc2Octave * mOsc2Pitch;
	if (mFreqModDestination == 0 || mFreqModDestination == 2) {
		osc2freq = osc2freq * ( mFreqModAmount * (lfo1buf[0] + 1.0f) + 1.0f - mFreqModAmount );
	}
	DspFloatType osc2pw = mOsc2PulseWidth;

	mFilterADSR.process(mProcessBuffers.filter_env, numSamples);
	DspFloatType env_f = mProcessBuffers.filter_env[numSamples - 1];
	DspFloatType cutoff_base = BLEND(kKeyTrackBaseFreq, frequency, mFilterKbdTrack);
	DspFloatType cutoff_vel_mult = BLEND(1.f, mKeyVelocity, mFilterVelSens);
	DspFloatType cutoff_lfo_mult = (lfo1buf[0] * 0.5f + 0.5f) * mFilterModAmt + 1 - mFilterModAmt;
	DspFloatType cutoff = mFilterCutoff * cutoff_base * cutoff_vel_mult * cutoff_lfo_mult;
	if (mFilterEnvAmt > 0.f) cutoff += (frequency * env_f * mFilterEnvAmt);
	else
	{
		static const DspFloatType r16 = 1.f/16.f; // scale if from -16 to -1
		cutoff += cutoff * r16 * mFilterEnvAmt * env_f;
	}
	

	//
	// VCOs
	//
	DspFloatType *osc1buf = mProcessBuffers.osc_1;
	DspFloatType *osc2buf = mProcessBuffers.osc_2;

	bool osc2sync = mOsc2Sync;
	// previous implementation of sync had a bug causing it to only work when osc1 was set to sine or saw
	// we need to recreate that behaviour here to ensure old presets still sound the same.
	osc2sync &= (osc1.GetWaveform() == Oscillator::Waveform::kSine || osc1.GetWaveform() == Oscillator::Waveform::kSaw);
	osc2.setSyncEnabled(osc2sync);

	osc1.ProcessSamples (osc1buf, numSamples, osc1freq, osc1pw);
	osc2.ProcessSamples (osc2buf, numSamples, osc2freq, osc2pw, osc1freq);

	//
	// Osc Mix
	//
	for (int i=0; i<numSamples; i++) {
		DspFloatType ringMod = mRingModAmt.tick();
		DspFloatType oscMix = mOscMix.tick();
		DspFloatType osc1vol = (1.F - ringMod) * (1.F - oscMix) / 2.F;
		DspFloatType osc2vol = (1.F - ringMod) * (1.F + oscMix) / 2.F;
		osc1buf[i] =
			osc1vol * osc1buf[i] +
			osc2vol * osc2buf[i] +
			ringMod * osc1buf[i] * osc2buf[i];
	}

	//
	// VCF
	//
	filter.ProcessSamples (osc1buf, numSamples, cutoff, mFilterRes, mFilterType, mFilterSlope);
	
	//
	// VCA
	// 
	DspFloatType *ampenvbuf = mProcessBuffers.amp_env;
	mAmpADSR.process(ampenvbuf, numSamples);
	for (int i=0; i<numSamples; i++) {
		DspFloatType ampModAmount = mAmpModAmount.tick();
		const DspFloatType amplitude = ampenvbuf[i] * BLEND(1.f, mKeyVelocity, mAmpVelSens.tick()) *
			( ((lfo1buf[i] * 0.5f) + 0.5f) * ampModAmount + 1 - ampModAmount);
		buffer[i] += osc1buf[i] * _vcaFilter.processSample(amplitude * mVolume.processSample(vol));
	}
}

void
VoiceBoard::SetSampleRate	(int rate)
{
	mSampleRate = rate;
	lfo1.SetSampleRate (rate);
	osc1.SetSampleRate (rate);
	osc2.SetSampleRate (rate);
	filter.SetSampleRate (rate);
	mFilterADSR.SetSampleRate(rate);
	mAmpADSR.SetSampleRate(rate);
	_vcaFilter.setCoefficients(rate, kVCALowPassFreq, IIRFilterFirstOrder::Mode::kLowPass);
}

bool 
VoiceBoard::isSilent()
{
	return mAmpADSR.getState() == 0 && _vcaFilter._z < 0.0000001;
}

void 
VoiceBoard::triggerOn(bool reset)
{
	if (reset) {
		mOscMix.reset();
		mRingModAmt.reset();
		mAmpModAmount.reset();
		mAmpVelSens.reset();
	}
	mAmpADSR.triggerOn();
	mFilterADSR.triggerOn();
}

void 
VoiceBoard::triggerOff()
{
	mAmpADSR.triggerOff();
	mFilterADSR.triggerOff();
}

void
VoiceBoard::reset()
{
	mAmpADSR.reset();
	mFilterADSR.reset();
	osc1.reset();
	osc2.reset();
	filter.reset();
	lfo1.reset();
}

void
VoiceBoard::setFrequency(DspFloatType startFrequency, DspFloatType targetFrequency, DspFloatType time)
{
	mFrequencyStart = startFrequency;
	mFrequencyTarget = targetFrequency;
	mFrequencyTime = time;
	mFrequencyDirty = true;
}

void
VoiceBoard::setVelocity(DspFloatType velocity)
{
	assert(velocity <= 1.0f);
	mKeyVelocity = velocity;
}

