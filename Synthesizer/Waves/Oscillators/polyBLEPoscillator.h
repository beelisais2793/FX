/***** polyBLEPoscillator.h *****/
#include <cmath>

#define SINE 0
#define SAW 1
#define SQUARE 2

class PolyBLEPOscillator {
public:
	PolyBLEPOscillator();							// Default constructor
	PolyBLEPOscillator(float sampleRate, int mode); // Constructor with argument
	
	void setSampleRate(float rate); 	// Set the sample rate
	void setFrequency(float f);			// Set the oscillator frequency
	void setMode(int mode);				// Set the oscillator mode
	float frequency();					// Get the oscillator frequency
	
	float nextSample();					// Get the next sample and update the phase
	double poly_blep(double t);			// Calculate the PolyBLEPs
	
	~PolyBLEPOscillator();				// Destructor

private:
	float sampleRate_;			// Sample rate of the audio
	float frequency_;			// Frequency of the oscillator
	float phase_;				// Phase of the oscillator
	int oscMode_;				// Mode of the oscillator e.g. Sine, Saw, Square
};