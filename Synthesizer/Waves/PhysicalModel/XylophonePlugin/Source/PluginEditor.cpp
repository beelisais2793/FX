/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XylophoneModelAudioProcessorEditor::XylophoneModelAudioProcessorEditor (XylophoneModelAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    for(int i = 0;i<activeButtons; ++i){
        addAndMakeVisible(keys[i]);
        keys[i].setButtonText(keyNames[i]);
        keys[i].onClick = [this,i]{
            audioProcessor.handleButtonPress(i);
        };
    }
    
    
    setSize (800, 600);
}

XylophoneModelAudioProcessorEditor::~XylophoneModelAudioProcessorEditor()
{
}

//==============================================================================
void XylophoneModelAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("", getLocalBounds(), juce::Justification::centred, 1);
}

void XylophoneModelAudioProcessorEditor::resized()
{
    /*int keyGap = 50;
    int keyWidth = 40;
    int keyHeight = 80;
    int startX = 50;
    int startY = 50;
    
    for(int i = 0;i<numKeys; ++i){
        keys[i].setBounds(startX + i*keyGap, startY, keyWidth, keyHeight);
    }*/
    setRingLayout(radius, centreX, centreY, activeButtons, keyWidth, keyHeight);
}

void XylophoneModelAudioProcessorEditor::setRingLayout(double radius, float Xcentre, float Ycentre, int numButtons, int keyWidth, int keyHeight){
    for(int i = 0;i<numButtons;++i){
        float currentX = Xcentre + radius*cos(i*(2*juce::double_Pi/(double)numButtons)) - keyWidth/2.0;
        float currentY = Ycentre - radius*sin(i*(2*juce::double_Pi/(double)numButtons)) - keyHeight/2.0;
        keys[i].setBounds(currentX, currentY, keyWidth, keyHeight);
    }
}
 
