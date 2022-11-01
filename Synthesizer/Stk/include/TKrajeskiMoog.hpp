#pragma once

#include "TLadderBase.hpp"
#include "Util.h"

namespace SoundAlchemy::MoogFilters
{

template<typename T>
class TKrajeskiMoog final : public TLadderFilterBase<T>
{
	
public:
	
    TKrajeskiMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
	{
		memset(state, 0, sizeof(state));
		memset(delay, 0, sizeof(delay));
		
		drive = 1.0;
		gComp = 1.0;
		
		SetCutoff(1000.0f);
		SetResonance(0.1f);
	}
	
	virtual ~TKrajeskiMoog() { }
	
	virtual void Process(size_t n, T * samples, T * output)
	{
		for (int s = 0; s < n; ++s)
		{
			state[0] = std::tanh(drive * (samples[s] - 4 * gRes * (state[4] - gComp * samples[s])));
			
			for(int i = 0; i < 4; i++)
			{
				state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
				delay[i] = state[i];
			}
			output[s] = state[4];
		}
	}

	virtual void Process(T * samples, uint32_t n)
	{
		for (int s = 0; s < n; ++s)
		{
			state[0] = std::tanh(drive * (samples[s] - 4 * gRes * (state[4] - gComp * samples[s])));
			
			for(int i = 0; i < 4; i++)
			{
				state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
				delay[i] = state[i];
			}
			samples[s] = state[4];
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
		gRes = this->resonance * (1.0029 + 0.0526 * wc - 0.926 * std::pow(wc, 2) + 0.0218 * std::pow(wc, 3));
	}
	
	virtual void SetCutoff(T c) override
	{
		this->cutoff = c;
		wc = 2 * MOOG_PI * this->cutoff / this->sampleRate;
		g = 0.9892 * wc - 0.4342 * std::pow(wc, 2) + 0.1381 * std::pow(wc, 3) - 0.0202 * std::pow(wc, 4);
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

}
