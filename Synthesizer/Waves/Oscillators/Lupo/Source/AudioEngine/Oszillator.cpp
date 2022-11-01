/*
  ==============================================================================

    Oszillator.cpp
    Created: 3 Jun 2016 8:50:32pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "Oszillator.h"

Oszillator::Oszillator() {
    this->pitch = 0;
}

Oszillator::Oszillator(double sampleRate) {
    this->sampleRate = sampleRate;
}

Oszillator::~Oszillator() {
    
}

void Oszillator::setPhase(double phase) {
    this->phase = phase;
}

void Oszillator::setPan(float pan) {
	this->pan = pan;
}

float Oszillator::getPan() {
	return pan;
}


void Oszillator::setVolume(float volume) {
    this->volume = volume;
}

float Oszillator::getVolume() {
    return volume;
}

void Oszillator::setFrequency(double frequency) {
    this->frequency = frequency;
}

float Oszillator::getFrequency() {
	return frequency;
}

void Oszillator::setPitch(int pitch) {
    this->pitch = pitch;
}

int Oszillator::getPitch() {
    return this->pitch;
}

void Oszillator::setPitchBend(float bend) {
    this->pitchBend = bend;
}

float Oszillator::getPitchBend() const {
    return this->pitchBend;
}

void Oszillator::setMode(OscMode mode) {
    this->mode = mode;
}

void Oszillator::setMode(float mode) {
	if (mode == 0.0f) {
		this->mode = OscMode::SAW;
	}
	else if (mode == 1.0f) {
		this->mode = OscMode::PULSE;
	}
	else if (mode == 2.0f) {
		this->mode = OscMode::SINE;
	}
	else if (mode == 3.0f) {
		this->mode = OscMode::NOISE;
	}
	else if (mode == 4.0f) {
		this->mode = OscMode::SAMPLE;
	}

}

void Oszillator::setPitchMod(float mod) {
	this->pitchMod = mod;
}

void Oszillator::setSync(bool sync) {
    this->sync = sync;
}

bool Oszillator::isSync() {
    return sync;
}

void Oszillator::setSlave(Oszillator *slave) {
    this->slave = slave;
}
