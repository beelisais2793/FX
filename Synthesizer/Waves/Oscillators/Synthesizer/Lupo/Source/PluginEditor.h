/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MainUI.h"
#include "TrioLookAndFeel.h"

class AttachmentFactory;

//==============================================================================
/**
*/
class LupoAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    LupoAudioProcessorEditor (LupoAudioProcessor&);
    ~LupoAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void saveSettings();
    void loadSettings();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LupoAudioProcessor& processor;
	MainUI* mainUI;
	TrioLookAndFeel tlf;
	
	AttachmentFactory* factory;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LupoAudioProcessorEditor)
};
