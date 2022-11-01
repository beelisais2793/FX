/*
  This code accompanies the textbook:
 
  Digital Audio Effects: Theory, Implementation and Application
  Joshua D. Reiss and Andrew P. McPherson
 
  ---
 
  Phaser: phasing effect using time-varying allpass filters
  See textbook Chapter 4: Filter Effects
 
  Code by Andrew McPherson, Brecht De Man and Joshua Reiss
 
  ---
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "Undenormal.hpp"
#define _USE_MATH_DEFINES


/**
 * This class implements a 1st-order allpass filter used in the phaser effect.
 * Unlike TwoPoleAllpassFilter, it does not extend IIRFilter which is two-pole.
 * Loosely modelled after OnePoleFilter.cpp by dRowAudio.
 */

class OnePoleAllpassFilter
{
public:
    //==============================================================================
	// Create an unititialised filter.
    // This will not perform any filtering until the allpass coefficients have been set.
	OnePoleAllpassFilter() noexcept;
	
	// Destructor.
	~OnePoleAllpassFilter() noexcept;
	
	//Process a number of samples in one go.
    // This is the most effecient method of filtering.
    // Note that the samples passed to it actually get changed.
	void processSamples (float* const samples,
						 const int numSamples) noexcept;
	
	// Process a single sample.
    // Less efficient method but leaves the sample unchanged,
    // returning a filtered copy of it.
    float processSingleSampleRaw (const float sampleToProcess) noexcept;
	
	// Set the centre frequency of the allpass with respect to the sample rate
	void makeAllpass (const double inverseSampleRate,
                      const double centreFrequency) noexcept;
    
private:
    //==============================================================================
	//CriticalSection lock;
	float x1, y1, b0, b1, a1;
	
    //==============================================================================

};


// Constructor: set defaults that do nothing
inline OnePoleAllpassFilter::OnePoleAllpassFilter() noexcept
: x1 (0.0f), y1 (0.0f), b0 (1.0f), b1 (0.0f), a1 (0.0f)
{
}

inline OnePoleAllpassFilter::~OnePoleAllpassFilter() noexcept
{
}

inline float OnePoleAllpassFilter::processSingleSampleRaw (const float sampleToProcess) noexcept
{
    // Process one sample, storing the last input and output
    y1 = (b0 * sampleToProcess) + (b1 * x1) + (a1 * y1);
    x1 = sampleToProcess;
    return y1;
}

void OnePoleAllpassFilter::processSamples (float* const samples,
                                    const int numSamples) noexcept
{
    // Process a buffer of samples all at once
    // make sure sample values are locked
    //const ScopedLock sl (lock);
    
    for (int i = 0; i < numSamples; ++i)
    {
        y1 = (b0 * samples[i]) + (b1 * x1) + (a1 * y1);
        x1 = samples[i];
        samples[i] = y1;
    }
}

// Set the coefficients for a one-pole allpass filter
// according to cutoff frequency relative to sample rate
// The first parameter is inverse because it is a faster
// calculation.
inline void OnePoleAllpassFilter::makeAllpass (const double inverseSampleRate,
                                        const double centreFrequency) noexcept
{
    Undenormal denormals;
    // This code based on calculations by Julius O. Smith:
    // https://ccrma.stanford.edu/~jos/pasp/Classic_Virtual_Analog_Phase.html
    
    // Avoid passing pi/2 to the tan function...
    const double w0 = fmin(centreFrequency * inverseSampleRate, 0.99*M_PI);
    const double tan_half_w0 = tan(0.5 * w0);
    
    //const ScopedLock sl (lock);
    
    b0 = a1 = (float)((1.0 - tan_half_w0) / (1.0 + tan_half_w0));
    b1 = -1.0f;
}


