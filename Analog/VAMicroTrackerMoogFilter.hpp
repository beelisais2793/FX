#pragma once

#include <algorithm>
#include <cmath>
#include "Undenormal.hpp"
#include "SoundObject.hpp"

namespace Analog::Filters::Moog
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// Stolen from microtracker
	///////////////////////////////////////////////////////////////////////////////////////////
	class MicrotrackerMoog : public FilterProcessor
	{

	public:

		MicrotrackerMoog(DspFloatType sr) : FilterProcessor(),sampleRate(sr)
		{
			p0 = p1 = p2 = p3 = p32 = p33 = p34 = 0.0;
			SetCutoff(1000.0f);
			SetResonance(0.10f);
		}

		virtual ~MicrotrackerMoog() {}

		DspFloatType fast_tanh(DspFloatType x) 
		{
			DspFloatType x2 = x * x;
			return x * (27.0 + x2) / (27.0 + 9.0 * x2);
		}

		void Process(size_t n, float * samples, float * output)
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

		void Process(size_t n, float * samples)
		{
			Process(n,samples);
		}

		
		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
			Undenormal denormal;
			DspFloatType k = resonance * 4;
			DspFloatType out = p3 * 0.360891 + p32 * 0.417290 + p33 * 0.177896 + p34 * 0.0439725;

			p34 = p33;
			p33 = p32;
			p32 = p3;

			p0 += (fast_tanh(I - k * out) - fast_tanh(p0)) * cutoff;
			p1 += (fast_tanh(p0) - fast_tanh(p1)) * cutoff;
			p2 += (fast_tanh(p1) - fast_tanh(p2)) * cutoff;
			p3 += (fast_tanh(p2) - fast_tanh(p3)) * cutoff;

			return A * out;

		}

		void SetResonance(DspFloatType r)
		{
			resonance = r;
		}

		void SetCutoff(DspFloatType c)
		{
			cutoff = c * 2 * M_PI / sampleRate;
			cutoff = std::min(cutoff, (DspFloatType)1);
		}

		DspFloatType GetResonance() { return resonance; }
		DspFloatType GetCutoff() { return cutoff; }

		enum
        {
            PORT_CUTOFF,
            PORT_RESONANCE,		
        };
        void setPort(int port, DspFloatType v)
        {
            switch (port)
            {
            case PORT_CUTOFF:
                SetCutoff(v);
                break;
            case PORT_RESONANCE:
                SetResonance(v);
                break;			
            }
        }
	private:

		DspFloatType p0;
		DspFloatType p1;
		DspFloatType p2;
		DspFloatType p3;
		DspFloatType p32;
		DspFloatType p33;
		DspFloatType p34;
		DspFloatType cutoff,resonance,sampleRate;
	};
}