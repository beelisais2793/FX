//
//  MultimodeOscillator.cpp
//  Trio
//
//  Created by Matthias Pueski on 20.11.16.
//
//

#include "MultimodeOscillator.h"
#include "Sine.h"
#include "WhiteNoise.h"
#include "Sawtooth.h"
#include "Pulse.h"
#include "SampleAndHold.h"
#include "../DummyModulator.h"

MultimodeOscillator::MultimodeOscillator(float sampleRate,int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 0.0f;
    this->fine = 0.0f;
	this->modAmount = 0.0f;
    this->sine = new Sine(sampleRate, buffersize);
    this->saw = new Sawtooth(sampleRate, buffersize);
    this->pulse = new Pulse(sampleRate, buffersize);
    this->noise = new WhiteNoise(sampleRate, buffersize);
	this->sah = new SampleAndHold(sampleRate, buffersize);
	this->mode = OscMode::SAW;
    this->modulator = nullptr;
	this->pan = 0.0f;
}

MultimodeOscillator::~MultimodeOscillator() {
    delete sine;
    delete saw;
    delete pulse;
    delete noise;
	delete sah;
}

void MultimodeOscillator::setFrequency(double frequency) {
	this->frequency = frequency;
	this->saw->setFrequency(this->frequency + this->fine);
    this->sine->setFrequency(this->frequency + this->fine);
    this->pulse->setFrequency(this->frequency + this->fine);
	this->sah->setFrequency(this->frequency + this->fine);
}

void MultimodeOscillator::setVolume(float volume) {
    this->saw->setVolume(volume);
    this->sine->setVolume(volume);
    this->pulse->setVolume(volume);
    this->noise->setVolume(volume);
	this->sah->setVolume(volume);
}

void MultimodeOscillator::setModulator(Modulator* mod) {
    this->modulator = mod;
}


float MultimodeOscillator::getOutput() {
    if (this->mode == SAW) {
        return this->saw->getOutput();
    }
    else if (this->mode == SINE) {
        return this->sine->getOutput();
    }
    else if (this->mode == PULSE) {
        return this->pulse->getOutput();
    }
    else if(this->mode == NOISE) {
        return this->noise->getOutput();
    }
	else if (this->mode == SAMPLE) {
		return this->sah->getOutput();
	}
    else {
        return 0;
    }
}

float MultimodeOscillator::process() {

    if (!enabled) {
        return 0;
    }

	float mod = 0;

	if (modulator != nullptr) {
		mod = modulator->getOutput() * modulator->getModAmount();
	}

    if (this->mode == SAW) {
		saw->setFrequency(mod + this->frequency + this->fine);
        return this->saw->process();
    }
    else if (this->mode == SINE) {
		sine->setFrequency(mod + this->frequency + this->fine);
        return this->sine->process();
    }
    else if (this->mode == PULSE) {
		pulse->setFrequency(mod + this->frequency + this->fine);			
        return this->pulse->process();
    }
    else if(this->mode == NOISE) {
        return this->noise->process();
    }
	else if (this->mode == SAMPLE) {
		return this->sah->process();
	}

    else {
        return 0;
    }
    
}

void MultimodeOscillator::reset() {
    this->saw->reset();
    this->pulse->reset();
    this->sine->reset();
}

void MultimodeOscillator::setFine(float fine) {
    this->fine = fine;
    saw->setFine(fine);
    sine->setFine(fine);
    pulse->setFine(fine);
}

float MultimodeOscillator::getFine() const {
    return this->fine;
}

void MultimodeOscillator::setSlave(Oszillator *slave) {
    this->saw->setSlave(slave);
}

void MultimodeOscillator::setSync(bool sync) {
    this->saw->setSync(sync);
}

void MultimodeOscillator::setSpread(float spread)
{
	this->saw->setSpread(spread);
}
