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
class MixerChannelPanel  : public Component,
                           public ChangeBroadcaster,
                           public Slider::Listener
{
public:
    //==============================================================================
    MixerChannelPanel (Model* model, AttachmentFactory* factory);
    ~MixerChannelPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void SetTitle(String title);
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
    std::unique_ptr<GroupComponent> channelGroup;
    std::unique_ptr<Slider> volSlider;
    std::unique_ptr<Slider> panSlider;
    std::unique_ptr<Label> volumeLabel;
    std::unique_ptr<Label> panLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerChannelPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

