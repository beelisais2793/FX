
#pragma once

#include "TLadderBase.hpp"

#ifndef MOOG_PI
#define MOOG_PI M_PI
#endif

/*
This model is based on a reference implementation of an algorithm developed by
Stefano D'Angelo and Vesa Valimaki, presented in a paper published at ICASSP in 2013.
This improved model is based on a circuit analysis and compared against a reference
Ngspice simulation. In the paper, it is noted that this particular model is
more accurate in preserving the self-oscillating nature of the real filter.

References: "An Improved Virtual Analog Model of the Moog Ladder Filter"
Original Implementation: D'Angelo, Valimaki
*/

// Thermal voltage (26 milliwats at room temperature)
#define VT 0.312

namespace SoundAlchemy::MoogFilters
{
	template<typename T>
	class TImprovedMoog : public TLadderFilterBase<T>
	{
	public:
		
		TImprovedMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
		{
			memset(V, 0, sizeof(V));
			memset(dV, 0, sizeof(dV));
			memset(tV, 0, sizeof(tV));
			
			drive = 1.0f;
			
			SetCutoff(1000.0f); // normalized cutoff frequency
			SetResonance(0.1f); // [0, 4]
		}
		
		virtual ~TImprovedMoog() { }
		
		void Process(T * samples, uint32_t n)
		{
			double dV0, dV1, dV2, dV3;

			for (int i = 0; i < n; i++)
			{
				dV0 = -g * (std::tanh((drive * samples[i] + this->resonance * V[3]) / (2.0 * VT)) + tV[0]);
				V[0] += (dV0 + dV[0]) / (2.0 * this->sampleRate);
				dV[0] = dV0;
				tV[0] = std::tanh(V[0] / (2.0 * VT));
				
				dV1 = g * (tV[0] - tV[1]);
				V[1] += (dV1 + dV[1]) / (2.0 * this->sampleRate);
				dV[1] = dV1;
				tV[1] = std::tanh(V[1] / (2.0 * VT));
				
				dV2 = g * (tV[1] - tV[2]);
				V[2] += (dV2 + dV[2]) / (2.0 * this->sampleRate);
				dV[2] = dV2;
				tV[2] = std::tanh(V[2] / (2.0 * VT));
				
				dV3 = g * (tV[2] - tV[3]);
				V[3] += (dV3 + dV[3]) / (2.0 * this->sampleRate);
				dV[3] = dV3;
				tV[3] = std::tanh(V[3] / (2.0 * VT));
				
				samples[i] = V[3];
			}
		}

		virtual void Process(uint32_t n,T * samples, T * output)
		{
			double dV0, dV1, dV2, dV3;

			for (int i = 0; i < n; i++)
			{
				dV0 = -g * (std::tanh((drive * samples[i] + this->resonance * V[3]) / (2.0 * VT)) + tV[0]);
				V[0] += (dV0 + dV[0]) / (2.0 * this->sampleRate);
				dV[0] = dV0;
				tV[0] = std::tanh(V[0] / (2.0 * VT));
				
				dV1 = g * (tV[0] - tV[1]);
				V[1] += (dV1 + dV[1]) / (2.0 * this->sampleRate);
				dV[1] = dV1;
				tV[1] = std::tanh(V[1] / (2.0 * VT));
				
				dV2 = g * (tV[1] - tV[2]);
				V[2] += (dV2 + dV[2]) / (2.0 * this->sampleRate);
				dV[2] = dV2;
				tV[2] = std::tanh(V[2] / (2.0 * VT));
				
				dV3 = g * (tV[2] - tV[3]);
				V[3] += (dV3 + dV[3]) / (2.0 * this->sampleRate);
				dV[3] = dV3;
				tV[3] = std::tanh(V[3] / (2.0 * VT));
				
				output[i] = V[3];
			}
		}
		
		T Tick(T input) {
			double dV0, dV1, dV2, dV3;
			dV0 = -g * (std::tanh((drive * input + this->resonance * V[3]) / (2.0 * VT)) + tV[0]);
			V[0] += (dV0 + dV[0]) / (2.0 * this->sampleRate);
			dV[0] = dV0;
			tV[0] = std::tanh(V[0] / (2.0 * VT));
			
			dV1 = g * (tV[0] - tV[1]);
			V[1] += (dV1 + dV[1]) / (2.0 * this->sampleRate);
			dV[1] = dV1;
			tV[1] = std::tanh(V[1] / (2.0 * VT));
			
			dV2 = g * (tV[1] - tV[2]);
			V[2] += (dV2 + dV[2]) / (2.0 * this->sampleRate);
			dV[2] = dV2;
			tV[2] = std::tanh(V[2] / (2.0 * VT));
			
			dV3 = g * (tV[2] - tV[3]);
			V[3] += (dV3 + dV[3]) / (2.0 * this->sampleRate);
			dV[3] = dV3;
			tV[3] = std::tanh(V[3] / (2.0 * VT));
			return V[3];
		}	
		virtual void SetResonance(T r) override
		{
			this->resonance = r;
		}
		
		virtual void SetCutoff(T c) override
		{
			this->cutoff = c;
			x = (MOOG_PI * this->cutoff) / this->sampleRate;
			g = 4.0 * MOOG_PI * VT * this->cutoff * (1.0 - x) / (1.0 + x);
		}
			
	private:
		
		double V[4];
		double dV[4];
		double tV[4];
		
		double x;
		double g;
		double drive;
	};
}
#undef VT