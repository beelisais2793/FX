
#pragma once

#include "TLadderBase.hpp"
#include "Util.h"

namespace SoundAlchemy::MoogFilters
{
template<typename T>
class TMusicDSPMoog : public TLadderFilterBase<T>
{
	
public:
	
	TMusicDSPMoog(T sampleRate) : TLadderFilterBase<T>(sampleRate)
	{
		memset(stage, 0, sizeof(stage));
		memset(delay, 0, sizeof(delay));
		SetCutoff(1000.0f);
		SetResonance(0.10f);
	}
	
	virtual ~TMusicDSPMoog()
	{

	}
	
	void Process(size_t n, T * samples, T * output)
	{
		for (int s = 0; s < n; ++s)
		{
			T x = samples[s] - this->resonance * stage[3];

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

	void Process(T * samples, uint32_t n)
	{
		for (int s = 0; s < n; ++s)
		{
			T x = samples[s] - this->resonance * stage[3];

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

	T Tick(T input) {
		T r = 0.0;
		Process(1,&input,&r);
		return r;
	}
	
	virtual void SetResonance(T r) override
	{
		this->resonance = r * (t2 + 6.0 * t1) / (t2 - 6.0 * t1);
	}
	
	virtual void SetCutoff(T c) override
	{
		this->cutoff = 2.0 * c / sampleRate;

		p = this->cutoff * (1.8 - 0.8 * this->cutoff);
		k = 2.0 * sin(this->cutoff * MOOG_PI * 0.5) - 1.0;
		t1 = (1.0 - p) * 1.386249;
		t2 = 12.0 + t1 * t1;

		SetResonance(this->resonance);
	}
	
private:
	
	double stage[4];
	double delay[4];

	double p;
	double k;
	double t1;
	double t2;

};

}