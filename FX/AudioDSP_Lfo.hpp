/*
  ==============================================================================
    LFO.h
    Created: 21 Mar 2019 12:02:50pm
    Author:  Joonas
  ==============================================================================
*/

#pragma once

#include "SoundObject.hpp"
#define _USE_MATH_DEFINES
#include <array>
#include <cmath>

//==============================================================================
/*
*/
namespace JoonasFX
{
	class LFO : public GeneratorProcessor
	{
	public:
		LFO(DspFloatType sr=44100);
		~LFO();
		
		enum {
			PORT_FREQ,
			PORT_UNIPOLAR,
			PORT_WAVEFORM,
			PORT_DEPTH
		};
		void printPorts() {
			printf("PORTS\n");
			printf("PORT_FREQ=0\n");
			printf("PORT_UNIPOLAR=1\n");
			printf("PORT_WAVEFORM=2\n");
			printf("PORT_DEPTH=3\n");
		}
		void setPort(int port, DspFloatType v) {
			switch(port)
			{
				case PORT_FREQ: setFreq(v); break;
				case PORT_UNIPOLAR: setUnipolar(v); break;
				case PORT_WAVEFORM: setWaveform((int)v % 5); break;
				case PORT_DEPTH: setDepth(v); break;
			}
		}


		DspFloatType getValue();
		DspFloatType Tick(DspFloatType I=0, DspFloatType A=0, DspFloatType X=0, DspFloatType Y=0) { return getValue(); }

		DspFloatType getValueAndAdvance() {
			DspFloatType out = getValue();
			mReadIndex = fmod(mReadIndex+1,1024);
			return out;
		}
		void setFreq(const DspFloatType& freq);
		void setUnipolar(const bool& isUnipolar);
		void setWaveform(const int& waveform);
		void setDepth(DspFloatType f) { depth = f; }

		void advanceSamples(const int& numSamples);
		void advanceBlock();
		DspFloatType linearInterp(const DspFloatType& y0, const DspFloatType& yp1, const DspFloatType& frac);

		DspFloatType getPhase() { return mReadIndex; }
		void  setPhase(DspFloatType p) { mReadIndex = p/360.0f; }

		enum Waveforms
		{
			sine,
			rectSine,
			saw,
			tri, 
			square
		};

	private:
		DspFloatType 	mReadIndex = 0.f;
		int 	mWaveform = 0;
		DspFloatType 	mInc = 0.f;
		bool 	mUnipolar = true;
		DspFloatType   depth = 1;

		DspFloatType mSampleRate = 44100.f;
		DspFloatType mSamplesPerBlock = 512.f;

		// Wavetables
		DspFloatType mSinArray[1024] = { 0.f };
		DspFloatType mRectSinArray[1024] = { 0.f };
		DspFloatType mSawtoothArray[1024] = { 0.f };
		DspFloatType mTriangleArray[1024] = { 0.f };
		DspFloatType mSquareArray[1024] = { 0.f };
	};


	//==============================================================================
	inline LFO::LFO(DspFloatType sr) : GeneratorProcessor()
	{
		mSampleRate =sr;
		// Triangle wave
		// Functions calculated from coefficients m and b
		// y = mx + b
		// Rising edge 1
		DspFloatType mt1 = 1.f / 256.f;
		DspFloatType bt1 = 0.f;

		// Rising edge 2
		DspFloatType mt2 =  1.f / 256.f;
		DspFloatType bt2 = -1.f;

		// Falling edge
		DspFloatType mtf2 = -2.f / 512.f;
		DspFloatType btf2  = 1.f;

		// Sawtooth
		// Rising edge 1
		DspFloatType ms1 = 1.f / 512.f;
		DspFloatType bs1 = 0.f;

		// Risign edge 2
		DspFloatType ms2 =  1.f / 512.f;
		DspFloatType bs2 = -1.f;


		for (auto i = 0; i < 1024; ++i)
		{
			// Sin
			mSinArray[i] = sin((static_cast<DspFloatType> (i) / 1024.f) * (2 * M_PI));
			mRectSinArray[i] = 2 * abs(mSinArray[i]) - 1;

			// Triangle
			if (i < 256)
				mTriangleArray[i] = mt1 * i + bt1;
			else if (i >= 256 && i < 768)
				mTriangleArray[i] = mtf2 * (i - 256) + btf2;
			else
				mTriangleArray[i] = mt2 * (i - 768) + bt2;

			// Sawtooth
			mSawtoothArray[i] = i < 512 ? ms1 * i + bs1 : ms2 * (i - 511) + bs2;

			// Square
			mSquareArray[i] = i < 512 ? 1.f : -1.f;
		}		
	}

	inline LFO::~LFO()
	{
		// Empty destructor
	}


	inline DspFloatType LFO::getValue()
	{
		DspFloatType LFOVal = 0.f;
		int   readIndex = floor(mReadIndex);
		DspFloatType frac = mReadIndex - readIndex;
		int   readIndexNext = readIndex + 1 > 1023 ? 0 : readIndex + 1;

		switch (mWaveform)
		{
		case Waveforms::sine:
			LFOVal = linearInterp(mSinArray[readIndex], mSinArray[readIndexNext], frac);
			break;
		case Waveforms::rectSine:
			LFOVal = linearInterp(mRectSinArray[readIndex], mRectSinArray[readIndexNext], frac);
			break;
		case Waveforms::saw:
			LFOVal = linearInterp(mSawtoothArray[readIndex], mSawtoothArray[readIndexNext], frac);
			break;
		case Waveforms::tri:
			LFOVal = linearInterp(mTriangleArray[readIndex], mTriangleArray[readIndexNext], frac);
			break;
		case Waveforms::square:
			LFOVal = linearInterp(mSquareArray[readIndex], mSquareArray[readIndexNext], frac);
			break;
		default:
			LFOVal = linearInterp(mSinArray[readIndex], mSinArray[readIndexNext], frac);
			break;
		}

		if (mUnipolar)
		{
			LFOVal /= 2;
			LFOVal += 0.5;

			if (LFOVal < 0 || LFOVal > 1)
				return 0.f;
		}

		return depth*LFOVal;
	}

	inline void LFO::setFreq(const DspFloatType & freq)
	{
		mInc = 1024.f * (freq / mSampleRate);
	}

	inline void LFO::setUnipolar(const bool & isUnipolar)
	{
		mUnipolar = isUnipolar;
	}

	inline void LFO::setWaveform(const int & waveform)
	{
		mWaveform = waveform;
	}

	void LFO::advanceSamples(const int & numSamples)
	{
		mReadIndex += (mInc * numSamples);
		if (mReadIndex > 1024)
			mReadIndex = mReadIndex - 1024;
	}

	inline void LFO::advanceBlock()
	{
		mReadIndex += (mInc * mSamplesPerBlock);
		if (mReadIndex > 1024)
			mReadIndex = mReadIndex - 1024;
	}

	inline DspFloatType LFO::linearInterp(const DspFloatType & y0, const DspFloatType & yp1, const DspFloatType & frac)
	{
		return y0 * (1 - frac) + yp1  * frac;
	}

}