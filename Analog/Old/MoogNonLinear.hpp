
#pragma once

#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::Moog::NonLinear
{
	// Also called Houvilainen Moog
	// But it's alot easier to spell NonLinear
	class NonLinearMoog
	{
	public:

		NonLinearMoog(double sr) : sampleRate(sr), thermal(0.000025)
		{
			memset(stage, 0, sizeof(stage));
			memset(delay, 0, sizeof(delay));
			memset(stageTanh, 0, sizeof(stageTanh));
			SetCutoff(1000.0f);
			SetResonance(0.10f);
		}

		virtual ~NonLinearMoog()
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
					double input = _input[s] - resQuad * delay[5];
					delay[0] = stage[0] = delay[0] + tune * (std::tanh(input * thermal) - stageTanh[0]);
					for (int k = 1; k < 4; k++)
					{
						input = stage[k-1];
						stage[k] = delay[k] + tune * ((stageTanh[k-1] = std::tanh(input * thermal)) - (k != 3 ? stageTanh[k] : tanh(delay[k] * thermal)));
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
			Process(n,_input,_input);
		}

		
		double Tick(double input) {
			double r = 0.0;
			Process(1,&input,&r);
			return r;
		}

		virtual void SetResonance(double r) override
		{
			resonance = r;
			resQuad = 4.0 * resonance * acr;
		}

		virtual void SetCutoff(double c) override
		{
			cutoff = c;

			double fc =  cutoff / sampleRate;
			double f  =  fc * 0.5; // oversampled
			double fc2 = fc * fc;
			double fc3 = fc * fc * fc;

			double fcr = 1.8730 * fc3 + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
			acr = -3.9364 * fc2 + 1.8409 * fc + 0.9968;

			tune = (1.0 - std::exp(-((2 * M_PI) * f * fcr))) / thermal;

			SetResonance(resonance);
		}

		double GetResonance() { return resonance; }
		double GetCutoff() { return cutoff; }

	private:

		double stage[4];
		double stageTanh[3];
		double delay[6];

		double thermal;
		double tune;
		double acr;
		double resQuad;

		double cutoff,resonance,sampleRate;
	};
}