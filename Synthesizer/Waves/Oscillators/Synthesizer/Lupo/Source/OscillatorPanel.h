/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioEngine/Oszillator.h"
class Model;
class AttachmentFactory;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class OscillatorPanel  : public Component,
                         public ChangeBroadcaster,
                         public Slider::Listener,
                         public Button::Listener,
                         public ComboBox::Listener
{
public:
    //==============================================================================
    OscillatorPanel (Model* model, AttachmentFactory* factory);
    ~OscillatorPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void SetTitle(String title);
	void initAttachments();
	ComboBox* getShapeCombo() {
		return shapeComboBox.get();
	}

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    // Binary resources:
    static const char* oscillator_square_48_png;
    static const int oscillator_square_48_pngSize;
    static const char* oscillator_sine_48_png;
    static const int oscillator_sine_48_pngSize;
    static const char* oscillator_saw_48_png;
    static const int oscillator_saw_48_pngSize;
    static const char* oscillator_noise_48_png;
    static const int oscillator_noise_48_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	Model* model;
	AttachmentFactory* factory;
	int id;
    bool enabled = false;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupComponent> oscillatorGroup;
    std::unique_ptr<Slider> oscPitch;
    std::unique_ptr<Label> pitchlabel1;
    std::unique_ptr<Slider> oscFine;
    std::unique_ptr<Label> fineLabel;
    std::unique_ptr<Slider> spreadSlider;
    std::unique_ptr<Label> spreadLabel;
    std::unique_ptr<ToggleButton> syncButton;
    std::unique_ptr<ComboBox> shapeComboBox;
    std::unique_ptr<Label> shapeLabel;
    std::unique_ptr<ToggleButton> oscEnabledButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

