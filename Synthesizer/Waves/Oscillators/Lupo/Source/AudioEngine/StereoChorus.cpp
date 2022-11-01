/*
  ==============================================================================

	StereoChorus.cpp
	Created: 4 Dec 2016 11:19:25am
	Author:  Matthias Pueski

  ==============================================================================
*/

#include "StereoChorus.h"
#include "FractionalDelayBuffer.h"
#include "Sine.h"

StereoChorus::StereoChorus(float sampleRate, int bufferSize) {
	this->sampleRate = sampleRate;
	this->bufferSize = bufferSize;

	leftBuffer = new FractionalDelayBuffer();
	rightBuffer = new FractionalDelayBuffer();

	leftOsc = new Sine(sampleRate, bufferSize);
	rightOsc = new Sine(sampleRate, bufferSize);

	leftBuffer->setBufferSize((int)sampleRate / 10);
	rightBuffer->setBufferSize((int)sampleRate / 10);
	leftOsc->setFrequency(0.5);
	rightOsc->setFrequency(0.6);
}

StereoChorus::~StereoChorus() {
	delete leftBuffer;
	delete rightBuffer;
	delete leftOsc;
	delete rightOsc;
}

void StereoChorus::processStereo(float * left, float * right, const int numSamples) {
	if (this->enabled) {

		for (int i = 0; i < bufferSize; i++) {

			float leftMod = (leftOsc->getOutput() + 1.01) * modulation * 100;
			float rightMod = (leftOsc->getOutput() + 1.01) * modulation * 100;

			leftDelayTime = (delay * 200) + leftMod + .002;
			rightDelayTime = (delay * 220) + rightMod + .0015;

			float l_xn = left[i];
			float r_xn = right[i];

			float l_combined;
			float r_combined;

			float l_yn;
			float r_yn;

			l_yn = leftBuffer->getSample(leftDelayTime);
			r_yn = rightBuffer->getSample(rightDelayTime);

			l_combined = l_xn + r_yn * feedback;
			r_combined = r_xn + l_yn * feedback;


			leftBuffer->addSample(l_combined);
			rightBuffer->addSample(r_combined);

			left[i] = (l_xn * (1 - mix) + l_yn * mix);
			right[i] = (r_xn * (1 - mix) + r_yn * mix);

			leftOsc->process();
			rightOsc->process();
		}
	
	}
}

