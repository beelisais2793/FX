#pragma once

#include <cmath>
#include "Undenormal.hpp"


///////////////////////////////////////////////////////////////////////////////////////////
// The new and improved moog filter
///////////////////////////////////////////////////////////////////////////////////////////
const double VT = 0.312;

class ImprovedMoog 
{
public:

	ImprovedMoog(double sr) 
    : sampleRate(sr)
	{
		memset(V, 0, sizeof(V));
		memset(dV, 0, sizeof(dV));
		memset(tV, 0, sizeof(tV));

		drive = 1.0f;

		SetCutoff(1000.0f); // normalized cutoff frequency
		SetResonance(0.1f); // [0, 4]
	}

	virtual ~ImprovedMoog() { }

	
	void Process(uint32_t n, float * samples, float * output)
	{
        Undenormal denormal;
		double dV0, dV1, dV2, dV3;

		for (uint32_t i = 0; i < n; i++)
		{
			dV0 = -g * (std::tanh((drive * samples[i] + resonance * V[3]) / (2.0 * VT)) + tV[0]);
			V[0] += (dV0 + dV[0]) / (2.0 * sampleRate);
			dV[0] = dV0;
			tV[0] = std::tanh(V[0] / (2.0 * VT));

			dV1 = g * (tV[0] - tV[1]);
			V[1] += (dV1 + dV[1]) / (2.0 * sampleRate);
			dV[1] = dV1;
			tV[1] = std::tanh(V[1] / (2.0 * VT));

			dV2 = g * (tV[1] - tV[2]);
			V[2] += (dV2 + dV[2]) / (2.0 * sampleRate);
			dV[2] = dV2;
			tV[2] = std::tanh(V[2] / (2.0 * VT));

			dV3 = g * (tV[2] - tV[3]);
			V[3] += (dV3 + dV[3]) / (2.0 * sampleRate);
			dV[3] = dV3;
			tV[3] = std::tanh(V[3] / (2.0 * VT));

			output[i] = V[3];
		}
	}

	void Process(size_t n, float * i)
    {
        Process(n,i,i);
    }

	void SetResonance(double r)
	{
		resonance = r;
	}

	void SetCutoff(double c)
	{
		cutoff = c;
		x = (M_PI * cutoff) / sampleRate;
		g = 4.0 * M_PI * VT * cutoff * (1.0 - x) / (1.0 + x);
	}

    double GetResonance() { return resonance; }
	double GetCutoff() { return cutoff; }

	double Tick(double input) {
		double r = input;
		Process(1,&input,&r);
		return r;
	}
private:

	double V[4];
	double dV[4];
	double tV[4];

	double x;
	double g;
	double drive;

    double cutoff,resonance,sampleRate;
};
