// ECS7012P Music and Audio Programming
// School of Electronic Engineering and Computer Science
// Queen Mary University of London
// Spring 2020

// sawtooth.cpp: file for implementing the sawtooth oscillator class

#include <cmath>
#include "sawtooth.h"

// Default constructor: call the specific constructor with a default value
Sawtooth::Sawtooth() : Sawtooth(44100.0) {}

// Constructor taking a sample rate
// Can also use initialisation lists instead of setting 
// variables inside the function
Sawtooth::Sawtooth(float sampleRate) {
	setSampleRate(sampleRate);
	frequency_ = 440.0;
	phase_ = 0;
} 

// Set the sample rate
void Sawtooth::setSampleRate(float rate) {
	sampleRate_ = rate;
}

// Set the oscillator frequency
void Sawtooth::setFrequency(float f) {
	frequency_ = f;
}

// Get the oscillator frequency
float Sawtooth::frequency() {
	return frequency_;
}			
	
// Get the next sample and update the phase
float Sawtooth::nextSample() {
	// Increment and wrap the phase
	phase_ += frequency_ / sampleRate_;
	while(phase_ >= 1.0)
		phase_ -= 1.0;
	
	// Scale a phase range of 0-1 to an output of -1 to 1
	return phase_ * 2.0 - 1.0;
}	
	
// Destructor
Sawtooth::~Sawtooth() {
	// Nothing to do here
}			
