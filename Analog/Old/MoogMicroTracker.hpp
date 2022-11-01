#pragma once

#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::Moog::MicrotrackerMoog
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// Stolen from microtracker
	///////////////////////////////////////////////////////////////////////////////////////////
	class MicrotrackerMoog 
	{

	public:

		MicrotrackerMoog(double sr) : sampleRate(sr)
		{
			p0 = p1 = p2 = p3 = p32 = p33 = p34 = 0.0;
			SetCutoff(1000.0f);
			SetResonance(0.10f);
		}

		virtual ~MicrotrackerMoog() {}

		double fast_tanh(double x) 
		{
			double x2 = x * x;
			return x * (27.0 + x2) / (27.0 + 9.0 * x2);
		}

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
			Process(n,samples);
		}

		
		double Tick(double input) {
			double r = 0.0;
			Process(1,&input,&r);
			return r;
		}

		void SetResonance(double r)
		{
			resonance = r;
		}

		void SetCutoff(double c)
		{
			cutoff = c * 2 * M_PI / sampleRate;
			cutoff = moog_min(cutoff, 1);
		}

		double GetResonance() { return resonance; }
		double GetCutoff() { return cutoff; }

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