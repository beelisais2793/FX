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
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SamplerPanel  : public Component,
                      public ChangeBroadcaster,
                      public Slider::Listener,
                      public Button::Listener
{
public:
    //==============================================================================
    SamplerPanel (Model* model);
    ~SamplerPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	Model* model;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GroupComponent> sampler;
    std::unique_ptr<Slider> oscPitch;
    std::unique_ptr<Label> pitchlabel1;
    std::unique_ptr<Slider> oscFine;
    std::unique_ptr<Label> fineLabel;
    std::unique_ptr<Label> sampleLabel;
    std::unique_ptr<TextEditor> sampleName;
    std::unique_ptr<TextButton> browseButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

