/*
  ==============================================================================

    Sawtooth.cpp
    Created: 3 Jun 2016 9:02:07pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "Sawtooth.h"
#include "../Stk.h"
#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>

using namespace std;

Sawtooth::Sawtooth(float sampleRate, int buffersize) : Oszillator(sampleRate) {
	this->bufferSIze = buffersize;
	this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    this->p = 0.0f;      //current position
    this->dp = 1.0f;     //change in postion per sample
    this->leak = 0.995f; //leaky integrator
    this->pmax = 0.5f * sampleRate / (frequency + this->fine);
    this->dc = -0.498f / this->pmax;
    this->lastValue = 0;
	this->saw = 0;
	for (int i = 0; i < 8; i++) {
		blitsaw[i] = new stk::BlitSaw();

	}
}

Sawtooth::~Sawtooth()
{
	for (int i = 0; i < 8; i++) {
		delete blitsaw[i];
	}
}

float Sawtooth::getOutput() {
    return this->saw;
}

void Sawtooth::reset() {
	for (int i = 0; i < 8; i++) {
		blitsaw[i]->reset();
	}
}


void Sawtooth::setSpread(float spread)
{
	this->spread = spread;
}



void Sawtooth::setFine(float fine) {
	this->fine = fine;    
	setFrequency(frequency);
}

float Sawtooth::getFine() const {
    return this->fine;
}
