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
class ChorusPanel  : public Component,
                     public ChangeBroadcaster,
                     public Slider::Listener
{
public:
    //==============================================================================
    ChorusPanel (Model* model, AttachmentFactory* factory);
    ~ChorusPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	Model* model;
	AttachmentFactory* factory;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupComponent> chorusGroup;
    std::unique_ptr<Slider> delay;
    std::unique_ptr<Label> delayLabel;
    std::unique_ptr<Slider> modulation;
    std::unique_ptr<Label> modLabel;
    std::unique_ptr<Slider> feedback;
    std::unique_ptr<Label> fblabel;
    std::unique_ptr<Slider> mix;
    std::unique_ptr<Label> mixLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

