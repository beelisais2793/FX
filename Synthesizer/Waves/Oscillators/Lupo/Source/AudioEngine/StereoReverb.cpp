/*
  ==============================================================================

    StereoReverb.cpp
    Created: 4 Dec 2016 4:15:58pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "StereoReverb.h"

using juce::Reverb;

StereoReverb::StereoReverb() {
	this->reverb = new Reverb();
	Reverb::Parameters params =  reverb->getParameters();
	params.damping = 0.0f;
	params.roomSize = 0.0f;
	params.damping = 0.0f;
	params.wetLevel = 0.0f;
	params.dryLevel = 1.0f;
	params.width = 0.0f;
	params.freezeMode = 0.0f;
	reverb->setParameters(params);
}

StereoReverb::~StereoReverb() {
    reverb = nullptr;
}

void StereoReverb::processStereo(float *const left, float *const right, const int numSamples) {
    if (this->enabled) {
        reverb->processStereo(left, right, numSamples);
    }
}

void StereoReverb::setParameters (const juce::Reverb::Parameters& newParams) {
    reverb->setParameters(newParams);
}

const juce::Reverb::Parameters& StereoReverb::getParameters() {
    return reverb->getParameters();
}

void StereoReverb::reset() {
    reverb->reset();
}
