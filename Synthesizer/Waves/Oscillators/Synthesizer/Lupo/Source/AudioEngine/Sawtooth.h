/*
  ==============================================================================

    Sawtooth.h
    Created: 3 Jun 2016 9:02:16pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef SAWTOOTH_H_INCLUDED
#define SAWTOOTH_H_INCLUDED


#include "Oszillator.h"
#include "Modulator.h"
#include "../BlitSaw.h"
#include "../JuceLibraryCode/JuceHeader.h"



const float scales[] = {
  1.0f,
  sqrt(1.0 / 2.0),
  sqrt(1.0 / 3.0),
  sqrt(1.0 / 4.0),
  sqrt(1.0 / 5.0),
  sqrt(1.0 / 6.0),
  sqrt(1.0 / 7.0),
  sqrt(1.0 / 8.0),
};

class Sawtooth : public Oszillator, public Modulator {
    
public:

	void setSpread(float spread);

    virtual float getOutput() override;
    virtual void setFine(float fine) override;
    virtual float getFine() const override;
    
	stk::BlitSaw* blitsaw[8];

    Sawtooth(float sampleRate, int buffersize);
	~Sawtooth();
    float fine;
    
    virtual void reset() override;
    
    float p;      //current position
    float dp;     //change in postion per sample
    float pmax;   //maximum position
    float x;      //position in sinc function
    float leak;   //leaky integrator
    float dc;     //dc offset
    float saw;    //output
    float lastValue;
	int currentSample = 0;
	int bufferSIze = 0;
	float spread = 0.1;
	float value = 0;

	inline float process() override {
		if (this->slave != 0 && sync) {
			if (blitsaw[0]->resetFlag) {
				slave->reset();
				blitsaw[0]->resetFlag = false;
			}
		}

		if (spread == 0.0f) {
			saw = blitsaw[0]->tick();
			return saw * volume;
		}
		else {
			value = 0;
			for (int i = 0; i < 8; i++) {
				value += blitsaw[i]->tick();
			}
			//value /= 2;
			saw = value;
			return value * volume;

		}
	}

	inline void setFrequency(double frequency) override {
		if (frequency == 0) {
			frequency = 0.01;
		}
		this->frequency = frequency;
		blitsaw[0]->setFrequency(frequency + this->fine + pitchMod);
		for (int i = 1; i < 8; i++) {
			blitsaw[i]->setFrequency(frequency + this->fine + pitchMod + scales[i - 1] * spread);
		}
	}


};



#endif  // SAWTOOTH_H_INCLUDED
