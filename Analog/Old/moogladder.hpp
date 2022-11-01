/*
MoogLadder Filter class based on an implementation by Aaron Krajeski
This code is Unlicensed (i.e. public domain)
Aaron Krajeski has stated in an email: "That work is under no copyright. You may use it however you might like."
Source: http://song-swap.com/MUMT618/aaron/Presentation/demo.html
*/

// moogladder.h: header file for defining the moog ladder filter

// Thermal voltage (26 milliwats at room temperature)

#include <cmath>

namespace Analog::Flters::Moog::MoogLadder
{
	#define VT 0.026
	class MoogLadder{

	public:
		// Constructor
		MoogLadder();
		
		// Constructor specifying a sample rate
		MoogLadder(float sampleRate);
		
		// Set the sample rate
		void setSampleRate(float rate);
		
		// Set the cutoff frequency 
		void setFrequency(float cutoff);
		
		// Set the filter Q
		void setQ(float resonance);
		
		void setDrive(float drive);
		
		// Reset previous history of filter
		void reset();
		
		// Calculate the next sample of output, changing the envelope
		// state as needed
		float process(float input); 
		
		// Destructor
		~MoogLadder();

	private:

		// State variables, not accessible to the outside world
		float sampleRate_;	// Filter sample rate
		float cutoff_;		// Filter cutoff frequency
		float resonance_;	// Filter resonance
		double drive_;	// Filter drive.
		double gComp_;	// Compensation factor, used to decide how much of the input signal is added into the feedback loop.
		
		double state[5];// Array for holding the output of each stage of the ladder
		double delay[5];// Array for holding the delayed components
		double wc;		// The angular frequency of the cutoff.
		double g;		// A derived parameter for the cutoff frequency
		double gRes;	// A similar derived parameter for resonance.

		
	};


	// Constructor
	MoogLadder::MoogLadder() : MoogLadder(44100.0) {}

	// Constructor specifying a sample rate
	MoogLadder::MoogLadder(float sampleRate)
	{
		// Set the samplerate and inital values for drive, compensation, cutoff and q/resonance
		setSampleRate(sampleRate);
		for(int i = 0; i < 5; i++){
			state[i] = 0;
			delay[i] = 0;
		}
		
		drive_ = 1.0;
		gComp_ = 0.5;
		
		setFrequency(1000.0f);
		setQ(0.10f);
	}
		
	// Set the sample rate, used for all calculations
	void MoogLadder::setSampleRate(float rate)
	{
		sampleRate_ = rate;	
	}

	// Set the cutoff frequency 
	void MoogLadder::setFrequency(float c)
	{
		//Set the filter cutoff
		cutoff_ = c;
		
		// Calculate angular frequency
		wc = 2 * M_PI * cutoff_ / sampleRate_;
		
		// Calculate Huovilainen derived g parameter controlled by cutoff
		g = 0.9892 * wc - 0.4342 * pow(wc, 2) + 0.1381 * pow(wc, 3) - 0.0202 * pow(wc, 4);
	}
		
	// Set the Q and recalculate the coefficients
	void MoogLadder::setQ(float resonance)
	{
		// Set the filter resonance
		resonance_ = resonance;
		// Calculate derived resonance value
		gRes = resonance_ * (1.0029 + 0.0526 * wc - 0.926 * pow(wc, 2) + 0.0218 * pow(wc, 3));
	}

	// Set the sample rate, used for all calculations
	void MoogLadder::setDrive(float drive)
	{
		drive_ = drive;	
	}
		
	// Calculate the next sample of output
	float MoogLadder::process(float input)
	{
		//Next input to the filter is a combination of the last output and current sample, 
		// scaled by the resonance, filter drive and compensation value
		state[0] = tanh(drive_ * (input - 4 * gRes * (state[4] - gComp_ * input)));
		
		// Loop through each pole of the ladder filter
		for(int i = 0; i < 4; i++)
		{
			// Equation implementing the Huovilainen (2006) one pole circuit diagram
			state[i+1] = g * (0.3 / 1.3 * state[i] + 1 / 1.3 * delay[i] - state[i + 1]) + state[i + 1];
			
			// Add output to feedback loop
			delay[i] = state[i];
		}
		
		// Filtered sample is the output of the final pole
		float output = state[4];
		
		return output;
	}

	// Reset the filter poles and feedback state
	void MoogLadder::reset()
	{
		for(int i = 0; i < 5; i++){
			state[i] = 0;
			delay[i] = 0;
		}	
	}
		
	// Destructor
	MoogLadder::~MoogLadder()
	{
		// Nothing to do here
	}
}
#undef VT