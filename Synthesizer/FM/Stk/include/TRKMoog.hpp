#pragma once


#include "TLadderBase.hpp"
#include "Util.h"

namespace SoundAlchemy::MoogFilters
{
template<typename T>
class TRKSimulationMoog : public TLadderFilterBase<T>
{
	
public:
	
	TRKSimulationMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
	{
		memset(state, 0, sizeof(state));
		
		saturation = 3.0;
		saturationInv = 1.0 / saturation;
		
		oversampleFactor = 1;
		
		stepSize = 1.0 / (oversampleFactor * sampleRate);
		
		SetCutoff(1000.f);
		SetResonance(1.0f);
	}
	
	virtual ~TRKSimulationMoog()
	{
	}
	
	void Process(size_t n, T * samples, T * output)
	{
		for (int s = 0; s < n; ++s)
		{
			for (int j = 0; j < oversampleFactor; j++)
			{
				rungekutteSolver(samples[s], state);
			}
			
			output[s] = state[3];
		}
	}
	void Process(T * samples, uint32_t n)
	{
		for (int s = 0; s < n; ++s)
		{
			for (int j = 0; j < oversampleFactor; j++)
			{
				rungekutteSolver(samples[s], state);
			}			
			samples[s] = state[3];
		}
	}
	T Tick(T input) {
		T r;
		Process(1,&input,&r);
		return r;
	}
	
	virtual void SetResonance(T r) override
	{
		// 0 to 10
		this->resonance = r;
	}
	
	virtual void SetCutoff(T c) override
	{
		this->cutoff = (2.0 * MOOG_PI * c);
	}
	
private:
	
	void calculateDerivatives(T input, double * dstate, double * state)
	{
		double satstate0 = clip(state[0], saturation, saturationInv);
		double satstate1 = clip(state[1], saturation, saturationInv);
		double satstate2 = clip(state[2], saturation, saturationInv);
		
		dstate[0] = this->cutoff * (clip(input - this->resonance * state[3], saturation, saturationInv) - satstate0);
		dstate[1] = this->cutoff * (satstate0 - satstate1);
		dstate[2] = this->cutoff * (satstate1 - satstate2);
		dstate[3] = this->cutoff * (satstate2 - clip(state[3], saturation, saturationInv));
	}

	void rungekutteSolver(T input, double * state)
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

}