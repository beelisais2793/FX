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

#define HZ_TO_RAD(f) (MOOG_PI_2 * f)
#define RAD_TO_HZ(omega) (MOOG_INV_PI_2 * omega)
#define MOOG_PI_2      1.57079632679489661923132169163975144

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
	void Process(uint32_t n, float * samples)
	{
        Undenormal denormal;
		double out = 0;
		for (int s = 0; s < n; ++s)
		{
			out = bCoef[0] * samples[s] + w[0];
			w[0] = bCoef[1] * samples[s] - aCoef[0] * out + w[1];
			w[1] = bCoef[2] * samples[s] - aCoef[1] * out;
			samples[s] = out;
		}
	}
	double Tick(double s)
	{
        Undenormal denormal;
		double out = bCoef[0] * s + w[0];
		w[0] = bCoef[1] * s - aCoef[0] * out + w[1];
		w[1] = bCoef[2] * s - aCoef[1] * out;
		return out;
	}
	void SetBiquadCoefs(std::array<double, 3> b, std::array<double, 2> a)
	{
		bCoef = b;
		aCoef = a;
	}
	
protected:
	std::array<double, 3> bCoef; // b0, b1, b2
	std::array<double, 2> aCoef; // a1, a2
	std::array<double, 2> w; // delays
};


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
	
	
	RBJFilter(FilterType type = FilterType::LOWPASS, double cutoff = 1, double sampleRate = 44100) : sampleRate(sampleRate), t(type)
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
		double factor = 1.0f / a[0];
		
		std::array<double, 2> aNorm;
		std::array<double, 3> bNorm;

		aNorm[0] = a[1] * factor;
		aNorm[1] = a[2] * factor;

		bNorm[0] = b[0] * factor;
		bNorm[1] = b[1] * factor;
		bNorm[2] = b[2] * factor;
		
		SetBiquadCoefs(bNorm, aNorm);
	}
	
	void SetSampleRate(double sr)
	{
		sampleRate = sr;
	}

	// In Hertz, 0 to Nyquist
	void SetCutoff(double c)
	{
		omega = HZ_TO_RAD(c) / sampleRate;
		UpdateCoefficients();
	}
	
	double GetCutoff()
	{
		return omega;
	}
	
	// Arbitrary, from 0.01f to ~20
	void SetQValue(double q)
	{
		Q = q;
		UpdateCoefficients();
	}
	
	double GetQValue()
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

	double sampleRate;
	
	double omega;
	double cosOmega;
	double sinOmega;
	
	double Q;
	double alpha;
	double A;

	std::array<double, 3> a;
	std::array<double, 3> b;
	
	FilterType t;
};

#undef HZ_TO_RAD
#undef RAD_TO_HZ
#undef MOOG_PI_2      