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
class DistortionPanel  : public Component,
                         public Slider::Listener,
                         public ComboBox::Listener
{
public:
    //==============================================================================
    DistortionPanel (Model* model, AttachmentFactory* factory);
    ~DistortionPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AttachmentFactory* factory;
	Model* model;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> driveSlider;
    std::unique_ptr<Slider> mixSlider;
    std::unique_ptr<Label> driveLabel;
    std::unique_ptr<Label> mixLabel;
    std::unique_ptr<ComboBox> modeCombo;
    std::unique_ptr<Label> modeLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

