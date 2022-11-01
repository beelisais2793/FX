/*
  ==============================================================================

	Main oscillator class, generating wavetable, and synthesizing a waveform.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <complex>
#include "math.h"
#include "TouchHandler.h"


class Oscillator
{
public:

	Oscillator(int fs, int buffSize);
	~Oscillator();

	void generatePolygon();
	void synthesizeWaveform(float* buff);

	void updateFreq(const int& f0);
	void updateOrder(const int& n);
	void updateTeeth(const float& t);
	void updatePhaseOffset(const float& phaseOffset);
	void updateRadius(const float& r);

	int getFreq();
	Range<int> getFreqLimits();
	int getOrder();
	float getTeeth();
	float getPhaseOffset();
	float getRadius();
	int getBufferSize();
	Point<float> getDrawCoords(const int& i); // returns the cartesian coordinates of the sampled geometry for rendering

private:

	void polyBLAMP(float* buff);

    int f0; // frequency
    float n; // order
    float t; // teeth
    float phaseOffset; // phase offset in radians
    float r; // radius (amplitude)

	int fs; // sampling rate
	int buffSize;
    float pi;

    bool isClipped;
    float pMax; // maximum radial amplitude
    dsp::Phase<float> theta;

	float* diff; // second order difference of the synthesized waveform
    float diffBuff; // storing the last sample of the frame, for calculating difference
    float disc; // location of discontinuity, expressed in samples

    float* p; // vector of radial amplitude
    float* pRender; // radial amplitude used for rendering
	std::complex<float>* polygon; // sampled polygon

 	// predefined ranges for oscillator parameters
	Range<int> freqRange;
	Range<int> orderRange;
    Range<float> teethRange;
    Range<float> phaseOffRange;
    Range<float> radRange;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator);
};
