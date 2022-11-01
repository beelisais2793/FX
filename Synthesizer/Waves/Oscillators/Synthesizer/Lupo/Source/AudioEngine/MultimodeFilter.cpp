//
//  MultimodePassFilter.cpp
//  Trio
//
//  Created by Matthias Pueski on 16.11.16.
//
//


#include "MultimodeFilter.h"
#include "ADSR.h"

MultimodeFilter::MultimodeFilter() {

    this->lowPassLeftStage1 = new LowPassFilter();
    this->lowPassRightStage1 = new LowPassFilter();

	this->lowPassLeftStage2 = new LowPassFilter();
	this->lowPassRightStage2 = new LowPassFilter();

    this->highPassLeft = new HighPassFilter();
    this->highPassRight = new HighPassFilter();
    
    this->mode = LOWPASS;
	this->keyTrack = 0;
}

MultimodeFilter::~MultimodeFilter() {
    this->lowPassLeftStage1 = nullptr;
    this->lowPassRightStage1 = nullptr;
    
	this->lowPassLeftStage2 = nullptr;
	this->lowPassRightStage2 = nullptr;

	this->highPassLeft = nullptr;
    this->highPassRight = nullptr;
}


void MultimodeFilter::setMode(Mode mode) {
    this->mode = mode;
}

void MultimodeFilter::coefficients(float sampleRate, float frequency, float resonance) {
	if (frequency <= 0) {
		frequency = 0.1;
	}
	if (resonance <= 0) {
		resonance = 0.01f;
	}
    this->lowPassLeftStage1->coefficients(sampleRate, frequency , resonance);
    this->lowPassRightStage1->coefficients(sampleRate, frequency, resonance);
	
	this->lowPassLeftStage2->coefficients(sampleRate, frequency + keyTrack * 10, resonance);
	this->lowPassRightStage2->coefficients(sampleRate, frequency + keyTrack * 10, resonance);
	
    this->highPassLeft->coefficients(sampleRate, frequency, resonance);
    this->highPassRight->coefficients(sampleRate, frequency, resonance);
}

void MultimodeFilter::processStereo(float *const left, float *const right, const int numSamples) {
    if (this->enabled) {
        if (this->mode == Mode::LOWPASS) {
            this->lowPassLeftStage1->process(left, 0, numSamples);
            this->lowPassRightStage1->process(right, 0, numSamples);
			this->lowPassLeftStage2->process(left, 0, numSamples);
			this->lowPassRightStage2->process(right, 0, numSamples);
			
		}
        else {
            this->highPassLeft->process(left, 0, numSamples);
            this->highPassRight->process(right, 0, numSamples);
        }
   
    }
}

void MultimodeFilter::processMono(int channel, float * const samples, const int numSamples)
{
	if (this->enabled) {
		if (this->mode == Mode::LOWPASS) {
			this->lowPassLeftStage1->process(samples, 0, numSamples);
			this->lowPassLeftStage2->process(samples, 0, numSamples);


		}
		else {
			this->highPassLeft->process(samples, 0, numSamples);
		}

	}

}

void MultimodeFilter::setModulator(Modulator* mod) {
    this->lowPassLeftStage1->setModulator(mod);
    this->lowPassRightStage1->setModulator(mod);

	this->lowPassLeftStage2->setModulator(mod);
	this->lowPassRightStage2->setModulator(mod);

    this->highPassLeft->setModulator(mod);
    this->highPassRight->setModulator(mod);
	this->modulator = mod;
}


void MultimodeFilter::setKeyTrack(int track) {
	this->keyTrack = track;
}