/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioEngine/LupoSynth.h"
#include "OscillatorPanel.h"
#include "EnvelopePanel.h"
#include "LFOPanel.h"
#include "ReverbPanel.h"
#include "DelayPanel.h"
#include "ChorusPanel.h"
#include "SamplerPanel.h"
#include "MixerChannelPanel.h"
#include "GraphicalEnvelope.h"
#include "PluginProcessor.h"
#include "DistortionPanel.h"
#include "ArpPanel.h"
#include "FilterPanel.h"
#include "ModPanel.h"
class ModMatrixPanel;
class ModMatrixModel;
class Model;
class AttachmentFactory;
class PresetDialog;
class Panel;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainUI  : public Component,
                public ChangeBroadcaster,
                public ChangeListener,
                public AudioProcessorValueTreeState::Listener,
                public Slider::Listener,
                public Button::Listener,
                public ComboBox::Listener
{
public:
    //==============================================================================
    MainUI (LupoAudioProcessor* processor, AttachmentFactory* factory);
    ~MainUI() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void changeListenerCallback(ChangeBroadcaster* source);
	void mouseDown(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;
	void mouseMove(const MouseEvent& event) override;
	void updatePresetList();
	Component* findComponentAtMousePosition(Point<int> mousePos, Component* parent);
	void parameterChanged(const String& parameterID, float newValue) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    // Binary resources:
    static const char* logo_png;
    static const int logo_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	Model* model;
	LupoSynth* synth;
	Point<int> mouseDownPos;
	Point<int> mousePos;
	bool draggingConnection = false;
	LupoAudioProcessor* processor;
	AttachmentFactory* factory;
	vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>>* sliderAttachments;
	PresetDialog* dlg;

	Component* modSource = nullptr;
	Component* modTarget = nullptr;

	ModMatrixModel* matrixModel;


    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Panel> GlassPanel;
    std::unique_ptr<GroupComponent> ModulationGroup;
    std::unique_ptr<GroupComponent> groupComponent;
    std::unique_ptr<GroupComponent> groupComponent3;
    std::unique_ptr<GroupComponent> filterGroup1;
    std::unique_ptr<OscillatorPanel> osc1Panel;
    std::unique_ptr<OscillatorPanel> osc3Panel;
    std::unique_ptr<OscillatorPanel> osc2Panel;
    std::unique_ptr<EnvelopePanel> ampEnvelope;
    std::unique_ptr<Slider> mainVolume;
    std::unique_ptr<Label> volumeLabel;
    std::unique_ptr<GroupComponent> mixerGroiup;
    std::unique_ptr<MixerChannelPanel> ch1Panel;
    std::unique_ptr<MixerChannelPanel> ch2Panel;
    std::unique_ptr<MixerChannelPanel> ch3Panel;
    std::unique_ptr<MixerChannelPanel> ch4Panel;
    std::unique_ptr<OscillatorPanel> osc4Panel;
    std::unique_ptr<GroupComponent> FXGroup;
    std::unique_ptr<ReverbPanel> reverbPanel;
    std::unique_ptr<DelayPanel> delayPanel;
    std::unique_ptr<ChorusPanel> chorusPanel;
    std::unique_ptr<TextButton> presetButton;
    std::unique_ptr<TextButton> saveButton;
    std::unique_ptr<ComboBox> presetCombo;
    std::unique_ptr<Slider> fmSlider;
    std::unique_ptr<GroupComponent> distGroup;
    std::unique_ptr<DistortionPanel> distortionPanel;
    std::unique_ptr<GroupComponent> arpGroup;
    std::unique_ptr<ArpPanel> arpPanel;
    std::unique_ptr<TabbedComponent> modulationTab;
    std::unique_ptr<FilterPanel> filterPanel1;
    std::unique_ptr<GroupComponent> filterGroup2;
    std::unique_ptr<FilterPanel> filterPanel2;
    std::unique_ptr<Label> filterModeLabel;
    std::unique_ptr<ComboBox> filterModeCombo;
    std::unique_ptr<ToggleButton> cutoffLink;
    std::unique_ptr<Label> label;
    std::unique_ptr<TextEditor> mainDisplay;
    std::unique_ptr<ModPanel> modMatrix;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainUI)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

