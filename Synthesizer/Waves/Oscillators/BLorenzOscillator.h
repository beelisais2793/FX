//=============================================================================	
//
// BLORENZOSC.H
//
// (c) 2004 Russell Borogove / www.tinygod.com
//
//=============================================================================	
//
//	Lorenz/Rossler iterative function systems as LFOs
//
//=============================================================================	

#ifndef _BLORENZOSC_H
#define _BLORENZOSC_H

//=============================================================================	
//
// This module defines the classes BLorenzOsc and BRosslerOsc - low frequency
// oscillators suitable for modeling 'analog drift' or other random-but-smooth
// processes. Both classes have identical APIs - you could unify the interface
// with virtual functions easily, but the current implementation is all inline, 
// baby. 
// 
// SetSampleRate():
// Sets the sample rate at which the Iterate() function will be called. Only 
// meaningful in conjunction with the SetFreq() function.
//
// SetFreq():
// Sets the fundamental frequency of the oscillator. The Rossler oscillator 
// should exhibit harmonic peaks at multiples of that frequency; the Lorenz 
// oscillator has a linear frequency-amplitude relation, so SetFreq() will 
// only control the scale of waveform features in a general way.
// 
// Iterate():
// Advances the clock by one sample period and returns the value of the 
// function at the current clock; it should be called once per sample-tick. 
//
// GetCurrent():
// Returns the same value returned by the latest call to Iterate(). Useful
// in cases where one generator modulates multiple destinations, for example.
// 
// GetAlternate():
// Returns a value separate from the current value but correlated with it; 
// these are the X and Y values used for the well-known "butterfly" plots 
// of the Lorenz and Rossler functions. You can use GetAlternate() if you 
// want two separate LFOs which are related in mysterious ways at a low 
// cost - for example, you can fine-tune one audio oscillator with the return 
// from Iterate() and another oscillator with the return from GetAlternate(). 
//
// Both the primary and alternate returns are calibrated to a -1.0 to +1.0 
// range in normal usage. The implementation is discrete, though, so if the 
// sample rate is low or the frequency high, it may occasionally jump outside
// that range -- the user is responsible for clamping if the range is 
// critical.
// 
//=============================================================================	

const float LORENZ_SCALE     = 0.05107f;
const float LORENZ_ALT_SCALE = 0.03679f;

//=============================================================================	
//
// Lorenz function - very broad spectrum noise function with amplitude 
// decreasing with increasing frequency, but tight short-term correlation. 
//
// The scale of waveform features will change somewhat with the set frequency
// and sample rate, but not drastically - it's fairly fractal. In particular, 
// there will not be substantial spectral peaks at multiples of the frequency
// selected by SetFreq(). 
//
//=============================================================================
	
class BLorenzOsc
{
private:
	float	mDX;
	float	mDY;
	float	mDZ;
	float	mDT;
	float	mFreq;
	float	mX;
	float	mY;
	float	mZ;

	float	mA;
	float	mB;
	float	mC;

	float	mRate;

public:
	BLorenzOsc()
	{
		mA = 10.0f; 
		mB = 28.0f;
		mC = 2.666f;
		mDX = mDY = mDZ = 0.0f;
		mX = mY = mZ = 1.0f;
		mFreq = 440.0f;
		SetSampleRate( 44100.0f );
		SetFreq( 440.0f );
	}
	~BLorenzOsc()
	{
	}

	__forceinline void SetSampleRate( float rate )
	{
		mRate = rate;
		mDT = mFreq / rate;
	}

	__forceinline void SetFreq( float freq )
	{
		mFreq = freq;
		mDT = freq / mRate;
	}


	__forceinline float GetCurrent( void )
	{
		return mX * LORENZ_SCALE;
	}

	__forceinline float GetAlternate( void )
	{
		return mY * LORENZ_ALT_SCALE;
	}

	__forceinline float Iterate( void )
	{
		mDX = mA * (mY-mX);
		mDY = mX * (mB-mZ) - mY;
		mDZ = mX * mY - mC * mZ;

		mX += mDX * mDT;
		mY += mDY * mDT;
		mZ += mDZ * mDT;

		return mX * LORENZ_SCALE;
	}
};


//=============================================================================	
//
// Rossler function - broad spectrum noise function with amplitude 
// decreasing with increasing frequency, and distinct harmonic peaks. The 
// peaks should occur at harmonics of the frequency set by SetFreq(). 
//
//=============================================================================
	
const float ROSSLER_SCALE     = 0.05757f;
const float ROSSLER_ALT_SCALE = 0.06028f;

class BRosslerOsc
{
private:
	float	mDX;
	float	mDY;
	float	mDZ;
	float	mDT;
	float	mFreq;
	float	mX;
	float	mY;
	float	mZ;

	float	mA;
	float	mB;
	float	mC;

	float	mRate;

public:
	BRosslerOsc()
	{
		mA = 0.15f; 
		mB = 0.20f;
		mC = 10.0f;
		mDX = mDY = mDZ = 0.0f;
		mX = mY = mZ = 1.0f;
		mFreq = 440.0f;
		SetSampleRate( 44100.0f );
		SetFreq( 440.0f );
	}
	~BRosslerOsc()
	{
	}

	__forceinline void SetSampleRate( float rate )
	{
		mRate = rate;
		mDT = 2.91f * mFreq / rate;
	}

	__forceinline void SetFreq( float freq )
	{
		mFreq = freq;
		mDT = 2.91f * freq / mRate;
	}


	__forceinline float GetCurrent( void )
	{
		return mX * ROSSLER_SCALE;
	}

	__forceinline float GetAlternate( void )
	{
		return mY * ROSSLER_ALT_SCALE;
	}

	__forceinline float Iterate( void )
	{
		mDX = -mY - mZ;
		mDY = mX + mA * mY;
		mDZ = mB + mZ * (mX - mC);

		mX += mDX * mDT;
		mY += mDY * mDT;
		mZ += mDZ * mDT;

		return mX * ROSSLER_SCALE;
	}
};

//=============================================================================	

#endif