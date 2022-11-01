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
class LFOPanel  : public Component,
                  public Slider::Listener,
                  public ComboBox::Listener
{
public:
    //==============================================================================
    LFOPanel (Model* model, AttachmentFactory* factory);
    ~LFOPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    // Binary resources:
    static const char* oscillator_sine_48_png;
    static const int oscillator_sine_48_pngSize;
    static const char* oscillator_square_48_png;
    static const int oscillator_square_48_pngSize;
    static const char* oscillator_saw_48_png;
    static const int oscillator_saw_48_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AttachmentFactory* factory;
	Model* model;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> lfoSpeed;
    std::unique_ptr<Label> speedLabel;
    std::unique_ptr<Slider> lfoAmount;
    std::unique_ptr<Label> amtLabel;
    std::unique_ptr<Label> shapeLabel;
    std::unique_ptr<ComboBox> shapeComboBox;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

