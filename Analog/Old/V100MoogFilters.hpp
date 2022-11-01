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
#include "ClipFunctions.hpp"

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

namespace Moog
{
// Linear interpolation, used to crossfade a gain table
inline float moog_lerp(float amount, float a, float b)
{
	return (1.0f - amount) * a + amount * b;
}

inline float moog_min(float a, float b)
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
inline float moog_saturate(float input)
{
	float x1 = fabs(input + 0.95f);
	float x2 = fabs(input - 0.95f);
	return 0.5f * (x1 - x2);
}

// Imitate the (tanh) clipping function of a transistor pair.
// to 4th order, tanh is x - x*x*x/3; this cubic's
// plateaus are at +/- 1 so clip to 1 and evaluate the cubic.
// This is pretty coarse - for instance if you clip a sinusoid this way you
// can sometimes hear the discontinuity in 4th derivative at the clip point
inline float clip(float value, float saturation, float saturationinverse)
{
	float v2 = (value * saturationinverse > 1 ? 1 :
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

inline double fast_tanh(double x) 
{
	double x2 = x * x;
	return x * (27.0 + x2) / (27.0 + 9.0 * x2);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Moog Ladder
///////////////////////////////////////////////////////////////////////////////////////////
class LadderFilterBase
{
public:

	LadderFilterBase(float sampleRate) : sampleRate(sampleRate) {}
	virtual ~LadderFilterBase() {}

	virtual void Process(size_t n,float * samples) = 0;	
	virtual void SetResonance(float r) = 0;
	virtual void SetCutoff(float c) = 0;

	float GetResonance() { return resonance; }
	float GetCutoff() { return cutoff; }

protected:

	float cutoff;
	float resonance;
	float sampleRate;
};


///////////////////////////////////////////////////////////////////////////////////////////
// Finnish Vampire
///////////////////////////////////////////////////////////////////////////////////////////
class HuovilainenMoog : public LadderFilterBase
{
public:

	HuovilainenMoog(float sampleRate) : LadderFilterBase(sampleRate), thermal(0.000025)
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

	void Process(uint32_t n, float * _input, float * _output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Oversample
			for (int j = 0; j < 2; j++)
			{
				float input = _input[s] - resQuad * delay[5];
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

	void Process(size_t n, float * _input)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Oversample
			for (int j = 0; j < 2; j++)
			{
				float input = _input[s] - resQuad * delay[5];
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

	
	float Tick(float input) {
		float r = 0.0;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(float r) override
	{
		resonance = r;
		resQuad = 4.0 * resonance * acr;
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = c;

		double fc =  cutoff / sampleRate;
		double f  =  fc * 0.5; // oversampled
		double fc2 = fc * fc;
		double fc3 = fc * fc * fc;

		double fcr = 1.8730 * fc3 + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
		acr = -3.9364 * fc2 + 1.8409 * fc + 0.9968;

		tune = (1.0 - exp(-((2 * MOOG_PI) * f * fcr))) / thermal;

		SetResonance(resonance);
	}

private:

	double stage[4];
	double stageTanh[3];
	double delay[6];

	double thermal;
	double tune;
	double acr;
	double resQuad;

};


///////////////////////////////////////////////////////////////////////////////////////////
// The new and improved moog filter
///////////////////////////////////////////////////////////////////////////////////////////
const double VT = 0.312;

class ImprovedMoog : public LadderFilterBase
{
public:

	ImprovedMoog(float sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(V, 0, sizeof(V));
		memset(dV, 0, sizeof(dV));
		memset(tV, 0, sizeof(tV));

		drive = 1.0f;

		SetCutoff(1000.0f); // normalized cutoff frequency
		SetResonance(0.1f); // [0, 4]
	}

	virtual ~ImprovedMoog() { }

	void Process(size_t n, float * samples)
	{
        Undenormal denormal;
		double dV0, dV1, dV2, dV3;

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

	void Process(uint32_t n,float * samples, float * output)
	{
        Undenormal denormal;
		double dV0, dV1, dV2, dV3;

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

	
	virtual void SetResonance(float r) override
	{
		resonance = r;
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = c;
		x = (MOOG_PI * cutoff) / sampleRate;
		g = 4.0 * MOOG_PI * VT * cutoff * (1.0 - x) / (1.0 + x);
	}

	float Tick(float input) {
		float r = input;
		Process(1,&input,&r);
		return r;
	}
private:

	double V[4];
	double dV[4];
	double tV[4];

	double x;
	double g;
	double drive;
};


///////////////////////////////////////////////////////////////////////////////////////////
// The Krajtski 5
///////////////////////////////////////////////////////////////////////////////////////////
class KrajeskiMoog final : public LadderFilterBase
{

public:

    KrajeskiMoog(float sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(state, 0, sizeof(state));
		memset(delay, 0, sizeof(delay));

		drive = 1.0;
		gComp = 1.0;

		SetCutoff(1000.0f);
		SetResonance(0.1f);
	}

	virtual ~KrajeskiMoog() { }

	void Process(size_t n, float * samples, float * output)
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

	void Process(size_t n, float * samples)
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
	
	float Tick(float input) {
		float r = 0.0;
		Process(1,&input,&r);
		return r;

	}
	virtual void SetResonance(float r) override
	{
		resonance = r;
		gRes = resonance * (1.0029 + 0.0526 * wc - 0.926 * pow(wc, 2) + 0.0218 * pow(wc, 3));
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = c;
		wc = 2 * MOOG_PI * cutoff / sampleRate;
		g = 0.9892 * wc - 0.4342 * pow(wc, 2) + 0.1381 * pow(wc, 3) - 0.0202 * pow(wc, 4);
	}

private:

	double state[5];
	double delay[5];
	double wc; // The angular frequency of the cutoff.
	double g; // A derived parameter for the cutoff frequency
	double gRes; // A similar derived parameter for resonance.
	double gComp; // Compensation factor.
	double drive; // A parameter that controls intensity of nonlinearities.

};


///////////////////////////////////////////////////////////////////////////////////////////
// Stolen from microtracker
///////////////////////////////////////////////////////////////////////////////////////////
class MicrotrackerMoog : public LadderFilterBase
{

public:

	MicrotrackerMoog(float sampleRate) : LadderFilterBase(sampleRate)
	{
		p0 = p1 = p2 = p3 = p32 = p33 = p34 = 0.0;
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~MicrotrackerMoog() {}

	void Process(size_t n, float * samples, float * output)
	{
        Undenormal denormal;
		double k = resonance * 4;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			double out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

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

	void Process(size_t n, float * samples)
	{
        Undenormal denormal;
		double k = resonance * 4;
		for (uint32_t s = 0; s < n; ++s)
		{
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			double out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

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

	void Process(float * samples, float * modulation, uint32_t n)
	{
        Undenormal denormal;
		double k = resonance * 4;
		for (uint32_t s = 0; s < n; ++s)
		{
            SetCutoff(modulation[s]);
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			double out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

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

	float Tick(float input) {
		float r = 0.0;
		Process(1,&input,&r);
		return r;

	}

	virtual void SetResonance(float r) override
	{
		resonance = r;
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = c * 2 * MOOG_PI / sampleRate;
		cutoff = moog_min(cutoff, 1);
	}

private:

	double p0;
	double p1;
	double p2;
	double p3;
	double p32;
	double p33;
	double p34;
};


///////////////////////////////////////////////////////////////////////////////////////////
// MusicDSP kaka
///////////////////////////////////////////////////////////////////////////////////////////
class MusicDSPMoog : public LadderFilterBase
{

public:

	MusicDSPMoog(float sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(stage, 0, sizeof(stage));
		memset(delay, 0, sizeof(delay));
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~MusicDSPMoog()
	{

	}

	void Process(size_t n, float * samples, float * output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			float x = samples[s] - resonance * stage[3];

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

	void Process(size_t n, float * samples)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			float x = samples[s] - resonance * stage[3];

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

    
	float Tick(float input) {
		float r = 0.0;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(float r) override
	{
		resonance = r * (t2 + 6.0 * t1) / (t2 - 6.0 * t1);
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = 2.0 * c / sampleRate;

		p = cutoff * (1.8 - 0.8 * cutoff);
		k = 2.0 * sin(cutoff * MOOG_PI * 0.5) - 1.0;
		t1 = (1.0 - p) * 1.386249;
		t2 = 12.0 + t1 * t1;

		SetResonance(resonance);
	}

private:

	double stage[4];
	double delay[4];

	double p;
	double k;
	double t1;
	double t2;

};


///////////////////////////////////////////////////////////////////////////////////////////
// Oberham
///////////////////////////////////////////////////////////////////////////////////////////
class VAOnePole
{
public:

	VAOnePole(float sr) : sampleRate(sr)
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

	double Tick(double s)
	{
        Undenormal denormal;
		s = s * gamma + feedback + epsilon * GetFeedbackOutput();
		double vn = (a0 * s - z1) * alpha;
		double out = vn + z1;
		z1 = vn + out;
		return out;
	}

	void SetFeedback(double fb) { feedback = fb; }
	double GetFeedbackOutput(){ return beta * (z1 + feedback * delta); }
	void SetAlpha(double a) { alpha = a; };
	void SetBeta(double b) { beta = b; };

private:

	float sampleRate;
	double alpha;
	double beta;
	double gamma;
	double delta;
	double epsilon;
	double a0;
	double feedback;
	double z1;
};

class OberheimVariationMoog : public LadderFilterBase
{

public:

	OberheimVariationMoog(float sampleRate) : LadderFilterBase(sampleRate)
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

	void Process(size_t n, float * samples, float * output)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			float input = samples[s];

			double sigma =
				LPF1->GetFeedbackOutput() +
				LPF2->GetFeedbackOutput() +
				LPF3->GetFeedbackOutput() +
				LPF4->GetFeedbackOutput();

			input *= 1.0 + K;

			// calculate input to first filter
			double u = (input - K * sigma) * alpha0;

			u = tanh(saturation * u);

			double stage1 = LPF1->Tick(u);
			double stage2 = LPF2->Tick(stage1);
			double stage3 = LPF3->Tick(stage2);
			double stage4 = LPF4->Tick(stage3);

			// Oberheim variations
			output[s] =
				oberheimCoefs[0] * u +
				oberheimCoefs[1] * stage1 +
				oberheimCoefs[2] * stage2 +
				oberheimCoefs[3] * stage3 +
				oberheimCoefs[4] * stage4;
		}
	}
	void Process(size_t n, float * samples)
	{
        Undenormal denormal;
		for (uint32_t s = 0; s < n; ++s)
		{
			float input = samples[s];

			double sigma =
				LPF1->GetFeedbackOutput() +
				LPF2->GetFeedbackOutput() +
				LPF3->GetFeedbackOutput() +
				LPF4->GetFeedbackOutput();

			input *= 1.0 + K;

			// calculate input to first filter
			double u = (input - K * sigma) * alpha0;

			u = tanh(saturation * u);

			double stage1 = LPF1->Tick(u);
			double stage2 = LPF2->Tick(stage1);
			double stage3 = LPF3->Tick(stage2);
			double stage4 = LPF4->Tick(stage3);

			// Oberheim variations
			samples[s] =
				oberheimCoefs[0] * u +
				oberheimCoefs[1] * stage1 +
				oberheimCoefs[2] * stage2 +
				oberheimCoefs[3] * stage3 +
				oberheimCoefs[4] * stage4;
		}
	}
	float Tick(float input) {
		float r;
		Process(1,&input,&r);
		return r;
	}
	virtual void SetResonance(float r) override
    {
            // this maps resonance = 1->10 to K = 0 -> 4
            K = (4.0) * (r - 1.0)/(10.0 - 1.0);
    }

	virtual void SetCutoff(float c) override
	{
		cutoff = c;

		// prewarp for BZT
		double wd = 2.0 * MOOG_PI * cutoff;
		double T = 1.0 / sampleRate;
		double wa = (2.0 / T) * tan(wd * T / 2.0);
		double g = wa * T / 2.0;

		// Feedforward coeff
		double G = g / (1.0 + g);

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

	double K;
	double gamma;
	double alpha0;
	double Q;
	double saturation;

	double oberheimCoefs[5];
};


///////////////////////////////////////////////////////////////////////////////////////////
// Runge-Kutta Simulator
///////////////////////////////////////////////////////////////////////////////////////////
class RKSimulationMoog : public LadderFilterBase
{
public:

	RKSimulationMoog(float sampleRate) : LadderFilterBase(sampleRate)
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

	void Process(size_t n, float * samples, float * output)
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
	void Process(size_t n, float * samples)
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
	float Tick(float input) {
		float r;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(float r) override
	{
		// 0 to 10
		resonance = r;
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = (2.0 * MOOG_PI * c);
	}

private:

	void calculateDerivatives(float input, double * dstate, double * state)
	{
		double satstate0 = clip(state[0], saturation, saturationInv);
		double satstate1 = clip(state[1], saturation, saturationInv);
		double satstate2 = clip(state[2], saturation, saturationInv);

		dstate[0] = cutoff * (clip(input - resonance * state[3], saturation, saturationInv) - satstate0);
		dstate[1] = cutoff * (satstate0 - satstate1);
		dstate[2] = cutoff * (satstate1 - satstate2);
		dstate[3] = cutoff * (satstate2 - clip(state[3], saturation, saturationInv));
	}

	void rungekutteSolver(float input, double * state)
	{
		int i;
		double deriv1[4], deriv2[4], deriv3[4], deriv4[4], tempState[4];

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

	double state[4];
	double saturation, saturationInv;
	int oversampleFactor;
	double stepSize;

};


///////////////////////////////////////////////////////////////////////////////////////////
// The SImple Moog
///////////////////////////////////////////////////////////////////////////////////////////
class SimplifiedMoog : public LadderFilterBase
{
public:

	SimplifiedMoog(float sampleRate) : LadderFilterBase(sampleRate)
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
	void Process(size_t n, float * samples, float * _output)
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

	void Process(size_t n, float * samples)
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

	float Tick(float input) {
		float r;
		Process(1,&input,&r);
		return r;
	}
	virtual void SetResonance(float r) override
	{
		resonance = r;
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = c;

		// Not being oversampled at the moment... * 2 when functional
		float fs2 = sampleRate;

		// Normalized cutoff [0, 1] in radians: ((2*pi) * cutoff / samplerate)
		g = (2 * MOOG_PI) * cutoff / fs2; // feedback coefficient at fs*2 because of doublesampling
		g *= MOOG_PI / 1.3; // correction factor that allows _cutoff to be supplied Hertz

		// FIR part with gain g
		h = g / 1.3;
		h0 = g * 0.3 / 1.3;
	}

private:

	double output;
	double lastStage;

	double stage[4];
	double stageZ1[4];
	double stageTanh[3];

	double input;
	double h;
	double h0;
	double g;

	float gainCompensation;
};


static float S_STILSON_GAINTABLE[199] =
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

	StilsonMoog(float sampleRate) : LadderFilterBase(sampleRate)
	{
		memset(state, 0, sizeof(state));
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~StilsonMoog()
	{

	}

	void Process(size_t n, float * samples, float * _output)
	{
		float localState;

		for (int s = 0; s < n; ++s)
		{
			// Scale by arbitrary value on account of our saturation function
			const float input = samples[s] * 0.65f;

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

	void Process(size_t n, float * samples)
	{
		float localState;

		for (int s = 0; s < n; ++s)
		{
			// Scale by arbitrary value on account of our saturation function
			const float input = samples[s] * 0.65f;

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

	void Process(float * samples, float *modulation, uint32_t n)
	{
		float localState;

		for (int s = 0; s < n; ++s)
		{
			// Scale by arbitrary value on account of our saturation function
			const float input = samples[s] * 0.65f;
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

	float Tick(float input) {
		float r;
		Process(1,&input,&r);
		return r;
	}

	virtual void SetResonance(float r) override
	{
		r = moog_min(r, 1);
		resonance = r;

		double ix;
		double ixfrac;
		int ixint;

		ix = p * 99;
		ixint = floor(ix);
		ixfrac = ix - ixint;

		Q = r * moog_lerp(ixfrac, S_STILSON_GAINTABLE[ixint + 99], S_STILSON_GAINTABLE[ixint + 100]);
	}

	virtual void SetCutoff(float c) override
	{
		cutoff = c;

		// Normalized cutoff between [0, 1]
		double fc = (cutoff) / sampleRate;
		double x2 = fc * fc;
		double x3 = fc * fc * fc;

		// Frequency & amplitude correction (Cubic Fit)
		p = -0.69346 * x3 - 0.59515 * x2 + 3.2937 * fc - 1.0072;

		SetResonance(resonance);
	}

private:

	double p;
	double Q;
	double state[4];
	double output;
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

    float f,fs,k,p,scale,r,y1,y2,y3,y4,oldx,oldy1,oldy2,oldy3;
    float cutoff,Q;
    float x;

    MoogFilter1(float sampleRate, float cutoff, float resonance) {
                
        coefficients(sampleRate,cutoff,resonance);
        x=y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
    }

    void coefficients(float sampleRate,float frequency, float resonance) 
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
        //float t=(1.f-p)*1.386249f;
        //float t2=12.f+t*t;
        //r = Q*(t2+6.f*t)/(t2-6.f*t);
    }
    void setCutoff(float c) {        
        c = clamp(c,0,fs/2);
        coefficients(fs,c,Q);
    }
    void setResonance(float res) {
        res = clamp(res,0,1);
        coefficients(fs,cutoff,res);
    }
    float Tick(float input, float A=1, float X=0, float Y=0)
    {
        Undenormal denormal;
        float c = cutoff;
        float res = Q;
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

    float f, p, q;             //filter coefficients
    float b0, b1, b2, b3, b4;  //filter buffers (beware denormals!)
    float t1, t2;              //temporary buffers
    float fs,fc,res;

    // Set coefficients given frequency & resonance [0.0...1.0]
    MoogFilter2(float sr, float cutoff, float r)
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
    void setCutoff(float f) { fc = f/fs; }
    void setResonance(float r) { res = r; }
    float Tick(float I, float A=1, float X = 0, float Y=0)
    {
        Undenormal denormals;
        calc();
        float in = I - q*b4;       
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
    double fc;
    double fs;
    double res;
    double out1,out2,out3,out4;
    double in1,in2,in3,in4;
    
    MoogVCF(double sr, double Fc, double R)
    {
        fs = sr;
        fc = Fc/sr;
        res= R;
        out1=out2=out3=out4=0;
        in1=in2=in3=in4=0;
    }
    void setCutoff(double f) { fc = f/fs; }
    void setResonance(double r) { res = r; }

    float Tick(float I, float A=1, float X=0, float Y=0) {
        double f = fc * 1.16;
        double fb = res * (1.0 - 0.15 * f * f);
        double input = I;
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


constexpr float gaintable[199] = { 0.999969, 0.990082, 0.980347, 0.970764, 0.961304, 0.951996, 0.94281, 0.933777, 0.924866, 0.916077, 0.90741, 0.898865, 0.890442, 0.882141 , 0.873962, 0.865906, 0.857941, 0.850067, 0.842346, 0.834686, 0.827148, 0.819733, 0.812378, 0.805145, 0.798004, 0.790955, 0.783997, 0.77713, 0.770355, 0.763672, 0.75708 , 0.75058, 0.744141, 0.737793, 0.731537, 0.725342, 0.719238, 0.713196, 0.707245, 0.701355, 0.695557, 0.689819, 0.684174, 0.678558, 0.673035, 0.667572, 0.66217, 0.65686, 0.651581, 0.646393, 0.641235, 0.636169, 0.631134, 0.62619, 0.621277, 0.616425, 0.611633, 0.606903, 0.602234, 0.597626, 0.593048, 0.588531, 0.584045, 0.579651, 0.575287 , 0.570953, 0.566681, 0.562469, 0.558289, 0.554169, 0.550079, 0.546051, 0.542053, 0.538116, 0.53421, 0.530334, 0.52652, 0.522736, 0.518982, 0.515289, 0.511627, 0.507996 , 0.504425, 0.500885, 0.497375, 0.493896, 0.490448, 0.487061, 0.483704, 0.480377, 0.477081, 0.473816, 0.470581, 0.467377, 0.464203, 0.46109, 0.457977, 0.454926, 0.451874, 0.448883, 0.445892, 0.442932, 0.440033, 0.437134, 0.434265, 0.431427, 0.428619, 0.425842, 0.423096, 0.42038, 0.417664, 0.415009, 0.412354, 0.409729, 0.407135, 0.404572, 0.402008, 0.399506, 0.397003, 0.394501, 0.392059, 0.389618, 0.387207, 0.384827, 0.382477, 0.380127, 0.377808, 0.375488, 0.37323, 0.370972, 0.368713, 0.366516, 0.364319, 0.362122, 0.359985, 0.357849, 0.355713, 0.353607, 0.351532,0.349457, 0.347412, 0.345398, 0.343384, 0.34137, 0.339417, 0.337463, 0.33551, 0.333588, 0.331665, 0.329773, 0.327911, 0.32605, 0.324188, 0.322357, 0.320557,0.318756, 0.316986, 0.315216, 0.313446, 0.311707, 0.309998, 0.308289, 0.30658, 0.304901, 0.303223, 0.301575, 0.299927, 0.298309, 0.296692, 0.295074, 0.293488, 0.291931, 0.290375, 0.288818, 0.287262, 0.285736, 0.284241, 0.282715, 0.28125, 0.279755, 0.27829, 0.276825, 0.275391, 0.273956, 0.272552, 0.271118, 0.269745, 0.268341, 0.266968, 0.265594, 0.264252, 0.262909, 0.261566, 0.260223, 0.258911, 0.257599, 0.256317, 0.255035, 0.25375 };

struct StilsonMoog2
{
    
    inline float crossfade( float amount, float a, float b ) {
        return (1-amount)*a + amount*b;
    }

    float fc,fs,Q,p;
    float cutoff,resonance;
    float lowpass,highpass,bandpass,lastX;
    float state[4], output; //should be gl obal scope / preserved between calls
    float pre_gain,post_gain;

    StilsonMoog2(float Fc, float R, float Fs) {        
        fs = Fs;        
        cutoff = Fc;
        resonance = R;
        pre_gain = 2.0f;
        post_gain = 3.0f;
        setCutoff(Fc);
        setResonance(R);
        memset(&state[0],0,4*sizeof(float));
        lowpass = highpass = bandpass = lastX = output = 0;
    }
    void setResonance(float resonance)        
    {
        float ix, ixfrac;
        int ixint;                
        ix = p * 99;
        ixint = floor( ix );        
        ixfrac = ix - ixint;        
        this->resonance = resonance;
        Q = resonance * crossfade( ixfrac, gaintable[ ixint + 99 ], gaintable[ ixint + 100 ] );        
    }
    void setCutoff(float frequency) 
    {
        //code for setting pole coefficient based on frequency        
        cutoff = clamp(frequency,0,fs/2);
        fc = 2 * frequency / fs;
        if(fc < 0.005) fc = 0.005;
        float x2 = fc*fc;
        float x3 = fc*x2;
        p = -0.69346 * x3 - 0.59515 * x2 + 3.2937 * fc - 1.0072; //cubic fit by DFL, not 100% accurate but better than nothing...
    }        
    float Tick(float I, float A=1, float X=0, float Y=0)
    {        
        int i,pole;
        float temp, input;
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

    double coef[9];
    double d[4];
    double omega; //peak freq
    double g;     //peak mag

    float  fs,res;
    float  in,out;

    // calculating coefficients:

    double k,p,q,a;
    double a0,a1,a2,a3,a4;
    
    MoogLike(float Fs, float Fc, float Q, float G)
    {
        omega = Fc;
        q  = Q;
        fs = Fs;
        g  = G;
        k =p=q=a=a0=a1=a2=a3=a4=0;
    }

    void SetCoefficients(float Fc, float R)
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
    float Tick(float I, float A = 1, float X=0, float Y=0)
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
    float sr;

    MoogLadderFilter(MoogModelType type, float sample_rate=44100) {
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
    void Process(uint32_t n,float * samples) {
        moog->Process(n,samples);
    }    

    void SetResonance(float r) {
        moog->SetResonance(r);
    }
    void SetCutoff(float c) {
        moog->SetCutoff(c);
    }
    float Tick(float I,float A=1, float X=0, float Y=0) {
        float o = I;
        float c = moog->GetCutoff();
        float r = moog->GetResonance();
        SetCutoff(clamp(c + c*(X*0.5),0,sr/2));
        SetResonance(clamp(r + Y,0,1));
        Process(1,&o);
        SetCutoff(c);
        SetResonance(r);
        return o;
    }
};

}