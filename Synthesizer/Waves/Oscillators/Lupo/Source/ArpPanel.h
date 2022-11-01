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
class Arpeggiator;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ArpPanel  : public Component,
                  public Slider::Listener,
                  public ComboBox::Listener,
                  public Button::Listener
{
public:
    //==============================================================================
    ArpPanel (AttachmentFactory* factory, Arpeggiator* arp );
    ~ArpPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AttachmentFactory* factory;
	Arpeggiator* arp;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> speedSlider;
    std::unique_ptr<ComboBox> octaveCombo;
    std::unique_ptr<Label> label;
    std::unique_ptr<Label> octaveLabel;
    std::unique_ptr<ComboBox> modeCombo;
    std::unique_ptr<Label> modeLabel;
    std::unique_ptr<ToggleButton> enabledButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArpPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

