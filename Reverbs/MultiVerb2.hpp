#pragma once
#include <cmath>

#define ORDER 8 // number of waveguide pairs
#define ULTDELAY 24000 // maximum waveguide delay in samples
#define GOLD 0.0011f // factor to convert sq ft to sample delay based on golden rectangle

class Mverb
{
public:

	Mverb();
	~Mverb();

	//Parameters
	float pctRev; // "percent" reverb (range 0 - 1)
	float level; // level converted from dB

	// Methods
	void Update(float, float, float, float); // Called to adjust waveguide parameters
	void Process(float*, float*, int); // Process block of stereo samples

	private:

	// delay lines
	float east [ORDER][ULTDELAY];
	float west [ORDER][ULTDELAY];
	int head;
	int tail[ORDER];
	float damp [ORDER];
	float gain [ORDER];
	
	// state vectors
	float xEast [ORDER];
	float xWest [ORDER];
};



bool isPrime(int n) // Check whether a number is prime
{
	if( n <= 1 )
		return false;
	int half = n / 2;
	for( int i = 2; i <= half; i++ )
		if( n % i == 0 )
			return false;
	return true;
}

Mverb::Mverb()
{
	// initialize state vectors and waveguides
	for (int i = 0; i < ORDER; i++)
	{
		xEast[i] = 0.0f;
		xWest[i] = 0.0f;
		tail[i] = 0;
		for (int j = 0; j < ULTDELAY; j++)
		{
			east[i][j] = 0.0f;
			west[i][j] = 0.0f;
		}
	}
	head = 0; // same pointer for all waveguides
}
Mverb::~Mverb()
{
}

void Mverb::Update(float lfRevTime, float hfRevTime, float roomArea, float fs)
{
	// Compute waveguide parameters based on user input
	float maxDelay = GOLD * sqrt(roomArea) * fs; // Sample delay, unrounded for now
	float minDelay = maxDelay / 10.0f;
	float alpha = pow(minDelay / maxDelay, 1.0f / float(ORDER - 1)); // exponential sequence of delays
	float alf, ahf;
	int d;

	for (int i = 0; i < ORDER; i++) 
	{
		// calculate prime delays
		d = int(maxDelay * pow(alpha, i) + 0.5f);
		while (!isPrime(d))
			d -= 1;
		// adjust delay buffer pointer
		tail[i] = head - d;
		if (tail[i] < 0)
			tail[i] += ULTDELAY;
		// compute damping and gain
		alf = pow(10.0f, -3.0f * float(d) / (fs * lfRevTime));
		ahf = pow(10.0f, -3.0f * float(d) / (fs * hfRevTime));
		damp[i] = (alf - ahf) / (alf + ahf);
		gain[i] = alf * (1.0f - damp[i]);
	}
}

void Mverb::Process(float* LeftSamples, float* RightSamples, int NumSamples)
{
	// process block of samples
	// state equations
	float vEast[ORDER];
	float vWest[ORDER];
	for (int n = 0; n < NumSamples; n++)
	{
		// matrix multiplication step
		float xEastAvg = 0.0f;
		float xWestAvg = 0.0f;
		for (int i = 0; i < ORDER; i++)
		{
			xEastAvg += xEast[i];
			xWestAvg += xWest[i];
		}
		xEastAvg /= float(ORDER);
		xWestAvg /= float(ORDER);
		// vE = A * xW + B * uL
		// vW = A * xE + B * uR
		for (int i = 0; i < ORDER; i++) {
			vEast[i] = xWestAvg - xWest[i] + LeftSamples[n];
			vWest[i] = xEastAvg - xEast[i] + RightSamples[n];
		}
		// Output
		// yL = C * xW + D * uL
		// yR = C * xW + D * uR
		LeftSamples[n] = (pctRev * xWestAvg + (1.0f - pctRev) * LeftSamples[n]) * level;
		RightSamples[n] = (pctRev * xEastAvg + (1.0f - pctRev) * RightSamples[n]) * level;
		// Waveguide equations
		for (int i = 0; i < ORDER; i++) {
			// xE = d * xE + g * vE_old
			// xW = d * xW + g * vW_old
			xEast[i] = damp[i] * xEast[i] + gain[i] * east[i][tail[i]];
			xWest[i] = damp[i] * xWest[i] + gain[i] * west[i][tail[i]];
			// adjust buffers
			east[i][head] = vEast[i];
			west[i][head] = vWest[i];
			tail[i] = (tail[i] + 1) % ULTDELAY;
		}
		head = (head + 1) % ULTDELAY;
	}
}


