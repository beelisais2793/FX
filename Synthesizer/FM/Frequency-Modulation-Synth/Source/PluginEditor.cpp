/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FmodSynthAudioProcessorEditor::FmodSynthAudioProcessorEditor(FmodSynthAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p), envelopeGui(p, "A"), envelopeB(p, "B"), envelopeC(p, "C"), envelopeD(p, "D"), gainGui(p), lfoGui(p),
	filterGui(p), operatorA(p, "A"), operatorB(p, "B"), operatorC(p, "C"), operatorD(p, "D"), algorithmGui(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (900, 700);

	addAndMakeVisible(&envelopeGui);
	addAndMakeVisible(&envelopeB);
	addAndMakeVisible(&envelopeC);
	addAndMakeVisible(&envelopeD);
	
	addAndMakeVisible(&gainGui);
	addAndMakeVisible(&lfoGui);
	addAndMakeVisible(&filterGui);
	addAndMakeVisible(&operatorA);
	addAndMakeVisible(&operatorB);
	addAndMakeVisible(&operatorC);
	addAndMakeVisible(&operatorD);
	addAndMakeVisible(&algorithmGui);
	
	//addAndMakeVisible(&graphGui);
}

FmodSynthAudioProcessorEditor::~FmodSynthAudioProcessorEditor()
{
}

//==============================================================================
void FmodSynthAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void FmodSynthAudioProcessorEditor::resized()
{
	juce::Rectangle<int> canvas = getLocalBounds();		// sets rectangle to whole UI

	//operatorA.setBounds(0, 400, 216, 256);
	//operatorB.setBounds(216, 400, 216, 256);
	//operatorC.setBounds(432, 400, 216, 256);
	//operatorD.setBounds(648, 400, 216, 256);
	
	operatorA.setBounds(canvas.removeFromLeft(350).removeFromTop(128));
	operatorB.setBounds(0, 128, 350, 128);
	operatorC.setBounds(0, 256, 350, 128);
	operatorD.setBounds(0, 384, 350, 128); 

	envelopeGui.setBounds(350, 0, 300, 128);
	envelopeB.	setBounds(350, 128, 300, 128);
	envelopeC.	setBounds(350, 256, 300, 128);
	envelopeD.	setBounds(350, 384, 300, 128);

	gainGui.setBounds(0, 512, 100, 128);
	lfoGui.setBounds(100, 512, 216, 128);
	algorithmGui.setBounds(316, 512, 100, 128);

	filterGui.setBounds(650, 0, 150, 256);

	//graphGui.setBounds(500, 128, 300, 200);

}

// getParameter is called in both getParameterValue and setParameterValue
AudioProcessorParameter* FmodSynthAudioProcessorEditor::getParameter(const String& paramId)
{    
	/** getAudioProcessor() returns a pointer to the processor that this editor represents.
		This method is here to support legacy code, but it's easier to just use the
		AudioProcessorEditor::processor member variable directly to get this object.
	*/
	if (AudioProcessor* processor = getAudioProcessor())
	{
		// return list of parameters in audio processor
		const OwnedArray<AudioProcessorParameter>& params = processor->getParameters();

		for (int i = 0; i < params.size(); ++i)
		{
			// if cast to audioProcessorParameterWithID succeeds, continue to compare with string iD and return paramId if it is what we're looking for
			if (AudioProcessorParameterWithID* param = dynamic_cast<AudioProcessorParameterWithID*> (params[i]))
			{
				if (param->paramID == paramId)
					return param;			// return pointer to AudioProcessorParameter
			}
		}
	}

	return nullptr;
}

// calls getParameter
float FmodSynthAudioProcessorEditor::getParameterValue(const String& paramId)
{
	if (AudioProcessorParameter* param = getParameter(paramId))
		return param->getValue();

	return 0.0f;
}

// calls getParameter, called in SliderValueChanged
void FmodSynthAudioProcessorEditor::setParameterValue(const String& paramId, float value)
{
	if (AudioProcessorParameter* param = getParameter(paramId))
		param->setValueNotifyingHost(value);
}