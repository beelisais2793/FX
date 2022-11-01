#pragma once

#define _USE_MATH_DEFINES 
#include <cmath>

#include "GPModel.h"
#include "GPDrunkRNG.h"

namespace GP
{
	/**
		Implements a single continous oscillator 
	*/
	class ContinuousOsc
	{
	public:

		using WaveTable = std::vector<float>;
	public:
		ContinuousOsc(unsigned int wtSize);

		inline float tick();

		inline void setFrequency(float freqNormalized);
		inline void setProgressionFrequency(float freqNormalized);

	private:

		inline void updateTables();

		inline float readTable();


		void generateTable(std::vector<float> *table);

	private:

		const unsigned int WTSize;


		// parameter level
		GP::Model *model{ nullptr };

		float oscFreq{ 440.f/44100.f };
		float progressionFreq{ 10.f / 44100.f };


		// data level
		float progression{ 0 };
		float phase{ 0 };

		WaveTable wt1;
		WaveTable wt2;

		WaveTable *wtCur{ &wt1 };
		WaveTable *wtNext{ &wt2 };

		std::vector<DrunkRNG> rngs;

		std::vector<float> rndVector;
	};
}


inline float GP::ContinuousOsc::tick()
{
	// update continous wave progression
	// generate new wave table if necessary
	progression += progressionFreq;

	if (progression >= 1)
	{
		progression -= 1;
		updateTables();
	}

	// update phase
	phase += oscFreq;
	if (phase > 1) phase -= 1;

	// read out
	return readTable();

}

inline void GP::ContinuousOsc::setFrequency(float freqNormalized)
{
	oscFreq = freqNormalized;
}

inline void GP::ContinuousOsc::setProgressionFrequency(float freqNormalized)
{
	progressionFreq = freqNormalized;
}


inline void GP::ContinuousOsc::updateTables()
{
	// swap
	std::swap(wtCur, wtNext);
	
	// generate new Table
	generateTable(wtNext);
}

inline float GP::ContinuousOsc::readTable()
{

	float interpFrac = phase * WTSize;
	int interpIdx = std::floor(interpFrac);
	interpFrac -= interpIdx;

	int idx0 = interpIdx;
	int idx1 = (idx0 + 1) % WTSize;

	float wtCurReadOut  = (1. - interpFrac) * wtCur->at(idx0)  + interpFrac * wtCur->at(idx1);
	float wtNextReadOut = (1. - interpFrac) * wtNext->at(idx0) + interpFrac * wtNext->at(idx1);

	float out = (1.f - progression) * wtCurReadOut + progression * wtNextReadOut;

	return out;
}

