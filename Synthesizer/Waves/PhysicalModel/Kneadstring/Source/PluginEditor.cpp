/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
StringSynthesisKarplusAudioProcessorEditor::StringSynthesisKarplusAudioProcessorEditor (StringSynthesisKarplusAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    m_sliderSize = 100;
    m_sliderY = 100;
    m_sliderX = 50;
    m_spacing = 150;
    
    //low pass filter slider
    addAndMakeVisible(m_lopSlider = new Slider("lop slider"));
    m_lopSlider->setSliderStyle(Slider::RotaryVerticalDrag);
    m_lopSlider->setRange(0, 8000, 50);
    m_lopSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    m_lopSlider->addListener(this);
 
    addAndMakeVisible(m_lopLabel = new Label ("lop label", TRANS("Low pass Filter\n")));
    m_lopLabel->setFont(Font (15.00f, Font::plain).withTypefaceStyle("Regular"));
    m_lopLabel->setEditable(false, false, false);
    m_lopLabel->setColour(TextEditor::textColourId, Colours::white);
    m_lopLabel->setColour(TextEditor::textColourId, Colours::black);
    
    //feedback amount slider
    addAndMakeVisible(m_feedbackSlider = new Slider("feedback slider"));
    m_feedbackSlider->setSliderStyle(Slider::RotaryVerticalDrag);
    m_feedbackSlider->setRange(0.2, 0.99, 0.01);
    m_feedbackSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
    m_feedbackSlider->addListener(this);
    
    addAndMakeVisible(m_feedbackLabel = new Label ("feedback label", TRANS("Feedback Amount\n")));
    m_feedbackLabel->setFont(Font (15.00f, Font::plain).withTypefaceStyle("Regular"));
    m_feedbackLabel->setEditable(false, false, false);
    m_feedbackLabel->setColour(TextEditor::textColourId, Colours::white);
    m_feedbackLabel->setColour(TextEditor::textColourId, Colours::black);
    
    
    
    
    //this needs to be at the end
    setSize (600, 400);
}

StringSynthesisKarplusAudioProcessorEditor::~StringSynthesisKarplusAudioProcessorEditor()
{
}

//==============================================================================
void StringSynthesisKarplusAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);

}

void StringSynthesisKarplusAudioProcessorEditor::resized()
{
    //Low pass filter position
    //                         x           y           w             h
    m_lopSlider->setBounds(m_sliderX, m_sliderY, m_sliderSize, m_sliderSize);
    m_lopLabel->setBounds(m_sliderX, m_sliderY + 60, m_sliderSize + 50, m_sliderSize);

    //feedback amount position
    //                                    x                y           w             h
    m_feedbackSlider->setBounds(m_sliderX + m_spacing, m_sliderY, m_sliderSize, m_sliderSize);
    m_feedbackLabel->setBounds(m_sliderX + m_spacing, m_sliderY + 60, m_sliderSize + 50, m_sliderSize);
}

void StringSynthesisKarplusAudioProcessorEditor::sliderValueChanged(Slider* sliderMoved)
{
    if (sliderMoved == m_lopSlider){
        SynthVoice::setLopVal((float)m_lopSlider->getValue());
    }
    if (sliderMoved == m_lopSlider){
        SynthVoice::setFeedbackVal((float)m_feedbackSlider->getValue());
    }

    
}
