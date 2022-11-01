// Based on an implementation by Magnus Jonsson
// https://github.com/magnusjonsson/microtracker (unlicense)

#pragma once


#include "TLadderBase.hpp"
#include "Util.h"

namespace SoundAlchemy::MoogFilters
{
template<typename T>
class TMicrotrackerMoog : public TLadderFilterBase<T>
{

public:

	TMicrotrackerMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
	{
		p0 = p1 = p2 = p3 = p32 = p33 = p34 = 0.0;
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}

	virtual ~TMicrotrackerMoog() {}

	void Process(size_t n, T * samples, T * output)
	{
		double k = this->resonance * 4;
		for (int s = 0; s < n; ++s)
		{
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			double out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

			p34 = p33;
			p33 = p32;
			p32 = p3;

			p0 += (fast_tanh(samples[s] - k * out) - fast_tanh(p0)) * this->cutoff;
			p1 += (fast_tanh(p0) - fast_tanh(p1)) * this->cutoff;
			p2 += (fast_tanh(p1) - fast_tanh(p2)) * this->cutoff;
			p3 += (fast_tanh(p2) - fast_tanh(p3)) * this->cutoff;

			output[s] = out;
		}
	}

	void Process(T * samples, uint32_t n)
	{
		double k = this->resonance * 4;
		for (int s = 0; s < n; ++s)
		{
			// Coefficients optimized using differential evolution
			// to make feedback gain 4.0 correspond closely to the
			// border of instability, for all values of omega.
			double out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

			p34 = p33;
			p33 = p32;
			p32 = p3;

			p0 += (fast_tanh(samples[s] - k * out) - fast_tanh(p0)) * this->cutoff;
			p1 += (fast_tanh(p0) - fast_tanh(p1)) * this->cutoff;
			p2 += (fast_tanh(p1) - fast_tanh(p2)) * this->cutoff;
			p3 += (fast_tanh(p2) - fast_tanh(p3)) * this->cutoff;

			samples[s] = out;
		}
	}

	T Tick(T input) {
		T r = 0.0;
		Process(1,&input,&r);
		return r;
	
	}

	virtual void SetResonance(T r) override
	{
		this->resonance = r;
	}

	virtual void SetCutoff(T c) override
	{
		this->cutoff = c * 2 * MOOG_PI / this->sampleRate;
		this->cutoff = moog_min(this->cutoff, 1);
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

}