// polyBLEPoscillator.cpp: file for implementing the polyblep oscillator class
// This code is based on the polyBLEP example provided by http://metafunction.co.uk/all-about-digital-oscillators-part-2-blits-bleps/

#include <cmath>
#include "polyBLEPoscillator.h"

#define SINE 0
#define SAW 1
#define SQUARE 2
#define twoPI M_PI*2

// Default constructor: call the specific constructor with a default value
PolyBLEPOscillator::PolyBLEPOscillator() : PolyBLEPOscillator(44100.0, SAW) {}

// Constructor taking a sample rate
// Can also use initialisation lists instead of setting 
// variables inside the function
PolyBLEPOscillator::PolyBLEPOscillator(float sampleRate, int mode) {
	setSampleRate(sampleRate);
	frequency_ = 440.0;
	phase_ = 0;
	oscMode_ = mode;
} 

// Set the sample rate
void PolyBLEPOscillator::setSampleRate(float rate) {
	sampleRate_ = rate;
}

// Set the oscillator frequency
void PolyBLEPOscillator::setFrequency(float f) {
	frequency_ = f;
}

void PolyBLEPOscillator::setMode(int mode){
	oscMode_ = mode;
}

// Get the oscillator frequency
float PolyBLEPOscillator::frequency() {
	return frequency_;
}			
	
// Get the next sample and update the phase
float PolyBLEPOscillator::nextSample() {
	
	if(oscMode_ == SINE){
		//Increment the phase
		phase_ += (M_PI*2)*frequency_ / sampleRate_;
		
		//If the phase has overran, wrap back around
		while (phase_ > 2*M_PI){
			phase_-= 2*M_PI;
		}
		
		//Sin wave does not have discontinuities so does not need polyBlep
    	return sin(phase_);
	}
	
	else if(oscMode_ == SAW){
		//Increment the phase
		phase_ += frequency_ / sampleRate_;
		
		//If the phase has overran, wrap back around
		while (phase_ > 1.0){
			phase_-= 1.0;
		}
		
		//Initalise the output
		double sample = 0.0;
		//Define half of the phase
    	double t = phase_ / (M_PI*2);
    	
    	//Calculate sample for trivial waveform, scale phase range of 0-1 to an output of -1 to 1
		sample = (phase_ * 2.0 - 1.0);
		//Subtract the polyBlep Residual to smooth discontinuities
		sample -= poly_blep(t);
		return sample;
	}
	
	else if(oscMode_ == SQUARE){
		
		//Increment the phase
		phase_ += (M_PI*2)*frequency_ / sampleRate_;
		
		//If the phase has overran, wrap back around
		while (phase_ > 2*M_PI){
			phase_-= 2*M_PI;
		}
		
		//Initalise the output
		double sample = 0.0;
		//Define half phase
    	double t = phase_ / (M_PI*2);
    	
    	if (phase_ < M_PI){
    		// Upper edge of square wave
    		sample = 1.0; 
    	} 
    	else { 
    		// Lower edge of square wave
    		sample = -1.0;
    	}
    	// Add Poly BLEP on top edge
    	sample += poly_blep(t); 
    	// Subtract Poly BLEP on bottom edge
    	sample -= poly_blep(fmod(t + 0.5, 1.0));
    	return sample;
	}
	
	else{
		return 0.0;
	}
}

double PolyBLEPOscillator::poly_blep(double t){
	
	//Calculate dt for checking discontinuities
	double dt = (frequency_ / sampleRate_)/(M_PI*2);
	
	// t-t^2/2 +1/2
	// 0 < t <= 1
	// discontinuities between 0 & 1
	if (t < dt) 
	{
		t /= dt;
		return t + t - t * t - 1.0;
	}

	// t^2/2 +t +1/2
	// -1 <= t <= 0
	// discontinuities between -1 & 0
	else if (t > 1.0 - dt) 
	{
		t = (t - 1.0) / dt;
		return t * t + t + t + 1.0;
	}

	// no discontinuities 
	// 0 otherwise
	else{ 
		return 0.0;
	}
}

	
// Destructor
PolyBLEPOscillator::~PolyBLEPOscillator() {
	// Nothing to do here
}			
