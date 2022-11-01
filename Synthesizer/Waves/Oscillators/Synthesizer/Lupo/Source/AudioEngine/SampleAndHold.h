/*
  ==============================================================================

	SampleAndHold.h
	Created: 3 Jun 2016 9:05:43pm
	Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef SampleAndHold_H_INCLUDED
#define SampleAndHold_H_INCLUDED

#include "Oszillator.h"
#include "Modulator.h"
#include "../JuceLibraryCode/JuceHeader.h"

class SampleAndHold : public Oszillator, public Modulator {

public:

	SampleAndHold(double sampleRate, int buffersize);
	~SampleAndHold();

	virtual float process() override;
	virtual float getOutput() override;
	virtual void setFine(float fine) override;
	virtual float getFine() const override;

	virtual void reset() override;


private:
	juce::ScopedPointer<juce::Random> random;
	float fine;
	int currentSample = 0;
	int buffersize = 0;
	float value = 0;
	float lastValue = 0;
	juce::int64 lastTime = 0;
	juce::int64 currentTime = 0;

};



#endif  // SampleAndHold_H_INCLUDED
