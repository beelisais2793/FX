/*
  ==============================================================================
    LFO.h
    Created: 21 Mar 2019 12:02:50pm
    Author:  Joonas
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

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
};

LFO.cpp
	#include "LFO.h"
	#define _USE_MATH_DEFINES
	#include <math.h>
	
	LFO::LFO()
	{

		for (auto i = 0; i < 1024; ++i)
		{
			// Sin
			mSinArray[i] = static_cast<float>(sin((static_cast<double> (i) / 1024.f) * (2.0 * M_PI)));

			// Lowpass filtered versions of waves
			mSquareArray[i] = 0.f;
			mSawtoothArray[i] = 0.f;
			mTriangleArray[i] = 0.f;

			// Triangle
			for (int g = 0; g <= 15; ++g)
			{
				double n = static_cast<double>(g);
				mTriangleArray[i] += static_cast<float>(pow(-1.0, n) * (1.0 / pow((2.0 * n + 1.0), 2)) 
									* sin(2.0 * M_PI * (2.0 * n + 1.0) * i / 1024.0));
			}

			// Sawtooth
			for (int g = 1; g <= 30; ++g)
			{
				double n = static_cast<double> (g);
				mSawtoothArray[i] += static_cast<float>(pow(-1.0, static_cast<double>(g + 1)) * (1.0 / n) * sin(2.0 * M_PI * i * n / 1024.0));
			}

			// Square
			for (int g = 1; g <= 29; g+=2)
			{
				double n = static_cast<double> (g);
				mSquareArray[i] += static_cast<float>((1.0 / n) * sin(2.0 * M_PI * i * n / 1024.0));
			}
			if (i == 0)
			{
				mMaxSqr = mSquareArray[i];
				mMaxSaw = mSawtoothArray[i];
				mMaxTri = mTriangleArray[i];
			}
			else
			{
				if (mSquareArray[i] > mMaxSqr)
					mMaxSqr = mSquareArray[i];

				if (mSawtoothArray[i] > mMaxSaw)
					mMaxSaw = mSawtoothArray[i];

				if (mTriangleArray[i] > mMaxTri)
					mMaxTri = mTriangleArray[i];
			}

		}
		for (int i = 0; i < 1024; ++i)
		{
			mSquareArray[i] /= mMaxSqr;
			mSawtoothArray[i] /= mMaxSaw;
			mTriangleArray[i] /= mMaxTri;
		}
	}

	LFO::~LFO()
	{
		// Empty destructor
	}

	void LFO::prepare(const float& sampleRate, const float& maximumBlockSize)
	{
		mSampleRate		 = sampleRate;
		mSamplesPerBlock = maximumBlockSize;
	}

	float LFO::getValue()
	{
		float readIndex = mReadIndex + 1024.f * (mPhase / 360.f);
		if (readIndex > 1024)
			readIndex = readIndex - 1024.f;

		float LFOVal = 0.f;
		int readIndexInt = static_cast<int>(readIndex);
		float frac = readIndex - readIndexInt;
		int readIndexNext = readIndexInt + 1 > 1023 ? 0 : readIndexInt + 1;

		switch (mWaveform)
		{
		case Waveforms::sine:
			LFOVal = linearInterp(mSinArray[readIndexInt], mSinArray[readIndexNext], frac);
			break;
		case Waveforms::saw:
			LFOVal = linearInterp(mSawtoothArray[readIndexInt], mSawtoothArray[readIndexNext], frac);
			break;
		case Waveforms::tri:
			LFOVal = linearInterp(mTriangleArray[readIndexInt], mTriangleArray[readIndexNext], frac);
			break;
		case Waveforms::square:
			LFOVal = linearInterp(mSquareArray[readIndexInt], mSquareArray[readIndexNext], frac);
			break;
		default:
			LFOVal = linearInterp(mSinArray[readIndexInt], mSinArray[readIndexNext], frac);
			break;
		}

		LFOVal *= mDepth;

		if (mUnipolar)
		{
			LFOVal /= 2;
			LFOVal += 0.5;

			if (LFOVal < 0 || LFOVal > 1)
				return 0.f;
		}

		return LFOVal;
	}

	float LFO::getValueAndAdvance()
	{
		float value = getValue();
		advanceSamples(1);
		return value;
	}

	void LFO::setFreq(const float & freq)
	{
		mInc = 1024.f * (freq / mSampleRate);
	}

	void LFO::setUnipolar(const bool & isUnipolar)
	{
		mUnipolar = isUnipolar;
	}

	void LFO::setWaveform(const int & waveform)
	{
		mWaveform = waveform;
	}

	void LFO::advanceSamples(const int & numSamples)
	{
		mReadIndex += (mInc * numSamples);
		if (mReadIndex > 1024)
			mReadIndex = mReadIndex - 1024;
	}

	void LFO::advanceBlock()
	{
		mReadIndex += (mInc * mSamplesPerBlock);
		if (mReadIndex > 1024)
			mReadIndex = mReadIndex - 1024;
	}

	float LFO::linearInterp(const float & y0, const float & yp1, const float & frac)
	{
		return y0 * (1 - frac) + yp1  * frac;
	}

	void LFO::setPhase(const float & phase)
	{
		mPhase = phase;
	}

	void LFO::setDepth(const float & depth)
	{
		mDepth = depth / 100.f;
	}