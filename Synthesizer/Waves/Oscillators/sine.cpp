// ECS7012P Music and Audio Programming
// School of Electronic Engineering and Computer Science
// Queen Mary University of London
// Spring 2020

// sine.cpp: file for implementing the sine oscillator class

#include <cmath>
#include "sine.h"

// Default constructor: call the specific constructor with a default value
Sine::Sine() : Sine(44100.0) {}

// Constructor taking a sample rate
// Can also use initialisation lists instead of setting 
// variables inside the function
Sine::Sine(float sampleRate) {
	setSampleRate(sampleRate);
	frequency_ = 440.0;
	phase_ = 0;
} 

// Set the sample rate
void Sine::setSampleRate(float rate) {
	sampleRate_ = rate;
}

// Set the oscillator frequency
void Sine::setFrequency(float f) {
	frequency_ = f;
}

// Get the oscillator frequency
float Sine::frequency() {
	return frequency_;
}			
	
// Get the next sample and update the phase
float Sine::nextSample() {
	// Increment and wrap the phase
	phase_ += 2.0 * M_PI * frequency_ / sampleRate_;
	while(phase_ >= 2.0 * M_PI)
		phase_ -= 2.0 * M_PI;
	
	return sinf(phase_);
}	
	
// Destructor
Sine::~Sine() {
	// Nothing to do here
}			
