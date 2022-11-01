//
//  Pulse.cpp
//  Trio
//
//  Created by Matthias Pueski on 23.11.16.
//
//

#include "Pulse.h"
#include "..\BlitSquare.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

Pulse::Pulse(float sampleRate, int buffersize) : Oszillator(sampleRate) {
    this->volume = 1.0f;
    this->frequency = 440.0f;
    this->fine = 0.0f;
    this->step = (frequency * 2 * M_PI) / sampleRate;
    this->phase = 0;
	blitSquare = new stk::BlitSquare();
}

void Pulse::reset() {
    this->phase = 0;
	blitSquare->reset();
}

float Pulse::getOutput() {
    return this->value;
}

float Pulse::process() {
	return blitSquare->tick() * volume;
}

void Pulse::setFrequency(double frequency) {
    this->frequency = frequency;
	blitSquare->setFrequency(frequency);
    //this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

void Pulse::setFine(float fine) {
    this->fine = fine;
	setFrequency(this->frequency);
    // this->step = ((frequency + (double)this->fine) * 2 * M_PI / sampleRate);
}

float Pulse::getFine() const {
    return this->fine;
}
