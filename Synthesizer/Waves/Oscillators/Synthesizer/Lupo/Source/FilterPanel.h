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
class FilterPanel  : public Component,
                     public Slider::Listener,
                     public ComboBox::Listener
{
public:
    //==============================================================================
    FilterPanel (Model* model, AttachmentFactory* factory);
    ~FilterPanel();

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
	Model* model;
	AttachmentFactory* factory;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> fltCutoff;
    std::unique_ptr<Slider> fltResonance;
    std::unique_ptr<Label> Cutoff;
    std::unique_ptr<Label> Res;
    std::unique_ptr<Slider> envAmt;
    std::unique_ptr<Label> Amt;
    std::unique_ptr<ComboBox> filterMode;
    std::unique_ptr<Label> filterModeLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

