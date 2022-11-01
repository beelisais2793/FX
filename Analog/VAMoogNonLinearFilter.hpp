
#pragma once

#include <cmath>
#include "Undenormal.hpp"

namespace Analog::Filters::Moog::NonLinear
{
	// Also called Houvilainen Moog
	// But it's alot easier to spell NonLinear
	struct NonLinearMoog : public FilterProcessor
	{
		NonLinearMoog(DspFloatType sr) : FilterProcessor(),sampleRate(sr), thermal(0.000025)
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

		void Process(uint32_t n,DspFloatType * _input, DspFloatType * _output)
		{
			Undenormal denormal;
			for (uint32_t s = 0; s < n; ++s)
			{
				// Oversample
				for (int j = 0; j < 2; j++)
				{
					DspFloatType input = _input[s] - resQuad * delay[5];
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

		void Process(size_t n, DspFloatType * _input)
		{
			Process(n,_input,_input);
		}

		
		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
			// Oversample
			for (int j = 0; j < 2; j++)
			{
				DspFloatType input = I - resQuad * delay[5];
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
			return  A*delay[5];
		}

		virtual void SetResonance(DspFloatType r) 
		{
			resonance = r;
			resQuad = 4.0 * resonance * acr;
		}

		virtual void SetCutoff(DspFloatType c) 
		{
			cutoff = c;

			DspFloatType fc =  cutoff / sampleRate;
			DspFloatType f  =  fc * 0.5; // oversampled
			DspFloatType fc2 = fc * fc;
			DspFloatType fc3 = fc * fc * fc;

			DspFloatType fcr = 1.8730 * fc3 + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
			acr = -3.9364 * fc2 + 1.8409 * fc + 0.9968;

			tune = (1.0 - std::exp(-((2 * M_PI) * f * fcr))) / thermal;

			SetResonance(resonance);
		}

		DspFloatType GetResonance() { return resonance; }
		DspFloatType GetCutoff() { return cutoff; }

		enum {
			PORT_CUTOFF,
			PORT_RESONANCE,
		};
		void setPort(int port, DspFloatType v) {
			switch(port) {
				case PORT_CUTOFF: SetCutoff(v); break;
				case PORT_RESONANCE: SetResonance(v); break;
			}
		}
		DspFloatType stage[4];
		DspFloatType stageTanh[3];
		DspFloatType delay[6];

		DspFloatType thermal;
		DspFloatType tune;
		DspFloatType acr;
		DspFloatType resQuad;

		DspFloatType cutoff,resonance,sampleRate;
	};
}