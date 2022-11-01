/*
  ==============================================================================

	Envelope generator.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Envelope
{
	public:
        typedef enum env{AR, ADSR} env;

        Envelope();
		Envelope(Envelope::env type);
		~Envelope();

		void trigger();
		float envelopeAR(); // AR envelope
		float envelopeADSR(); // ADSR envelope

		void process(AudioBuffer<float>& buffer); // processses an audio buffer based on the envelope type

		void setAttackTime(const int& a);
		void setPeak(const float& p);
		void setDecayTime(const int& d);
        void setSustainLevel(const float& s);
		void setReleaseTime(const int& r);
		void setSamplingRate(const int& fs);
    
        float* getAmplitude(); // returns a pointer to the envelope's amplitude

	private:
		float amplitude;
		bool trig;
		int fs;
		float aMin;
		bool noteOn;

		// envelope parameters
		env envelopeType; 
		int attackTime;
		float peak;
		int decayTime;
		float sustainLevel;
		int releaseTime;

		// phase conditions
		bool attack;
    	bool decay;
    	bool release;
		
		// amplitude increment/decrement
    	float attDelta;
    	float decDelta; 
    	float relDelta;
	
};
