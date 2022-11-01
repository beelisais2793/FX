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


class AttachmentFactory;
class Model;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ReverbPanel  : public Component,
                     public ChangeBroadcaster,
                     public Slider::Listener,
                     public Button::Listener
{
public:
    //==============================================================================
    ReverbPanel (Model* model, AttachmentFactory* factory);
    ~ReverbPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked(Button* buttonThatWasClicked) override;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	Model* model;
	AttachmentFactory* factory;
    bool enabled = false;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupComponent> reverbGroup;
    std::unique_ptr<Slider> roomSize;
    std::unique_ptr<Label> roomSIzeLbl;
    std::unique_ptr<Slider> damping;
    std::unique_ptr<Label> dampingLbl;
    std::unique_ptr<Slider> width;
    std::unique_ptr<Label> widthLbl;
    std::unique_ptr<Slider> dryLevel;
    std::unique_ptr<Label> dryLbl;
    std::unique_ptr<Slider> wetLevel;
    std::unique_ptr<Label> wetLbl;
    std::unique_ptr<Slider> freeze;
    std::unique_ptr<Label> freezeLbl;
    std::unique_ptr<ToggleButton> reverbEnabledButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

