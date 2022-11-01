// ECS7012P Music and Audio Programming
// School of Electronic Engineering and Computer Science
// Queen Mary University of London
// Spring 2020

// sawtooth.h: header file for sawtooth oscillator class

class Sawtooth {
public:
	Sawtooth();					// Default constructor
	Sawtooth(float sampleRate); // Constructor with argument
	
	void setSampleRate(float rate); // Set the sample rate
	void setFrequency(float f);	// Set the oscillator frequency
	float frequency();			// Get the oscillator frequency
	
	float nextSample();			// Get the next sample and update the phase
	
	~Sawtooth();				// Destructor

private:
	float sampleRate_;			// Sample rate of the audio
	float frequency_;			// Frequency of the oscillator
	float phase_;				// Phase of the oscillator
};
