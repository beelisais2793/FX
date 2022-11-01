#pragma once


#include <cmath>
#include <array>
#include <vector>
#include <cstdint>
#include <exception>
#include <array>
#include <random>
#include <cassert>
#include <algorithm>
#include <cstring>


#include "Undenormal.hpp"
#include "FX/ClipFunctions.hpp"

#define MOOG_E         2.71828182845904523536028747135266250
#define MOOG_LOG2E     1.44269504088896340735992468100189214
#define MOOG_LOG10E    0.434294481903251827651128918916605082
#define MOOG_LN2       0.693147180559945309417232121458176568
#define MOOG_LN10      2.30258509299404568401799145468436421
#define MOOG_PI        3.14159265358979323846264338327950288
#define MOOG_PI_2      1.57079632679489661923132169163975144
#define MOOG_PI_4      0.785398163397448309615660845819875721
#define MOOG_1_PI      0.318309886183790671537767526745028724
#define MOOG_2_PI      0.636619772367581343075535053490057448
#define MOOG_2_SQRTPI  1.12837916709551257389615890312154517
#define MOOG_SQRT2     1.41421356237309504880168872420969808
#define MOOG_SQRT1_2   0.707106781186547524400844362104849039
#define MOOG_INV_PI_2  0.159154943091895

#define NO_COPY(C) C(const C &) = delete; C & operator = (const C &) = delete
#define NO_MOVE(C) NO_COPY(C); C(C &&) = delete; C & operator = (const C &&) = delete

#define SNAP_TO_ZERO(n)    if (! (n < -1.0e-8 || n > 1.0e-8)) n = 0;

namespace Analog::MoogFilters
{
// Linear interpolation, used to crossfade a gain table
inline DspFloatType moog_lerp(DspFloatType amount, DspFloatType a, DspFloatType b)
{
	return (1.0f - amount) * a + amount * b;
}

inline DspFloatType moog_min(DspFloatType a, DspFloatType b)
{
	a = b - a;
	a += fabs(a);
	a *= 0.5f;
	a = b - a;
	return a;
}

// Clamp without branching
// If input - _limit < 0, then it really substracts, and the 0.5 to make it half the 2 inputs.
// If > 0 then they just cancel, and keeps input normal.
// The easiest way to understand it is check what happends on both cases.
inline DspFloatType moog_saturate(DspFloatType input)
{
	DspFloatType x1 = fabs(input + 0.95f);
	DspFloatType x2 = fabs(input - 0.95f);
	return 0.5f * (x1 - x2);
}

// Imitate the (tanh) clipping function of a transistor pair.
// to 4th order, tanh is x - x*x*x/3; this cubic's
// plateaus are at +/- 1 so clip to 1 and evaluate the cubic.
// This is pretty coarse - for instance if you clip a sinusoid this way you
// can sometimes hear the discontinuity in 4th derivative at the clip point
inline DspFloatType clip(DspFloatType value, DspFloatType saturation, DspFloatType saturationinverse)
{
	DspFloatType v2 = (value * saturationinverse > 1 ? 1 :
				(value * saturationinverse < -1 ? -1:
				 value * saturationinverse));
	return (saturation * (v2 - (1./3.) * v2 * v2 * v2));
}

#define HZ_TO_RAD(f) (MOOG_PI_2 * f)
#define RAD_TO_HZ(omega) (MOOG_INV_PI_2 * omega)

#ifdef __GNUC__
	#define ctz(N) __builtin_ctz(N)
#else
	template<typename T>
	inline int ctz(T x)
	{
		int p, b;
		for (p = 0, b = 1; !(b & x); b <<= 1, ++p)
			;
		return p;
	}
#endif

inline DspFloatType fast_tanh(DspFloatType x) 
{
	DspFloatType x2 = x * x;
	return x * (27.0 + x2) / (27.0 + 9.0 * x2);
}

class BiQuadBase
{
public:
	
	BiQuadBase()
	{
		bCoef = {{0.0f, 0.0f, 0.0f}};
		aCoef = {{0.0f, 0.0f}};
		w = {{0.0f, 0.0f}};
	}	
	~BiQuadBase()
	{

	}	
	// DF-II impl	
	void Process(uint32_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		DspFloatType out = 0;
		for (int s = 0; s < n; ++s)
		{
			out = bCoef[0] * samples[s] + w[0];
			w[0] = bCoef[1] * samples[s] - aCoef[0] * out + w[1];
			w[1] = bCoef[2] * samples[s] - aCoef[1] * out;
			samples[s] = out;
		}
	}
	DspFloatType Tick(DspFloatType s)
	{
        Undenormal denormal;
		DspFloatType out = bCoef[0] * s + w[0];
		w[0] = bCoef[1] * s - aCoef[0] * out + w[1];
		w[1] = bCoef[2] * s - aCoef[1] * out;
		return out;
	}
	void SetBiquadCoefs(std::array<DspFloatType, 3> b, std::array<DspFloatType, 2> a)
	{
		bCoef = b;
		aCoef = a;
	}
	
protected:
	std::array<DspFloatType, 3> bCoef; // b0, b1, b2
	std::array<DspFloatType, 2> aCoef; // a1, a2
	std::array<DspFloatType, 2> w; // delays
};


/*! \brief implement a circular buffer of type T
*/
template <class T>
class CRingBuffer
{
public:
    explicit CRingBuffer(int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iReadIdx(0),
        m_iWriteIdx(0),
        m_ptBuff(0)
    {
        assert(iBufferLengthInSamples > 0);

        m_ptBuff = new T[m_iBuffLength];
        reset();
    }

    virtual ~CRingBuffer()
    {
        delete[] m_ptBuff;
        m_ptBuff = 0;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc(T tNewValue)
    {
        put(tNewValue);
        incIdx(m_iWriteIdx);
    }

    /*! add new values of type T to write index and increment write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void putPostInc(const T* ptNewBuff, int iLength)
    {
        put(ptNewBuff, iLength);
        incIdx(m_iWriteIdx, iLength);
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        m_ptBuff[m_iWriteIdx] = tNewValue;
    }

    /*! add new values of type T to write index
    \param ptNewBuff: new values
    \param iLength: number of values
    \return void
    */
    void put(const T* ptNewBuff, int iLength)
    {
        assert(iLength <= m_iBuffLength && iLength >= 0);

        // copy two parts: to the end of buffer and after wrap around
        int iNumValues2End = std::min(iLength, m_iBuffLength - m_iWriteIdx);

        std::memcpy (&m_ptBuff[m_iWriteIdx], ptNewBuff, sizeof(T)*iNumValues2End);
        if ((iLength - iNumValues2End) > 0)
            std::memcpy (m_ptBuff, &ptNewBuff[iNumValues2End], sizeof(T)*(iLength - iNumValues2End));
    }

    /*! return the value at the current read index and increment the read pointer
    \return DspFloatType the value from the read index
    */
    T getPostInc(DspFloatType fOffset = 0)
    {
        T tValue = get(fOffset);
        incIdx(m_iReadIdx);
        return tValue;
    }

    /*! return the values starting at the current read index and increment the read pointer
    \param ptBuff: pointer to where the values will be written
    \param iLength: number of values
    \return void
    */
    void getPostInc(T* ptBuff, int iLength)
    {
        get(ptBuff, iLength);
        incIdx(m_iReadIdx, iLength);
    }

    /*! return the value at the current read index
    \param fOffset: read at offset from read index
    \return DspFloatType the value from the read index
    */
    T get(DspFloatType fOffset = 0) const
    {
        if (fOffset == 0)
            return m_ptBuff[m_iReadIdx];
        else
        {

            // compute fraction for linear interpolation 
            int     iOffset = static_cast<int>(std::floor(fOffset));
            DspFloatType   fFrac = fOffset - iOffset;
            int     iRead = m_iReadIdx + iOffset;
            while (iRead > m_iBuffLength - 1)
                iRead -= m_iBuffLength;
            while (iRead < 0)
                iRead += m_iBuffLength;

            return (1 - fFrac) * m_ptBuff[iRead] +
                fFrac * m_ptBuff[(iRead + 1) % m_iBuffLength];
        }
    }

    /*! return the values starting at the current read index
    \param ptBuff to where the values will be written
    \param iLength: number of values
    \return void
    */
    void get(T* ptBuff, int iLength) const
    {
        assert(iLength <= m_iBuffLength && iLength >= 0);

        // copy two parts: to the end of buffer and after wrap around
        int iNumValues2End = std::min(iLength, m_iBuffLength - m_iReadIdx);

        std::memcpy (ptBuff, &m_ptBuff[m_iReadIdx], sizeof(T)*iNumValues2End);
        if ((iLength - iNumValues2End)>0)
            std::memcpy (&ptBuff[iNumValues2End], m_ptBuff, sizeof(T)*(iLength - iNumValues2End));
    }

    T extractPostInc()
    {
        T value = get();
        m_ptBuff[m_iReadIdx] = 0;
        incIdx(m_iReadIdx);
        return value;
    }

    /*! set buffer content and indices to 0
    \return void
    */
    void reset()
    {
        std::memset (m_ptBuff, 0, sizeof(T)*m_iBuffLength);
        m_iReadIdx  = 0;
        m_iWriteIdx = 0;
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx() const
    {
        return m_iWriteIdx;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx(int iNewWriteIdx)
    {
        incIdx(m_iWriteIdx, iNewWriteIdx - m_iWriteIdx);
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx() const
    {
        return m_iReadIdx;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx(int iNewReadIdx)
    {
        incIdx(m_iReadIdx, iNewReadIdx - m_iReadIdx);
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer() const
    {
        return (m_iWriteIdx - m_iReadIdx + m_iBuffLength) % m_iBuffLength;
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength() const
    {
        return m_iBuffLength;
    }
private:
    CRingBuffer();
    CRingBuffer(const CRingBuffer& that);

    void incIdx(int& iIdx, int iOffset = 1)
    {
        while ((iIdx + iOffset) < 0)
        {
            // avoid negative buffer indices
            iOffset += m_iBuffLength;
        }
        iIdx = (iIdx + iOffset) % m_iBuffLength;
    };

    int m_iBuffLength = 0,      //!< length of the internal buffer
        m_iReadIdx = 0,         //!< current read index
        m_iWriteIdx = 0;        //!< current write index

    T* m_ptBuff = 0;            //!< data buffer
};

///////////////////////////////////////////////////////////////////////////////////////////
// Randolf Biloninan Jackson Filter
///////////////////////////////////////////////////////////////////////////////////////////
class RBJFilter : public BiQuadBase
{
public:
	
	enum FilterType
	{
		LOWPASS,
		HIGHPASS,
		BANDPASS,
		ALLPASS,
		NOTCH,
		PEAK,
		LOW_SHELF,
		HIGH_SHELF
	};
	
	
	RBJFilter(FilterType type = FilterType::LOWPASS, DspFloatType cutoff = 1, DspFloatType sampleRate = 44100) : sampleRate(sampleRate), t(type)
	{
		Q = 1;
		A = 1;

		a = {{0.0f, 0.0f, 0.0f}};
		b = {{0.0f, 0.0f, 0.0f}};

		SetCutoff(cutoff);
	}
	
	~RBJFilter()
	{
		
	}
	
	void UpdateCoefficients()
	{
		cosOmega = cos(omega);
		sinOmega = sin(omega);
		
		switch (t)
		{
			case LOWPASS:
			{
				alpha = sinOmega / (2.0 * Q);
				b[0] = (1 - cosOmega) / 2;
				b[1] = 1 - cosOmega;
				b[2] = b[0];
				a[0] = 1 + alpha;
				a[1] = -2 * cosOmega;
				a[2] = 1 - alpha;
			} break;
				
			case HIGHPASS:
			{
				alpha = sinOmega / (2.0 * Q);
				b[0] = (1 + cosOmega) / 2;
				b[1] = -(1 + cosOmega);
				b[2] = b[0];
				a[0] = 1 + alpha;
				a[1] = -2 * cosOmega;
				a[2] = 1 - alpha;
			} break;
				
			case BANDPASS:
			{
				alpha = sinOmega * sinhf(logf(2.0) / 2.0 * Q * omega/sinOmega);
				b[0] = sinOmega / 2;
				b[1] = 0;
				b[2] = -b[0];
				a[0] = 1 + alpha;
				a[1] = -2 * cosOmega;
				a[2] = 1 - alpha;
			} break;
				
			case ALLPASS:
			{
				alpha = sinOmega / (2.0 * Q);
				b[0] = 1 - alpha;
				b[1] = -2 * cosOmega;
				b[2] = 1 + alpha;
				a[0] = b[2];
				a[1] = b[1];
				a[2] = b[0];
			} break;
				
			case NOTCH:
			{
				alpha = sinOmega * sinhf(logf(2.0) / 2.0 * Q * omega/sinOmega);
				b[0] = 1;
				b[1] = -2 * cosOmega;
				b[2] = 1;
				a[0] = 1 + alpha;
				a[1] = b[1];
				a[2] = 1 - alpha;
			} break;
				
			case PEAK:
			{
				alpha = sinOmega * sinhf(logf(2.0) / 2.0 * Q * omega/sinOmega);
				b[0] = 1 + (alpha * A);
				b[1] = -2 * cosOmega;
				b[2] = 1 - (alpha * A);
				a[0] = 1 + (alpha / A);
				a[1] = b[1];
				a[2] = 1 - (alpha / A);
			} break;
				
			case LOW_SHELF:
			{
				alpha = sinOmega / 2.0 * sqrt( (A + 1.0 / A) * (1.0 / Q - 1.0) + 2.0);
				b[0] = A * ((A + 1) - ((A - 1) * cosOmega) + (2 * sqrtf(A) * alpha));
				b[1] = 2 * A * ((A - 1) - ((A + 1) * cosOmega));
				b[2] = A * ((A + 1) - ((A - 1) * cosOmega) - (2 * sqrtf(A) * alpha));
				a[0] = ((A + 1) + ((A - 1) * cosOmega) + (2 * sqrtf(A) * alpha));
				a[1] = -2 * ((A - 1) + ((A + 1) * cosOmega));
				a[2] = ((A + 1) + ((A - 1) * cosOmega) - (2 * sqrtf(A) * alpha));
			} break;
				
			case HIGH_SHELF:
			{
				alpha = sinOmega / 2.0 * sqrt( (A + 1.0 / A) * (1.0 / Q - 1.0) + 2.0);
				b[0] = A * ((A + 1) + ((A - 1) * cosOmega) + (2 * sqrtf(A) * alpha));
				b[1] = -2 * A * ((A - 1) + ((A + 1) * cosOmega));
				b[2] = A * ((A + 1) + ((A - 1) * cosOmega) - (2 * sqrtf(A) * alpha));
				a[0] = ((A + 1) - ((A - 1) * cosOmega) + (2 * sqrtf(A) * alpha));
				a[1] = 2 * ((A - 1) - ((A + 1) * cosOmega));
				a[2] = ((A + 1) - ((A - 1) * cosOmega) - (2 * sqrtf(A) * alpha));
			} break;
		}
		
		// Normalize filter coefficients
		DspFloatType factor = 1.0f / a[0];
		
		std::array<DspFloatType, 2> aNorm;
		std::array<DspFloatType, 3> bNorm;

		aNorm[0] = a[1] * factor;
		aNorm[1] = a[2] * factor;

		bNorm[0] = b[0] * factor;
		bNorm[1] = b[1] * factor;
		bNorm[2] = b[2] * factor;
		
		SetBiquadCoefs(bNorm, aNorm);
	}
	
	void SetSampleRate(DspFloatType sr)
	{
		sampleRate = sr;
	}

	// In Hertz, 0 to Nyquist
	void SetCutoff(DspFloatType c)
	{
		omega = HZ_TO_RAD(c) / sampleRate;
		UpdateCoefficients();
	}
	
	DspFloatType GetCutoff()
	{
		return omega;
	}
	
	// Arbitrary, from 0.01f to ~20
	void SetQValue(DspFloatType q)
	{
		Q = q;
		UpdateCoefficients();
	}
	
	DspFloatType GetQValue()
	{
		return Q;
	}

	void SetType(FilterType newType)
	{
		t = newType;
		UpdateCoefficients();
	}
	
	FilterType GetType()
	{
		return t;
	}
	
private:

	DspFloatType sampleRate;
	
	DspFloatType omega;
	DspFloatType cosOmega;
	DspFloatType sinOmega;
	
	DspFloatType Q;
	DspFloatType alpha;
	DspFloatType A;

	std::array<DspFloatType, 3> a;
	std::array<DspFloatType, 3> b;
	
	FilterType t;
};


///////////////////////////////////////////////////////////////////////////////////////////
// Noise kaka
///////////////////////////////////////////////////////////////////////////////////////////
// +/-0.05dB above 9.2Hz @ 44,100Hz
class PinkingFilter
{
	DspFloatType b0, b1, b2, b3, b4, b5, b6;
public:
	PinkingFilter() : b0(0), b1(0), b2(0), b3(0), b4(0), b5(0), b6(0) {}
	DspFloatType process(const DspFloatType s)
	{
        Undenormal denormal;
		b0 = 0.99886 * b0 + s * 0.0555179;
		b1 = 0.99332 * b1 + s * 0.0750759;
		b2 = 0.96900 * b2 + s * 0.1538520;
		b3 = 0.86650 * b3 + s * 0.3104856;
		b4 = 0.55000 * b4 + s * 0.5329522;
		b5 = -0.7616 * b5 - s * 0.0168980;
		const DspFloatType pink = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + (s * 0.5362)) * 0.11;
		b6 = s * 0.115926;
		return pink;
	}
};

class BrowningFilter
{
DspFloatType l;
public:
	BrowningFilter() : l(0) {}
	DspFloatType process(const DspFloatType s)
	{
        Undenormal denormal;
		DspFloatType brown = (l + (0.02f * s)) / 1.02f;
		l = brown;
		return brown * 3.5f; // compensate for gain
	}
};

struct WhiteNoiseSource
{
	WhiteNoiseSource() : dist(-1, 1) {}
	std::mt19937 engine;
	std::uniform_real_distribution<DspFloatType> dist;
};

// Full spectrum noise
struct WhiteNoise : public WhiteNoiseSource
{
	DspFloatType operator()() { return dist(engine); }
};

// Pink noise has a decrease of 3dB/Octave
struct PinkNoise : public WhiteNoiseSource
{
	DspFloatType operator()() { return f.process(dist(engine)); }
	PinkingFilter f;
};

 // Brown noise has a decrease of 6dB/Octave
struct BrownNoise : public WhiteNoiseSource
{
	DspFloatType operator()() { return f.process(dist(engine)); }
	BrowningFilter f;
};

// Note! This noise is only valid for 44100 because of the hard-coded filter coefficients
struct NoiseGenerator
{
	enum NoiseType
	{
		WHITE,
		PINK,
		BROWN,
	} noise_type = PINK;
	
	std::vector<DspFloatType> produce(NoiseType t, int sampleRate, int channels, DspFloatType seconds)
	{
		int samplesToGenerate = sampleRate * seconds * channels;
		std::vector<DspFloatType> samples;
		samples.resize(samplesToGenerate);
		
		switch (t)
		{
		case NoiseType::WHITE:
		{
			WhiteNoise n;
			for(int s = 0; s < samplesToGenerate; s++) samples[s] = n();
		} break;
		case NoiseType::PINK:
		{
			PinkNoise n;
			for(int s = 0; s < samplesToGenerate; s++) samples[s] = n();
		} break;
		case NoiseType::BROWN:
		{
			BrownNoise n;
			for(int s = 0; s < samplesToGenerate; s++) samples[s] = n();
		} break;
		default: throw std::runtime_error("Invalid noise type");
		}
		return samples;
	}
	
	DspFloatType Tick() {
		switch (noise_type)
		{
		case NoiseType::WHITE:
		{
			WhiteNoise n;
			return n();
		} break;
		case NoiseType::PINK:
		{
			PinkNoise n;
			return n();
		} break;
		case NoiseType::BROWN:
		{
			BrownNoise n;
			return n();
		} break;
		default: throw std::runtime_error("Invalid noise type");
		}
	}
	void Process(size_t n, DspFloatType * input, DspFloatType * output) {
		for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
	}
	void Process(DspFloatType * samples,size_t n) {
		for(size_t i = 0; i < n; i++) samples[i] = samples[i]*Tick();
	}
};


///////////////////////////////////////////////////////////////////////////////////////////
// Moog Ladder
///////////////////////////////////////////////////////////////////////////////////////////
class LadderFilterBase
{
public:

	LadderFilterBase(DspFloatType sampleRate) : sampleRate(sampleRate) {}
	virtual ~LadderFilterBase() {}

	virtual void Process(size_t n,DspFloatType * samples) = 0;	
	virtual void SetResonance(DspFloatType r) = 0;
	virtual void SetCutoff(DspFloatType c) = 0;

	DspFloatType GetResonance() { return resonance; }
	DspFloatType GetCutoff() { return cutoff; }

protected:

	DspFloatType cutoff;
	DspFloatType resonance;
	DspFloatType sampleRate;
};


///////////////////////////////////////////////////////////////////////////////////////////
// Finnish Vampire
///////////////////////////////////////////////////////////////////////////////////////////
class HuovilainenMoog : public LadderFilterBase
{
public:

	HuovilainenMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate), thermal(0.000025)
	{
		memset(stage, 0, sizeof(stage));
		memset(delay, 0, sizeof(delay));
		memset(stageTanh, 0, sizeof(stageTanh));
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~HuovilainenMoog()
	{

	}

	void Process(uint32_t n, DspFloatType * _input, DspFloatType * _output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Oversample
			for (int j = 0; j < 2; j++)
			{
				DspFloatType input = _input[s] - resQuad * delay[5];
				delay[0] = stage[0] = delay[0] + tune * (tanh(input * thermal) - stageTanh[0]);
				for (int k = 1; k < 4; k++)
				{
					input = stage[k-1];
					stage[k] = delay[k] + tune * ((stageTanh[k-1] = tanh(input * thermal)) - (k != 3 ? stageTanh[k] : tanh(delay[k] * thermal)));
					delay[k] = stage[k];
				}
				// 0.5 sample delay for phase compensation
				delay[5] = (stage[3] + delay[4]) * 0.5;
				delay[4] = stage[3];
			}
			_output[s] = delay[5];
		}
	}

	void Process(size_t n, DspFloatType * _input)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Oversample
			for (int j = 0; j < 2; j++)
			{
				DspFloatType input = _input[s] - resQuad * delay[5];
				delay[0] = stage[0] = delay[0] + tune * (tanh(input * thermal) - stageTanh[0]);
				for (int k = 1; k < 4; k++)
				{
					input = stage[k-1];
					stage[k] = delay[k] + tune * ((stageTanh[k-1] = tanh(input * thermal)) - (k != 3 ? stageTanh[k] : tanh(delay[k] * thermal)));
					delay[k] = stage[k];
				}
				// 0.5 sample delay for phase compensation
				delay[5] = (stage[3] + delay[4]) * 0.5;
				delay[4] = stage[3];
			}
			_input[s] = delay[5];
		}
	}

	
	DspFloatType Tick(DspFloatType input) {
		DspFloatType r = 0.0;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(DspFloatType r) override
	{
		resonance = r;
		resQuad = 4.0 * resonance * acr;
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c;

		DspFloatType fc =  cutoff / sampleRate;
		DspFloatType f  =  fc * 0.5; // oversampled
		DspFloatType fc2 = fc * fc;
		DspFloatType fc3 = fc * fc * fc;

		DspFloatType fcr = 1.8730 * fc3 + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
		acr = -3.9364 * fc2 + 1.8409 * fc + 0.9968;

		tune = (1.0 - exp(-((2 * MOOG_PI) * f * fcr))) / thermal;

		SetResonance(resonance);
	}

private:

	DspFloatType stage[4];
	DspFloatType stageTanh[3];
	DspFloatType delay[6];

	DspFloatType thermal;
	DspFloatType tune;
	DspFloatType acr;
	DspFloatType resQuad;

};


///////////////////////////////////////////////////////////////////////////////////////////
// The new and improved moog filter
///////////////////////////////////////////////////////////////////////////////////////////
const DspFloatType VT = 0.312;

class ImprovedMoog : public LadderFilterBase
{
public:

	ImprovedMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(V, 0, sizeof(V));
		memset(dV, 0, sizeof(dV));
		memset(tV, 0, sizeof(tV));

		drive = 1.0f;

		SetCutoff(1000.0f); // normalized cutoff frequency
		SetResonance(0.1f); // [0, 4]
	}

	virtual ~ImprovedMoog() { }

	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		DspFloatType dV0, dV1, dV2, dV3;

		for (int i = 0; i < n; i++)
		{
			dV0 = -g * (tanh((drive * samples[i] + resonance * V[3]) / (2.0 * VT)) + tV[0]);
			V[0] += (dV0 + dV[0]) / (2.0 * sampleRate);
			dV[0] = dV0;
			tV[0] = tanh(V[0] / (2.0 * VT));

			dV1 = g * (tV[0] - tV[1]);
			V[1] += (dV1 + dV[1]) / (2.0 * sampleRate);
			dV[1] = dV1;
			tV[1] = tanh(V[1] / (2.0 * VT));

			dV2 = g * (tV[1] - tV[2]);
			V[2] += (dV2 + dV[2]) / (2.0 * sampleRate);
			dV[2] = dV2;
			tV[2] = tanh(V[2] / (2.0 * VT));

			dV3 = g * (tV[2] - tV[3]);
			V[3] += (dV3 + dV[3]) / (2.0 * sampleRate);
			dV[3] = dV3;
			tV[3] = tanh(V[3] / (2.0 * VT));

			samples[i] = V[3];
		}
	}

	void Process(uint32_t n,DspFloatType * samples, DspFloatType * output)
	{
        Undenormal denormal;
		DspFloatType dV0, dV1, dV2, dV3;

		for (uint32_t i = 0; i < n; i++)
		{
			dV0 = -g * (tanh((drive * samples[i] + resonance * V[3]) / (2.0 * VT)) + tV[0]);
			V[0] += (dV0 + dV[0]) / (2.0 * sampleRate);
			dV[0] = dV0;
			tV[0] = tanh(V[0] / (2.0 * VT));

			dV1 = g * (tV[0] - tV[1]);
			V[1] += (dV1 + dV[1]) / (2.0 * sampleRate);
			dV[1] = dV1;
			tV[1] = tanh(V[1] / (2.0 * VT));

			dV2 = g * (tV[1] - tV[2]);
			V[2] += (dV2 + dV[2]) / (2.0 * sampleRate);
			dV[2] = dV2;
			tV[2] = tanh(V[2] / (2.0 * VT));

			dV3 = g * (tV[2] - tV[3]);
			V[3] += (dV3 + dV[3]) / (2.0 * sampleRate);
			dV[3] = dV3;
			tV[3] = tanh(V[3] / (2.0 * VT));

			output[i] = V[3];
		}
	}

	
	virtual void SetResonance(DspFloatType r) override
	{
		resonance = r;
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c;
		x = (MOOG_PI * cutoff) / sampleRate;
		g = 4.0 * MOOG_PI * VT * cutoff * (1.0 - x) / (1.0 + x);
	}

	DspFloatType Tick(DspFloatType input) {
		DspFloatType r = input;
		Process(1,&input,&r);
		return r;
	}
private:

	DspFloatType V[4];
	DspFloatType dV[4];
	DspFloatType tV[4];

	DspFloatType x;
	DspFloatType g;
	DspFloatType drive;
};


///////////////////////////////////////////////////////////////////////////////////////////
// The Krajtski 5
///////////////////////////////////////////////////////////////////////////////////////////
class KrajeskiMoog final : public LadderFilterBase
{

public:

    KrajeskiMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(state, 0, sizeof(state));
		memset(delay, 0, sizeof(delay));

		drive = 1.0;
		gComp = 1.0;

		SetCutoff(1000.0f);
		SetResonance(0.1f);
	}

	virtual ~KrajeskiMoog() { }

	void Process(size_t n, DspFloatType * samples, DspFloatType * output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			state[0] = tanh(drive * (samples[s] - 4 * gRes * (state[4] - gComp * samples[s])));

			for(int i = 0; i < 4; i++)
			{
				state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
				delay[i] = state[i];
			}
			output[s] = state[4];
		}
	}

	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			state[0] = tanh(drive * (samples[s] - 4 * gRes * (state[4] - gComp * samples[s])));

			for(int i = 0; i < 4; i++)
			{
				state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
				delay[i] = state[i];
			}
			samples[s] = state[4];
		}
	}
	
	DspFloatType Tick(DspFloatType input) {
		DspFloatType r = 0.0;
		Process(1,&input,&r);
		return r;

	}
	virtual void SetResonance(DspFloatType r) override
	{
		resonance = r;
		gRes = resonance * (1.0029 + 0.0526 * wc - 0.926 * pow(wc, 2) + 0.0218 * pow(wc, 3));
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c;
		wc = 2 * MOOG_PI * cutoff / sampleRate;
		g = 0.9892 * wc - 0.4342 * pow(wc, 2) + 0.1381 * pow(wc, 3) - 0.0202 * pow(wc, 4);
	}

private:

	DspFloatType state[5];
	DspFloatType delay[5];
	DspFloatType wc; // The angular frequency of the cutoff.
	DspFloatType g; // A derived parameter for the cutoff frequency
	DspFloatType gRes; // A similar derived parameter for resonance.
	DspFloatType gComp; // Compensation factor.
	DspFloatType drive; // A parameter that controls intensity of nonlinearities.

};


///////////////////////////////////////////////////////////////////////////////////////////
// Stolen from microtracker
///////////////////////////////////////////////////////////////////////////////////////////
class MicrotrackerMoog : public LadderFilterBase
{

public:

	MicrotrackerMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		p0 = p1 = p2 = p3 = p32 = p33 = p34 = 0.0;
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~MicrotrackerMoog() {}

	void Process(size_t n, DspFloatType * samples, DspFloatType * output)
	{
        Undenormal denormal;
		DspFloatType k = resonance * 4;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			DspFloatType out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

			p34 = p33;
			p33 = p32;
			p32 = p3;

			p0 += (fast_tanh(samples[s] - k * out) - fast_tanh(p0)) * cutoff;
			p1 += (fast_tanh(p0) - fast_tanh(p1)) * cutoff;
			p2 += (fast_tanh(p1) - fast_tanh(p2)) * cutoff;
			p3 += (fast_tanh(p2) - fast_tanh(p3)) * cutoff;

			output[s] = out;
		}
	}

	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		DspFloatType k = resonance * 4;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			DspFloatType out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

			p34 = p33;
			p33 = p32;
			p32 = p3;

			p0 += (fast_tanh(samples[s] - k * out) - fast_tanh(p0)) * cutoff;
			p1 += (fast_tanh(p0) - fast_tanh(p1)) * cutoff;
			p2 += (fast_tanh(p1) - fast_tanh(p2)) * cutoff;
			p3 += (fast_tanh(p2) - fast_tanh(p3)) * cutoff;

			samples[s] = out;
		}
	}

	void Process(DspFloatType * samples, DspFloatType * modulation, uint32_t n)
	{
        Undenormal denormal;
		DspFloatType k = resonance * 4;
		for (uint32_t s = 0; s < n; ++s)
		{
            SetCutoff(modulation[s]);
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			DspFloatType out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

			p34 = p33;
			p33 = p32;
			p32 = p3;

			p0 += (fast_tanh(samples[s] - k * out) - fast_tanh(p0)) * cutoff;
			p1 += (fast_tanh(p0) - fast_tanh(p1)) * cutoff;
			p2 += (fast_tanh(p1) - fast_tanh(p2)) * cutoff;
			p3 += (fast_tanh(p2) - fast_tanh(p3)) * cutoff;

			samples[s] = out;
		}
	}

	DspFloatType Tick(DspFloatType input) {
		DspFloatType r = 0.0;
		Process(1,&input,&r);
		return r;

	}

	virtual void SetResonance(DspFloatType r) override
	{
		resonance = r;
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c * 2 * MOOG_PI / sampleRate;
		cutoff = moog_min(cutoff, 1);
	}

private:

	DspFloatType p0;
	DspFloatType p1;
	DspFloatType p2;
	DspFloatType p3;
	DspFloatType p32;
	DspFloatType p33;
	DspFloatType p34;
};


///////////////////////////////////////////////////////////////////////////////////////////
// MusicDSP kaka
///////////////////////////////////////////////////////////////////////////////////////////
class MusicDSPMoog : public LadderFilterBase
{

public:

	MusicDSPMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(stage, 0, sizeof(stage));
		memset(delay, 0, sizeof(delay));
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~MusicDSPMoog()
	{

	}

	void Process(size_t n, DspFloatType * samples, DspFloatType * output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			DspFloatType x = samples[s] - resonance * stage[3];

			// Four cascaded one-pole filters (bilinear transform)
			stage[0] = x * p + delay[0]  * p - k * stage[0];
			stage[1] = stage[0] * p + delay[1] * p - k * stage[1];
			stage[2] = stage[1] * p + delay[2] * p - k * stage[2];
			stage[3] = stage[2] * p + delay[3] * p - k * stage[3];

			// Clipping band-limited sigmoid
			stage[3] -= (stage[3] * stage[3] * stage[3]) / 6.0;

			delay[0] = x;
			delay[1] = stage[0];
			delay[2] = stage[1];
			delay[3] = stage[2];

			output[s] = stage[3];
		}
	}

	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			DspFloatType x = samples[s] - resonance * stage[3];

			// Four cascaded one-pole filters (bilinear transform)
			stage[0] = x * p + delay[0]  * p - k * stage[0];
			stage[1] = stage[0] * p + delay[1] * p - k * stage[1];
			stage[2] = stage[1] * p + delay[2] * p - k * stage[2];
			stage[3] = stage[2] * p + delay[3] * p - k * stage[3];

			// Clipping band-limited sigmoid
			stage[3] -= (stage[3] * stage[3] * stage[3]) / 6.0;

			delay[0] = x;
			delay[1] = stage[0];
			delay[2] = stage[1];
			delay[3] = stage[2];

			samples[s] = stage[3];
		}
	}

    
	DspFloatType Tick(DspFloatType input) {
		DspFloatType r = 0.0;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(DspFloatType r) override
	{
		resonance = r * (t2 + 6.0 * t1) / (t2 - 6.0 * t1);
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = 2.0 * c / sampleRate;

		p = cutoff * (1.8 - 0.8 * cutoff);
		k = 2.0 * sin(cutoff * MOOG_PI * 0.5) - 1.0;
		t1 = (1.0 - p) * 1.386249;
		t2 = 12.0 + t1 * t1;

		SetResonance(resonance);
	}

private:

	DspFloatType stage[4];
	DspFloatType delay[4];

	DspFloatType p;
	DspFloatType k;
	DspFloatType t1;
	DspFloatType t2;

};


///////////////////////////////////////////////////////////////////////////////////////////
// Oberham
///////////////////////////////////////////////////////////////////////////////////////////
class VAOnePole
{
public:

	VAOnePole(DspFloatType sr) : sampleRate(sr)
	{
		Reset();
	}

	void Reset()
	{
		alpha = 1.0;
		beta = 0.0;
		gamma = 1.0;
		delta = 0.0;
		epsilon = 0.0;
		a0 = 1.0;
		feedback = 0.0;
		z1 = 0.0;
	}

	DspFloatType Tick(DspFloatType s)
	{
        Undenormal denormal;
		s = s * gamma + feedback + epsilon * GetFeedbackOutput();
		DspFloatType vn = (a0 * s - z1) * alpha;
		DspFloatType out = vn + z1;
		z1 = vn + out;
		return out;
	}

	void SetFeedback(DspFloatType fb) { feedback = fb; }
	DspFloatType GetFeedbackOutput(){ return beta * (z1 + feedback * delta); }
	void SetAlpha(DspFloatType a) { alpha = a; };
	void SetBeta(DspFloatType b) { beta = b; };

private:

	DspFloatType sampleRate;
	DspFloatType alpha;
	DspFloatType beta;
	DspFloatType gamma;
	DspFloatType delta;
	DspFloatType epsilon;
	DspFloatType a0;
	DspFloatType feedback;
	DspFloatType z1;
};

class OberheimVariationMoog : public LadderFilterBase
{

public:

	OberheimVariationMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		LPF1 = new VAOnePole(sampleRate);
		LPF2 = new VAOnePole(sampleRate);
		LPF3 = new VAOnePole(sampleRate);
		LPF4 = new VAOnePole(sampleRate);

		saturation = 1.0;
		Q = 3.0;

		SetCutoff(1000.f);
		SetResonance(0.1f);
	}

	virtual ~OberheimVariationMoog()
	{
		delete LPF1;
		delete LPF2;
		delete LPF3;
		delete LPF4;
	}

	void Process(size_t n, DspFloatType * samples, DspFloatType * output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			DspFloatType input = samples[s];

			DspFloatType sigma =
				LPF1->GetFeedbackOutput() +
				LPF2->GetFeedbackOutput() +
				LPF3->GetFeedbackOutput() +
				LPF4->GetFeedbackOutput();

			input *= 1.0 + K;

			// calculate input to first filter
			DspFloatType u = (input - K * sigma) * alpha0;

			u = tanh(saturation * u);

			DspFloatType stage1 = LPF1->Tick(u);
			DspFloatType stage2 = LPF2->Tick(stage1);
			DspFloatType stage3 = LPF3->Tick(stage2);
			DspFloatType stage4 = LPF4->Tick(stage3);

			// Oberheim variations
			output[s] =
				oberheimCoefs[0] * u +
				oberheimCoefs[1] * stage1 +
				oberheimCoefs[2] * stage2 +
				oberheimCoefs[3] * stage3 +
				oberheimCoefs[4] * stage4;
		}
	}
	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			DspFloatType input = samples[s];

			DspFloatType sigma =
				LPF1->GetFeedbackOutput() +
				LPF2->GetFeedbackOutput() +
				LPF3->GetFeedbackOutput() +
				LPF4->GetFeedbackOutput();

			input *= 1.0 + K;

			// calculate input to first filter
			DspFloatType u = (input - K * sigma) * alpha0;

			u = tanh(saturation * u);

			DspFloatType stage1 = LPF1->Tick(u);
			DspFloatType stage2 = LPF2->Tick(stage1);
			DspFloatType stage3 = LPF3->Tick(stage2);
			DspFloatType stage4 = LPF4->Tick(stage3);

			// Oberheim variations
			samples[s] =
				oberheimCoefs[0] * u +
				oberheimCoefs[1] * stage1 +
				oberheimCoefs[2] * stage2 +
				oberheimCoefs[3] * stage3 +
				oberheimCoefs[4] * stage4;
		}
	}
	DspFloatType Tick(DspFloatType input) {
		DspFloatType r;
		Process(1,&input,&r);
		return r;
	}
	virtual void SetResonance(DspFloatType r) override
    {
            // this maps resonance = 1->10 to K = 0 -> 4
            K = (4.0) * (r - 1.0)/(10.0 - 1.0);
    }

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c;

		// prewarp for BZT
		DspFloatType wd = 2.0 * MOOG_PI * cutoff;
		DspFloatType T = 1.0 / sampleRate;
		DspFloatType wa = (2.0 / T) * tan(wd * T / 2.0);
		DspFloatType g = wa * T / 2.0;

		// Feedforward coeff
		DspFloatType G = g / (1.0 + g);

		LPF1->SetAlpha(G);
		LPF2->SetAlpha(G);
		LPF3->SetAlpha(G);
		LPF4->SetAlpha(G);

		LPF1->SetBeta(G*G*G / (1.0 + g));
		LPF2->SetBeta(G*G / (1.0 + g));
		LPF3->SetBeta(G / (1.0 + g));
		LPF4->SetBeta(1.0 / (1.0 + g));

		gamma = G*G*G*G;
		alpha0 = 1.0 / (1.0 + K * gamma);

		// Oberheim variations / LPF4
		oberheimCoefs[0] = 0.0;
		oberheimCoefs[1] = 0.0;
		oberheimCoefs[2] = 0.0;
		oberheimCoefs[3] = 0.0;
		oberheimCoefs[4] = 1.0;
	}

private:

	VAOnePole * LPF1;
	VAOnePole * LPF2;
	VAOnePole * LPF3;
	VAOnePole * LPF4;

	DspFloatType K;
	DspFloatType gamma;
	DspFloatType alpha0;
	DspFloatType Q;
	DspFloatType saturation;

	DspFloatType oberheimCoefs[5];
};


///////////////////////////////////////////////////////////////////////////////////////////
// Runge-Kutta Simulator
///////////////////////////////////////////////////////////////////////////////////////////
class RKSimulationMoog : public LadderFilterBase
{
public:

	RKSimulationMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(state, 0, sizeof(state));

		saturation = 3.0;
		saturationInv = 1.0 / saturation;

		oversampleFactor = 1;

		stepSize = 1.0 / (oversampleFactor * sampleRate);

		SetCutoff(1000.f);
		SetResonance(1.0f);
	}

	virtual ~RKSimulationMoog()
	{
	}

	void Process(size_t n, DspFloatType * samples, DspFloatType * output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			for (int j = 0; j < oversampleFactor; j++)
			{
				rungekutteSolver(samples[s], state);
			}

			output[s] = state[3];
		}
	}
	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			for (int j = 0; j < oversampleFactor; j++)
			{
				rungekutteSolver(samples[s], state);
			}
			samples[s] = state[3];
		}
	}
	DspFloatType Tick(DspFloatType input) {
		DspFloatType r;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(DspFloatType r) override
	{
		// 0 to 10
		resonance = r;
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = (2.0 * MOOG_PI * c);
	}

private:

	void calculateDerivatives(DspFloatType input, DspFloatType * dstate, DspFloatType * state)
	{
		DspFloatType satstate0 = clip(state[0], saturation, saturationInv);
		DspFloatType satstate1 = clip(state[1], saturation, saturationInv);
		DspFloatType satstate2 = clip(state[2], saturation, saturationInv);

		dstate[0] = cutoff * (clip(input - resonance * state[3], saturation, saturationInv) - satstate0);
		dstate[1] = cutoff * (satstate0 - satstate1);
		dstate[2] = cutoff * (satstate1 - satstate2);
		dstate[3] = cutoff * (satstate2 - clip(state[3], saturation, saturationInv));
	}

	void rungekutteSolver(DspFloatType input, DspFloatType * state)
	{
		int i;
		DspFloatType deriv1[4], deriv2[4], deriv3[4], deriv4[4], tempState[4];

		calculateDerivatives(input, deriv1, state);

		for (i = 0; i < 4; i++)
			tempState[i] = state[i] + 0.5 * stepSize * deriv1[i];

		calculateDerivatives(input, deriv2, tempState);

		for (i = 0; i < 4; i++)
			tempState[i] = state[i] + 0.5 * stepSize * deriv2[i];

		calculateDerivatives(input, deriv3, tempState);

		for (i = 0; i < 4; i++)
			tempState[i] = state[i] + stepSize * deriv3[i];

		calculateDerivatives(input, deriv4, tempState);

		for (i = 0; i < 4; i++)
			state[i] += (1.0 / 6.0) * stepSize * (deriv1[i] + 2.0 * deriv2[i] + 2.0 * deriv3[i] + deriv4[i]);
	}

	DspFloatType state[4];
	DspFloatType saturation, saturationInv;
	int oversampleFactor;
	DspFloatType stepSize;

};


///////////////////////////////////////////////////////////////////////////////////////////
// The SImple Moog
///////////////////////////////////////////////////////////////////////////////////////////
class SimplifiedMoog : public LadderFilterBase
{
public:

	SimplifiedMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		// To keep the overall level approximately constant, comp should be set
		// to 0.5 resulting in a 6 dB passband gain decrease at the maximum resonance
		// (compared to a 12 dB decrease in the original Moog model
		gainCompensation = 0.5;

		memset(stage, 0, sizeof(stage));
		memset(stageZ1, 0, sizeof(stageZ1));
		memset(stageTanh, 0, sizeof(stageTanh));

		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~SimplifiedMoog()
	{

	}

	// This system is nonlinear so we are probably going to create a signal with components that exceed nyquist.
	// To prevent aliasing distortion, we need to oversample this processing chunk. Where do these extra samples
	// come from? Todo! We can use polynomial interpolation to generate the extra samples, but this is expensive.
	// The cheap solution is to zero-stuff the incoming sample buffer.
	// With resampling, numSamples should be 2x the frame size of the existing sample rate.
	// The output of this filter needs to be run through a decimator to return to the original samplerate.
	void Process(size_t n, DspFloatType * samples, DspFloatType * _output)
	{
        Undenormal denormal;
		// Processing still happens at sample rate...
		for (size_t s = 0; s < n; ++s)
		{
			for (int stageIdx = 0; stageIdx < 4; ++stageIdx)
			{
				if (stageIdx)
				{
					input = stage[stageIdx-1];
					stageTanh[stageIdx-1] = tanh(input);
					stage[stageIdx] = (h * stageZ1[stageIdx] + h0 * stageTanh[stageIdx-1]) + (1.0 - g) * (stageIdx != 3 ? stageTanh[stageIdx] : tanh(stageZ1[stageIdx]));
				}
				else
				{
					input = samples[s] - ((4.0 * resonance) * (output - gainCompensation * samples[s]));
					stage[stageIdx] = (h * tanh(input) + h0 * stageZ1[stageIdx]) + (1.0 - g) * stageTanh[stageIdx];
				}

				stageZ1[stageIdx] = stage[stageIdx];
			}

			output = stage[3];
			SNAP_TO_ZERO(output);
			_output[s] = output;
		}
	}

	void Process(size_t n, DspFloatType * samples)
	{
        Undenormal denormal;
		// Processing still happens at sample rate...
		for (int s = 0; s < n; ++s)
		{
			for (int stageIdx = 0; stageIdx < 4; ++stageIdx)
			{
				if (stageIdx)
				{
					input = stage[stageIdx-1];
					stageTanh[stageIdx-1] = tanh(input);
					stage[stageIdx] = (h * stageZ1[stageIdx] + h0 * stageTanh[stageIdx-1]) + (1.0 - g) * (stageIdx != 3 ? stageTanh[stageIdx] : tanh(stageZ1[stageIdx]));
				}
				else
				{
					input = samples[s] - ((4.0 * resonance) * (output - gainCompensation * samples[s]));
					stage[stageIdx] = (h * tanh(input) + h0 * stageZ1[stageIdx]) + (1.0 - g) * stageTanh[stageIdx];
				}

				stageZ1[stageIdx] = stage[stageIdx];
			}

			output = stage[3];
			SNAP_TO_ZERO(output);
			samples[s] = output;
		}
	}

	DspFloatType Tick(DspFloatType input) {
		DspFloatType r;
		Process(1,&input,&r);
		return r;
	}
	virtual void SetResonance(DspFloatType r) override
	{
		resonance = r;
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c;

		// Not being oversampled at the moment... * 2 when functional
		DspFloatType fs2 = sampleRate;

		// Normalized cutoff [0, 1] in radians: ((2*pi) * cutoff / samplerate)
		g = (2 * MOOG_PI) * cutoff / fs2; // feedback coefficient at fs*2 because of doublesampling
		g *= MOOG_PI / 1.3; // correction factor that allows _cutoff to be supplied Hertz

		// FIR part with gain g
		h = g / 1.3;
		h0 = g * 0.3 / 1.3;
	}

private:

	DspFloatType output;
	DspFloatType lastStage;

	DspFloatType stage[4];
	DspFloatType stageZ1[4];
	DspFloatType stageTanh[3];

	DspFloatType input;
	DspFloatType h;
	DspFloatType h0;
	DspFloatType g;

	DspFloatType gainCompensation;
};


static DspFloatType S_STILSON_GAINTABLE[199] =
{
	0.999969, 0.990082, 0.980347, 0.970764, 0.961304, 0.951996, 0.94281, 0.933777, 0.924866, 0.916077,
	0.90741, 0.898865, 0.890442, 0.882141 , 0.873962, 0.865906, 0.857941, 0.850067, 0.842346, 0.834686,
	0.827148, 0.819733, 0.812378, 0.805145, 0.798004, 0.790955, 0.783997, 0.77713, 0.770355, 0.763672,
	0.75708 , 0.75058, 0.744141, 0.737793, 0.731537, 0.725342, 0.719238, 0.713196, 0.707245, 0.701355,
	0.695557, 0.689819, 0.684174, 0.678558, 0.673035, 0.667572, 0.66217, 0.65686, 0.651581, 0.646393,
	0.641235, 0.636169, 0.631134, 0.62619, 0.621277, 0.616425, 0.611633, 0.606903, 0.602234, 0.597626,
	0.593048, 0.588531, 0.584045, 0.579651, 0.575287 , 0.570953, 0.566681, 0.562469, 0.558289, 0.554169,
	0.550079, 0.546051, 0.542053, 0.538116, 0.53421, 0.530334, 0.52652, 0.522736, 0.518982, 0.515289,
	0.511627, 0.507996 , 0.504425, 0.500885, 0.497375, 0.493896, 0.490448, 0.487061, 0.483704, 0.480377,
	0.477081, 0.473816, 0.470581, 0.467377, 0.464203, 0.46109, 0.457977, 0.454926, 0.451874, 0.448883,
	0.445892, 0.442932, 0.440033, 0.437134, 0.434265, 0.431427, 0.428619, 0.425842, 0.423096, 0.42038,
	0.417664, 0.415009, 0.412354, 0.409729, 0.407135, 0.404572, 0.402008, 0.399506, 0.397003, 0.394501,
	0.392059, 0.389618, 0.387207, 0.384827, 0.382477, 0.380127, 0.377808, 0.375488, 0.37323, 0.370972,
	0.368713, 0.366516, 0.364319, 0.362122, 0.359985, 0.357849, 0.355713, 0.353607, 0.351532, 0.349457,
	0.347412, 0.345398, 0.343384, 0.34137, 0.339417, 0.337463, 0.33551, 0.333588, 0.331665, 0.329773,
	0.327911, 0.32605, 0.324188, 0.322357, 0.320557, 0.318756, 0.316986, 0.315216, 0.313446, 0.311707,
	0.309998, 0.308289, 0.30658, 0.304901, 0.303223, 0.301575, 0.299927, 0.298309, 0.296692, 0.295074,
	0.293488, 0.291931, 0.290375, 0.288818, 0.287262, 0.285736, 0.284241, 0.282715, 0.28125, 0.279755,
	0.27829, 0.276825, 0.275391, 0.273956, 0.272552, 0.271118, 0.269745, 0.268341, 0.266968, 0.265594,
	0.264252, 0.262909, 0.261566, 0.260223, 0.258911, 0.257599, 0.256317, 0.255035, 0.25375
};

class StilsonMoog : public LadderFilterBase
{
public:

	StilsonMoog(DspFloatType sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(state, 0, sizeof(state));
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~StilsonMoog()
	{

	}

	void Process(size_t n, DspFloatType * samples, DspFloatType * _output)
	{
		DspFloatType localState;

		for (int s = 0; s < n; ++s)
		{
			// Scale by arbitrary value on account of our saturation function
			const DspFloatType input = samples[s] * 0.65f;

			// Negative Feedback
			output = 0.25 * (input - output);

			for (int pole = 0; pole < 4; ++pole)
			{
				localState = state[pole];
				output = moog_saturate(output + p * (output - localState));
				state[pole] = output;
				output = moog_saturate(output + localState);
			}

			SNAP_TO_ZERO(output);
			_output[s] = output;
			output *= Q; // Scale stateful output by Q
		}
	}

	void Process(size_t n, DspFloatType * samples)
	{
		DspFloatType localState;

		for (int s = 0; s < n; ++s)
		{
			// Scale by arbitrary value on account of our saturation function
			const DspFloatType input = samples[s] * 0.65f;

			// Negative Feedback
			output = 0.25 * (input - output);

			for (int pole = 0; pole < 4; ++pole)
			{
				localState = state[pole];
				output = moog_saturate(output + p * (output - localState));
				state[pole] = output;
				output = moog_saturate(output + localState);
			}

			SNAP_TO_ZERO(output);
			samples[s] = output;
			output *= Q; // Scale stateful output by Q
		}
	}

	void Process(DspFloatType * samples, DspFloatType *modulation, uint32_t n)
	{
		DspFloatType localState;

		for (int s = 0; s < n; ++s)
		{
			// Scale by arbitrary value on account of our saturation function
			const DspFloatType input = samples[s] * 0.65f;
            SetCutoff(modulation[s]);
			// Negative Feedback
			output = 0.25 * (input - output);

			for (int pole = 0; pole < 4; ++pole)
			{
				localState = state[pole];
				output = moog_saturate(output + p * (output - localState));
				state[pole] = output;
				output = moog_saturate(output + localState);
			}

			SNAP_TO_ZERO(output);
			samples[s] = output;
			output *= Q; // Scale stateful output by Q
		}
	}

	DspFloatType Tick(DspFloatType input) {
		DspFloatType r;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(DspFloatType r) override
	{
		r = moog_min(r, 1);
		resonance = r;

		DspFloatType ix;
		DspFloatType ixfrac;
		int ixint;

		ix = p * 99;
		ixint = floor(ix);
		ixfrac = ix - ixint;

		Q = r * moog_lerp(ixfrac, S_STILSON_GAINTABLE[ixint + 99], S_STILSON_GAINTABLE[ixint + 100]);
	}

	virtual void SetCutoff(DspFloatType c) override
	{
		cutoff = c;

		// Normalized cutoff between [0, 1]
		DspFloatType fc = (cutoff) / sampleRate;
		DspFloatType x2 = fc * fc;
		DspFloatType x3 = fc * fc * fc;

		// Frequency & amplitude correction (Cubic Fit)
		p = -0.69346 * x3 - 0.59515 * x2 + 3.2937 * fc - 1.0072;

		SetResonance(resonance);
	}

private:

	DspFloatType p;
	DspFloatType Q;
	DspFloatType state[4];
	DspFloatType output;
};

///////////////////////////////////////////////////////////////////////////////////////////
// MusicDSP Kaka
///////////////////////////////////////////////////////////////////////////////////////////
struct MoogFilter1 
{
//Init
// cutoff = cutoff freq in Hz
//fs = sampling frequency //(e.g. 44100Hz)
//res = resonance [0 - 1] //(minimum - maximum)

    DspFloatType f,fs,k,p,scale,r,y1,y2,y3,y4,oldx,oldy1,oldy2,oldy3;
    DspFloatType cutoff,Q;
    DspFloatType x;

    MoogFilter1(DspFloatType sampleRate, DspFloatType cutoff, DspFloatType resonance) {
                
        coefficients(sampleRate,cutoff,resonance);
        x=y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
    }

    void coefficients(DspFloatType sampleRate,DspFloatType frequency, DspFloatType resonance) 
    {
        fs = sampleRate;
        cutoff = frequency;
        Q = resonance;

        f = 2 * cutoff / fs; //[0 - 1]
        k = 3.6*f - 1.6*f*f -1; //(Empirical tuning)
        p = (k+1)*0.5;

        // resonance sucks 
        scale = std::exp((1-p)*1.386249);
        r = resonance*scale;        
        //DspFloatType t=(1.f-p)*1.386249f;
        //DspFloatType t2=12.f+t*t;
        //r = Q*(t2+6.f*t)/(t2-6.f*t);
    }
    void setCutoff(DspFloatType c) {        
        c = clamp(c,0,fs/2);
        coefficients(fs,c,Q);
    }
    void setResonance(DspFloatType res) {
        res = clamp(res,0,1);
        coefficients(fs,cutoff,res);
    }
    DspFloatType Tick(DspFloatType input, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
    {
        Undenormal denormal;
        DspFloatType c = cutoff;
        DspFloatType res = Q;
        coefficients(fs,c + 0.5*X*c,Q + 0.5*Y*Q);
        //Loop
        //--Inverted feed back for corner peaking
        x = input - r*y4;                
        
        //Four cascaded onepole filters (bilinear transform)
        y1=x*p + oldx*p - k*y1;        
        y2=y1*p+oldy1*p - k*y2;        
        y3=y2*p+oldy2*p - k*y3;        
        y4=y3*p+oldy3*p - k*y4;        

        coefficients(fs,c,res);

        //Clipper band limited sigmoid
        y4 = y4 - (y4*y4*y4)/6;        
        oldx  = x;
        oldy1 = y1;
        oldy2 = y2;
        oldy3 = y3;

        return A*y4;
    }
};

struct MoogFilter2
{
    // Moog 24 dB/oct resonant lowpass VCF
    // References: CSound source code, Stilson/Smith CCRMA paper.
    // Modified by paul.kellett@maxim.abel.co.uk July 2000

    DspFloatType f, p, q;             //filter coefficients
    DspFloatType b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
    DspFloatType t1, t2;              //temporary buffers
    DspFloatType fs,fc,res;

    // Set coefficients given frequency & resonance [0.0...1.0]
    MoogFilter2(DspFloatType sr, DspFloatType cutoff, DspFloatType r)
    {
        fs = sr;
        fc = cutoff/sr;
        res = r;
        calc();
        b0=b1=b2=b3=b4=0;
    }
    void calc()
    {
        q = 1.0f - fc;
        p = fc + 0.8f * fc * q;
        f = p + p - 1.0f;
        q = res * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
    }
    void setCutoff(DspFloatType f) { fc = f/fs; }
    void setResonance(DspFloatType r) { res = r; }
    DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X = 0, DspFloatType Y=0)
    {
        Undenormal denormals;
        calc();
        DspFloatType in = I - q*b4;       
        t1 = b1; //std::tanh(b1);  
        b1 = (in + b0) * p - b1 * f;
        t2 = b2; //std::tanh(b2);  
        b2 = (b1 + t1) * p - b2 * f;
        t1 = b3; //std::tanh(b3); 
        b3 = (b2 + t2) * p - b3 * f;
        b4 = (b3 + t1) * p - b4 * f;
        b4 = b4 - b4 * b4 * b4 * 0.166667f;
        b0 = in;
        return b4;
    }
};

struct MoogVCF
{
    //Init
    DspFloatType fc;
    DspFloatType fs;
    DspFloatType res;
    DspFloatType out1,out2,out3,out4;
    DspFloatType in1,in2,in3,in4;
    
    MoogVCF(DspFloatType sr, DspFloatType Fc, DspFloatType R)
    {
        fs = sr;
        fc = Fc/sr;
        res= R;
        out1=out2=out3=out4=0;
        in1=in2=in3=in4=0;
    }
    void setCutoff(DspFloatType f) { fc = f/fs; }
    void setResonance(DspFloatType r) { res = r; }

    DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
        DspFloatType f = fc * 1.16;
        DspFloatType fb = res * (1.0 - 0.15 * f * f);
        DspFloatType input = I;
        input -= out4 * fb;
        input *= 0.35013 * (f*f)*(f*f);
        out1 = input + 0.3 * in1 + (1 - f) * out1; // Pole 1
        in1  = input;
        out2 = out1 + 0.3 * in2 + (1 - f) * out2;  // Pole 2
        in2  = out1;
        out3 = out2 + 0.3 * in3 + (1 - f) * out3;  // Pole 3
        in3  = out2;
        out4 = out3 + 0.3 * in4 + (1 - f) * out4;  // Pole 4
        in4  = out3;
        return out4;
    }
};


constexpr DspFloatType gaintable[199] = { 0.999969, 0.990082, 0.980347, 0.970764, 0.961304, 0.951996, 0.94281, 0.933777, 0.924866, 0.916077, 0.90741, 0.898865, 0.890442, 0.882141 , 0.873962, 0.865906, 0.857941, 0.850067, 0.842346, 0.834686, 0.827148, 0.819733, 0.812378, 0.805145, 0.798004, 0.790955, 0.783997, 0.77713, 0.770355, 0.763672, 0.75708 , 0.75058, 0.744141, 0.737793, 0.731537, 0.725342, 0.719238, 0.713196, 0.707245, 0.701355, 0.695557, 0.689819, 0.684174, 0.678558, 0.673035, 0.667572, 0.66217, 0.65686, 0.651581, 0.646393, 0.641235, 0.636169, 0.631134, 0.62619, 0.621277, 0.616425, 0.611633, 0.606903, 0.602234, 0.597626, 0.593048, 0.588531, 0.584045, 0.579651, 0.575287 , 0.570953, 0.566681, 0.562469, 0.558289, 0.554169, 0.550079, 0.546051, 0.542053, 0.538116, 0.53421, 0.530334, 0.52652, 0.522736, 0.518982, 0.515289, 0.511627, 0.507996 , 0.504425, 0.500885, 0.497375, 0.493896, 0.490448, 0.487061, 0.483704, 0.480377, 0.477081, 0.473816, 0.470581, 0.467377, 0.464203, 0.46109, 0.457977, 0.454926, 0.451874, 0.448883, 0.445892, 0.442932, 0.440033, 0.437134, 0.434265, 0.431427, 0.428619, 0.425842, 0.423096, 0.42038, 0.417664, 0.415009, 0.412354, 0.409729, 0.407135, 0.404572, 0.402008, 0.399506, 0.397003, 0.394501, 0.392059, 0.389618, 0.387207, 0.384827, 0.382477, 0.380127, 0.377808, 0.375488, 0.37323, 0.370972, 0.368713, 0.366516, 0.364319, 0.362122, 0.359985, 0.357849, 0.355713, 0.353607, 0.351532,0.349457, 0.347412, 0.345398, 0.343384, 0.34137, 0.339417, 0.337463, 0.33551, 0.333588, 0.331665, 0.329773, 0.327911, 0.32605, 0.324188, 0.322357, 0.320557,0.318756, 0.316986, 0.315216, 0.313446, 0.311707, 0.309998, 0.308289, 0.30658, 0.304901, 0.303223, 0.301575, 0.299927, 0.298309, 0.296692, 0.295074, 0.293488, 0.291931, 0.290375, 0.288818, 0.287262, 0.285736, 0.284241, 0.282715, 0.28125, 0.279755, 0.27829, 0.276825, 0.275391, 0.273956, 0.272552, 0.271118, 0.269745, 0.268341, 0.266968, 0.265594, 0.264252, 0.262909, 0.261566, 0.260223, 0.258911, 0.257599, 0.256317, 0.255035, 0.25375 };

struct StilsonMoog2
{
    
    inline DspFloatType crossfade( DspFloatType amount, DspFloatType a, DspFloatType b ) {
        return (1-amount)*a + amount*b;
    }

    DspFloatType fc,fs,Q,p;
    DspFloatType cutoff,resonance;
    DspFloatType lowpass,highpass,bandpass,lastX;
    DspFloatType state[4], output; //should be gl obal scope / preserved between calls
    DspFloatType pre_gain,post_gain;

    StilsonMoog2(DspFloatType Fc, DspFloatType R, DspFloatType Fs) {        
        fs = Fs;        
        cutoff = Fc;
        resonance = R;
        pre_gain = 2.0f;
        post_gain = 3.0f;
        setCutoff(Fc);
        setResonance(R);
        memset(&state[0],0,4*sizeof(DspFloatType));
        lowpass = highpass = bandpass = lastX = output = 0;
    }
    void setResonance(DspFloatType resonance)        
    {
        DspFloatType ix, ixfrac;
        int ixint;                
        ix = p * 99;
        ixint = floor( ix );        
        ixfrac = ix - ixint;        
        this->resonance = resonance;
        Q = resonance * crossfade( ixfrac, gaintable[ ixint + 99 ], gaintable[ ixint + 100 ] );        
    }
    void setCutoff(DspFloatType frequency) 
    {
        //code for setting pole coefficient based on frequency        
        cutoff = clamp(frequency,0,fs/2);
        fc = 2 * frequency / fs;
        if(fc < 0.005) fc = 0.005;
        DspFloatType x2 = fc*fc;
        DspFloatType x3 = fc*x2;
        p = -0.69346 * x3 - 0.59515 * x2 + 3.2937 * fc - 1.0072; //cubic fit by DFL, not 100% accurate but better than nothing...
    }        
    DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
    {        
        int i,pole;
        DspFloatType temp, input;
        Undenormal denormal;
        
        input  = std::tanh(pre_gain*I);
        output = 0.25 * ( input - output ); //negative feedback
        output = clamp(output,-1,1);

        for( pole = 0; pole < 4; pole++) {
                temp = state[pole];
                output = output + p * (output - temp);
                state[pole] = output;
                output = output + temp;
                //if(std::fabs(output) < 1e-6) output=0;
        }        
        lowpass = output;
        highpass = input - output;
        bandpass = 3 * state[2] - lowpass; //got this one from paul kellet
        output = lowpass;
        output *= Q;  //scale the feedback
        lastX = I;
        return std::tanh(post_gain*output);
    }
};

struct MoogLike
{

    enum {
        LOWPASS,
        HIGHPASS    
    }   
    type = LOWPASS;

    DspFloatType coef[9];
    DspFloatType d[4];
    DspFloatType omega; //peak freq
    DspFloatType g;     //peak mag

    DspFloatType  fs,res;
    DspFloatType  in,out;

    // calculating coefficients:

    DspFloatType k,p,q,a;
    DspFloatType a0,a1,a2,a3,a4;
    
    MoogLike(DspFloatType Fs, DspFloatType Fc, DspFloatType Q, DspFloatType G)
    {
        omega = Fc;
        q  = Q;
        fs = Fs;
        g  = G;
        k =p=q=a=a0=a1=a2=a3=a4=0;
    }

    void SetCoefficients(DspFloatType Fc, DspFloatType R)
    {
        omega = Fc;
        q     = R;
        k=(4.0*g-3.0)/(g+1.0);
        p=1.0-0.25*k;
        p*=p;
        
        if(type == LOWPASS) {
            // LP:
            a=1.0/(std::tan(0.5*omega)*(1.0+p));
            p=1.0+a;
            q=1.0-a;

            a0= 1.0/(k+p*p*p*p);
            a1= 4.0*(k+p*p*p*q);
            a2= 6.0*(k+p*p*q*q);
            a3= 4.0*(k+p*q*q*q);
            a4= (k+q*q*q*q);
            p = a0*(k+1.0);

            coef[0]=p;
            coef[1]=4.0*p;
            coef[2]=6.0*p;
            coef[3]=4.0*p;
            coef[4]=p;
            coef[5]=-a1*a0;
            coef[6]=-a2*a0;
            coef[7]=-a3*a0;
            coef[8]=-a4*a0;
        }
        else {
            // or HP:
            a=std::tan(0.5*omega)/(1.0+p);
            p=a+1.0;
            q=a-1.0;

            a0=1.0/(p*p*p*p+k);
            a1=4.0*(p*p*p*q-k);
            a2=6.0*(p*p*q*q+k);
            a3=4.0*(p*q*q*q-k);
            a4=    (q*q*q*q+k);
            p=a0*(k+1.0);

            coef[0]=p;
            coef[1]=-4.0*p;
            coef[2]=6.0*p;
            coef[3]=-4.0*p;
            coef[4]=p;
            coef[5]=-a1*a0;
            coef[6]=-a2*a0;
            coef[7]=-a3*a0;
            coef[8]=-a4*a0;
        }
    }
    DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X=0, DspFloatType Y=0)
    {
        Undenormal denormal;
                
        in = I;
        // per sample:
        out=coef[0]*in+d[0];
        d[0]=coef[1]*in+coef[5]*out+d[1];
        d[1]=coef[2]*in+coef[6]*out+d[2];
        d[2]=coef[3]*in+coef[7]*out+d[3];
        d[3]=coef[4]*in+coef[8]*out;
        return out;
    }
};



enum FilterType {
    LOWPASS = RBJFilter::FilterType::LOWPASS,
    HIGHPASS = RBJFilter::FilterType::HIGHPASS,
    BANDPASS = RBJFilter::FilterType::BANDPASS,
    ALLPASS = RBJFilter::FilterType::ALLPASS,
    NOTCH = RBJFilter::FilterType::NOTCH,
    PEAK = RBJFilter::FilterType::PEAK,
    LOW_SHELF = RBJFilter::FilterType::LOW_SHELF,
    HIGH_SHELF = RBJFilter::FilterType::HIGH_SHELF,
};

struct Filter
{
    RBJFilter * filter;
    Filter(FilterType type, DspFloatType cutoff, DspFloatType sample_rate) {
        filter = new RBJFilter((RBJFilter::FilterType)type,cutoff,sample_rate);
        assert(filter != nullptr);
    }
    ~Filter() {
        if(filter) delete filter;
    }

    void UpdateCoefficients() { filter->UpdateCoefficients(); }
    void SetCutoff(DspFloatType c) { filter->SetCutoff(c); }
    void SetQ(DspFloatType q) { filter->SetQValue(q); }
    DspFloatType GetCutoff() { return filter->GetCutoff(); }
    DspFloatType GetQ() { return filter->GetQValue(); }

    void Process(DspFloatType * samples, const uint32_t n) { filter->Process(n,samples); }
    DspFloatType Tick(DspFloatType s) { return filter->Tick(s); }
};

struct RBJLowPassFilter : public Filter
{
    RBJLowPassFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(LOWPASS,cutoff,sample_rate) {

    }
};

struct RBJHighPassFilter : public Filter
{
    RBJHighPassFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(HIGHPASS,cutoff,sample_rate) {

    }
};

struct RBJBandPassFilter : public Filter
{
    RBJBandPassFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(BANDPASS,cutoff,sample_rate) {

    }
};
struct RBJAllPassFilter : public Filter
{
    RBJAllPassFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(ALLPASS,cutoff,sample_rate) {

    }
};

struct RBJNotchFilter : public Filter
{
    RBJNotchFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(NOTCH,cutoff,sample_rate) {

    }
};

struct RBJPeakFilter : public Filter
{
    RBJPeakFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(PEAK,cutoff,sample_rate) {

    }
};

struct RBJLowShelfFilter : public Filter
{
    RBJLowShelfFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(LOW_SHELF,cutoff,sample_rate) {

    }
};

struct RBJHighShelfFilter : public Filter
{
    RBJHighShelfFilter(DspFloatType cutoff, DspFloatType sample_rate=44100) : Filter(HIGH_SHELF,cutoff,sample_rate) {

    }
};



enum NoiseType
	{
		WHITE,
		PINK,
		BROWN,
	};

struct NoiseSamples
{
    NoiseGenerator * noise;
    NoiseType type;
    int sampleRate;
    int channels;

    NoiseSamples(NoiseType type, int sampleRate, int channels) {
        noise = new NoiseGenerator();
        assert(noise != nullptr);
    }
    ~NoiseSamples() {
        if(noise) delete noise;
    }

    std::vector<DspFloatType> produce(DspFloatType seconds) {
        std::vector<DspFloatType> r;
        r = noise->produce((NoiseGenerator::NoiseType)type,sampleRate,channels,seconds);
        return r;
    }
};

enum MoogModelType
{
    FINN_MOOG,
    IMPROVED_MOOG,
    POLISH_MOOG,
    MICROTRACKER_MOOG,
    MUSICDSP_MOOG,
    OBERHEIM_MOOG,
    RK_MOOG,
    SIMPLIFIED_MOOG,
    STILSON_MOOG
};

struct MoogLadderFilter
{
    LadderFilterBase * moog;
    DspFloatType sr;
	
    MoogLadderFilter(MoogModelType type, DspFloatType sample_rate=44100) {
        sr = sample_rate;
        switch(type) {
        case FINN_MOOG: moog = new HuovilainenMoog(sample_rate); break;
        case IMPROVED_MOOG: moog = new ImprovedMoog(sample_rate); break;
        case POLISH_MOOG: moog = new KrajeskiMoog(sample_rate); break;
        case MICROTRACKER_MOOG: moog = new MicrotrackerMoog(sample_rate); break;
        case MUSICDSP_MOOG: moog = new MusicDSPMoog(sample_rate); break;
        case OBERHEIM_MOOG: moog = new OberheimVariationMoog(sample_rate); break;
        case RK_MOOG: moog = new RKSimulationMoog(sample_rate); break;
        case STILSON_MOOG: moog = new StilsonMoog(sample_rate); break;
        case SIMPLIFIED_MOOG: moog = new SimplifiedMoog(sample_rate); break;
        }		
        assert(moog != nullptr);
    }
    ~MoogLadderFilter() {
        if(moog) delete moog;
    }
    void Process(uint32_t n,DspFloatType * samples) {
        moog->Process(n,samples);
    }    

    void SetResonance(DspFloatType r) {		
        moog->SetResonance(r);
    }
    void SetCutoff(DspFloatType c) {		
        moog->SetCutoff(c);
    }
	void setType(MoogModelType type)
	{
		if(moog) delete moog;
		switch(type) {
        case FINN_MOOG: moog = new HuovilainenMoog(sampleRate); break;
        case IMPROVED_MOOG: moog = new ImprovedMoog(sampleRate); break;
        case POLISH_MOOG: moog = new KrajeskiMoog(sampleRate); break;
        case MICROTRACKER_MOOG: moog = new MicrotrackerMoog(sampleRate); break;
        case MUSICDSP_MOOG: moog = new MusicDSPMoog(sampleRate); break;
        case OBERHEIM_MOOG: moog = new OberheimVariationMoog(sampleRate); break;
        case RK_MOOG: moog = new RKSimulationMoog(sampleRate); break;
        case STILSON_MOOG: moog = new StilsonMoog(sampleRate); break;
        case SIMPLIFIED_MOOG: moog = new SimplifiedMoog(sampleRate); break;
        }
        assert(moog != nullptr);
	}
    DspFloatType Tick(DspFloatType I,DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
        DspFloatType o = I;
        DspFloatType c = moog->GetCutoff();
        DspFloatType r = moog->GetResonance();
        SetCutoff(clamp(c + c*(X*0.5),0,sr/2));
        SetResonance(clamp(r + Y,0,1));
        Process(1,&o);
        SetCutoff(c);
        SetResonance(r);
        return o;
    }
};

}