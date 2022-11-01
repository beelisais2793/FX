/*
  ==============================================================================

    ModMatrix.cpp
    Created: 10 May 2019 1:22:18am
    Author:  quasa

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModMatrix.h"

//==============================================================================
ModMatrix::ModMatrix()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

ModMatrix::~ModMatrix()
{
}

void ModMatrix::getLfoParams(float* lfoOsc, float* lfoRateValue, float* lfoAmountValue)
{
	lfoOscType = *lfoOsc;
	lfoRate = *lfoRateValue;
	lfoAmount = *lfoAmountValue;
}

double ModMatrix::setLfo()
{
	double lfo;
	switch (lfoOscType)
	{
	case 0:
		lfo = oscLFO.sinewave(lfoRate) * lfoAmount;
		break;
	case 1:
		lfo = oscLFO.saw(lfoRate) * lfoAmount;
		break;
	case 2:
		lfo = oscLFO.sawInv(lfoRate) * lfoAmount;
		break;
	case 3:
		lfo = oscLFO.square(lfoRate) * lfoAmount;
		break;
	case 4:
		lfo = oscLFO.triangle(lfoRate) * lfoAmount;
		break;
	default:
		lfo = oscLFO.sinewave(lfoRate) * lfoAmount;
		break;
	}
	return lfo;
}

















void ModMatrix::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("ModMatrix", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void ModMatrix::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
