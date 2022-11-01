/*
  ==============================================================================

    ModMatrix.h
    Created: 10 May 2019 1:22:18am
    Author:  quasa

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Maximilian.h"


//==============================================================================
/*
*/
class ModMatrix    : public Component
{
public:
    ModMatrix();
    ~ModMatrix();

    void paint (Graphics&) override;
    void resized() override;

	void ModMatrix::getLfoParams(float* lfoOsc, float* lfoRateValue, float* lfoAmountValue);
	double ModMatrix::setLfo();


private:

	double lfo1;
	maxiOsc oscLFO;
	int lfoOscType;
	float lfoRate;
	float lfoAmount;

//	std::unique_ptr<AudioProcessorValueTreeState::> amountValue;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModMatrix)
};
