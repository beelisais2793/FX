/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NoiseGeneratorPluginAudioProcessorEditor::NoiseGeneratorPluginAudioProcessorEditor(NoiseGeneratorPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Apple II font from http://www.kreativekorp.com/software/fonts/apple2.shtml
    // this line is important to ensure that the custom font is used
    LookAndFeel::setDefaultLookAndFeel(&oldSchoolLookAndFeel);

    // LABELS
    titleLabel.setText("Noise Generator", dontSendNotification);
    titleLabel.setJustificationType(Justification::centred);
    titleLabel.setFont(Font(18.0f, Font::bold));
    titleLabel.setColour(Label::textColourId, Colours::green);

//    levelLabel.setText("Mix", dontSendNotification);
//    levelLabel.setJustificationType(Justification::centred);
//    levelLabel.setColour(Label::textColourId, Colours::green);
//    levelLabel.setFont(Font(12.0f, Font::bold));

    addAndMakeVisible(&titleLabel);
    //addAndMakeVisible(&levelLabel);

    // BUTTONS
    // attach the value tree to the buttons
    wAttach   = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, WHITE_ID, wButton);
    pAttach   = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, PINK_ID,  pButton);
    bAttach   = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, BROWN_ID, bButton);
    onAttach  = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, STATE_ID, onButton);
    dcAttach  = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, DC_ID,    dcButton);
    avgAttach = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, AVG_ID,   avgButton);
    
    // set formatting
    setupOldSchoolAndFeelColours(oldSchoolLookAndFeel);

    wButton.setButtonText("White");
    pButton.setButtonText("Pink");
    bButton.setButtonText("Brown");
    onButton.setButtonText("ON");
    dcButton.setButtonText("DC");
    avgButton.setButtonText("smooth");

    wButton.onClick   = [this] { updateToggleState(&wButton,   "White"); };
    pButton.onClick   = [this] { updateToggleState(&pButton,   "Pink"); };
    bButton.onClick   = [this] { updateToggleState(&bButton,   "Brown"); };
    onButton.onClick  = [this] { updateToggleState(&onButton,  "On"); };
    dcButton.onClick  = [this] { updateToggleState(&dcButton,  "DC block"); };
    avgButton.onClick = [this] { updateToggleState(&avgButton, "Smooth"); };

    // add to radio ID group so that they operate in mutually exclusive mode
    wButton.setRadioGroupId(NoiseButtons);
    pButton.setRadioGroupId(NoiseButtons);
    bButton.setRadioGroupId(NoiseButtons);

    // set formatting
    wButton.setLookAndFeel(&oldSchoolLookAndFeel);
    pButton.setLookAndFeel(&oldSchoolLookAndFeel);
    bButton.setLookAndFeel(&oldSchoolLookAndFeel);
    onButton.setLookAndFeel(&oldSchoolLookAndFeel);
    dcButton.setLookAndFeel(&oldSchoolLookAndFeel);
    avgButton.setLookAndFeel(&oldSchoolLookAndFeel);

    // Set edges for the noise selection row
    wButton.setConnectedEdges(2);
    pButton.setConnectedEdges(3);
    bButton.setConnectedEdges(1);

    // set the noise buttons to not be toggle switches
    wButton.setClickingTogglesState(true);
    pButton.setClickingTogglesState(true);
    bButton.setClickingTogglesState(true);
    onButton.setClickingTogglesState(true);
    dcButton.setClickingTogglesState(true);
    avgButton.setClickingTogglesState(true);

    addAndMakeVisible(&wButton);
    addAndMakeVisible(&pButton);
    addAndMakeVisible(&bButton);
    addAndMakeVisible(&onButton);
    addAndMakeVisible(&dcButton);
    addAndMakeVisible(&avgButton);

    // SLIDERS
    // attach the value tree to the slider
    levelAttach     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, LEVEL_ID, levelSlider);

    // set formatting
    levelSlider.setSliderStyle(Slider::LinearBar);
    levelSlider.setRange(0.0f, 1.0f);
    levelSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    levelSlider.setLookAndFeel(&oldSchoolLookAndFeel);
    levelSlider.setColour(Slider::backgroundColourId, Colours::black);
    addAndMakeVisible(&levelSlider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // setSize(320, 180); - ORIGINAL
    setSize(320, 220);
}

NoiseGeneratorPluginAudioProcessorEditor::~NoiseGeneratorPluginAudioProcessorEditor()
{
}

//==============================================================================
void NoiseGeneratorPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
}

void NoiseGeneratorPluginAudioProcessorEditor::resized()
{
    titleLabel.setBounds(32, 7, 256, 36);

    wButton.setBounds(30,  50, 85, 45);
    pButton.setBounds(115, 50, 85, 45);
    bButton.setBounds(200, 50, 85, 45);
    
    onButton.setBounds (30,  105, 85, 30);
    dcButton.setBounds (115, 105, 85, 30);
    avgButton.setBounds(200, 105, 85, 30);
    
    levelSlider.setBounds(30, 150, 255, 35);
}

void NoiseGeneratorPluginAudioProcessorEditor::updateToggleState(Button* button, String name)
{
    auto state = button->getToggleState();
    String stateString = state ? "ON" : "OFF";

    Logger::outputDebugString(name + " Button changed to " + stateString);
}

void NoiseGeneratorPluginAudioProcessorEditor::setupOldSchoolAndFeelColours(LookAndFeel& laf)
{
    auto baseColour = Colours::green;

    laf.setColour(Slider::thumbColourId, Colour::greyLevel(0.95f));
    laf.setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
    laf.setColour(Slider::rotarySliderFillColourId, baseColour);
    laf.setColour(Slider::rotarySliderOutlineColourId, Colours::black);
    laf.setColour(Slider::trackColourId, Colours::white);

    laf.setColour(TextButton::buttonColourId, Colours::black);
    laf.setColour(TextButton::textColourOffId, baseColour);

    laf.setColour(TextButton::buttonOnColourId, laf.findColour(TextButton::textColourOffId));
    laf.setColour(TextButton::textColourOnId, laf.findColour(TextButton::buttonColourId));
}
