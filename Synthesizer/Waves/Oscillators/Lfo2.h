/*
  ==============================================================================
    LFO.h
    Created: 21 Mar 2019 12:02:50pm
    Author:  Joonas
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

enum Waveforms
{
	sine,	// 0
	saw,	// 1
	tri,	// 2
	square	// 3
};

class LFO
{
public:
    LFO();
    ~LFO();
	void prepare(const float& sampleRate, const float& maximumBlockSize);
	float getValue();
	float getValueAndAdvance();
	void setFreq(const float& freq);
	void setUnipolar(const bool& isUnipolar);
	void setWaveform(const int& waveform);
	void advanceSamples(const int& numSamples);
	void advanceBlock();
	float linearInterp(const float& y0, const float& yp1, const float& frac);
	void setPhase(const float& phase);
	void setDepth(const float& depth);


private:
	float mReadIndex = 0.f;
	float mPhase	 = 0.f;
	int mWaveform	 = 0;
	float mInc		 = 0.f;
	bool mUnipolar   = true;
	float mDepth	 = 1.f;

	float mSampleRate	   = 44100.f;
	float mSamplesPerBlock = 512.f;

	// Wavetables
	float mSinArray[1024]	   = { 0.f };
	float mSawtoothArray[1024] = { 0.f };
	float mTriangleArray[1024] = { 0.f };
	float mSquareArray[1024]   = { 0.f };

	float mMaxSqr = 0.f;
	float mMaxSaw = 0.f;
	float mMaxTri = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFO)
};