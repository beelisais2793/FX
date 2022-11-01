//
//  LadderFilter.cpp
//  ladderFIlter - VST3
//
//  Created by Rares on 07/10/2020.
//

#include "LadderFilter.h"


template <class T>
LadderFilter<T>::LadderFilter(int osFactor)
{
	// initialize voltage arrays
	for(int index = 0; index < NUMBER_OF_FILTERS; ++index)
	{
		mvVolt[index] = 0.f;
		mvVolt_1[index] = 0.f;
		mvDiffVolt[index] = 0.f;
		mvDiffVolt_1[index] = 0.f;
	}
	
	// define oversampling factor
	mOversampleFactor = osFactor;
}

template <class T>
LadderFilter<T>::~LadderFilter()
{
	
}

template <class T>
void LadderFilter<T>::setSampleRate(T sampleRate)
{
	mSampleRate = mOversampleFactor*sampleRate;
}

template <class T>
void LadderFilter<T>::setCutoffFreq(T cutFreq)
{

	mCutoffFreq = cutFreq;
	T var = PI * mCutoffFreq / mSampleRate;
	
	// mBiasControl = I_ctl / (2*C)
	// where I_ctl - control current intensity
	// and C - capacitor value used at the output of each filter stage
	mBiasControl = 4.f * PI * THERMAL_VOLT * mCutoffFreq * (1.f - var)/(1.f + var);
	
}

template <class T>
void LadderFilter<T>::setResonance(T resonance)
{
	mResonance = resonance;
}

template <class T>
void LadderFilter<T>::setGain(T dbLevel)
{
	mGain = juce::Decibels::decibelsToGain(dbLevel);
}

template <class T>
T LadderFilter<T>::process(T inputSample)
{
	for(int i = 0; i < mOversampleFactor; ++i) // repeat sample processing per oversampling factor
	{
		for(int filterIndex = 0; filterIndex < NUMBER_OF_FILTERS; ++filterIndex) // go through filter stages
		{
			// compute voltage differential at current time step
			if(filterIndex == 0) // for the first filter
			{
				mvDiffVolt[filterIndex] = -mBiasControl * (tanh(mvVolt_1[filterIndex] * OVER_TWO_THERMAL_VOLT) + tanh((mGain * inputSample + mResonance * mvVolt_1[NUMBER_OF_FILTERS-1]) * OVER_TWO_THERMAL_VOLT));
			}
			else // for the rest of the filters
			{
				mvDiffVolt[filterIndex] = mBiasControl * (tanh(mvVolt[filterIndex-1] * OVER_TWO_THERMAL_VOLT) - tanh(mvVolt_1[filterIndex] * OVER_TWO_THERMAL_VOLT));
			}
			
			// compute voltage output for current filter at current time step
			mvVolt[filterIndex] += (mvDiffVolt[filterIndex] +  mvDiffVolt_1[filterIndex])/(2*mSampleRate);
			
			// update voltages from previous time step
			mvVolt_1[filterIndex] = mvVolt[filterIndex];
			mvDiffVolt_1[filterIndex] = mvDiffVolt[filterIndex];
		}
	}
	
	// return the voltage output of the last filter
	return mvVolt[NUMBER_OF_FILTERS-1];
}



