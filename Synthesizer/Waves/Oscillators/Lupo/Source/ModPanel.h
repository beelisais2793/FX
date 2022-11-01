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
class Model;
class AttachmentFactory;
class ModMatrix;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ModPanel  : public Component,
                  public ComboBox::Listener,
                  public Slider::Listener
{
public:
    //==============================================================================
    ModPanel (ModMatrix* matrix,Model* model, AttachmentFactory* factory);
    ~ModPanel() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void initAttachments();
	void populateSources(ComboBox* combo);
	void populateTargets(ComboBox* combo);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AttachmentFactory* factory;
	Model* model;
	ModMatrix* matrix;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ComboBox> Source_1;
    std::unique_ptr<ComboBox> Source_2;
    std::unique_ptr<ComboBox> Source_3;
    std::unique_ptr<ComboBox> Source_4;
    std::unique_ptr<ComboBox> Source_5;
    std::unique_ptr<ComboBox> Source_0;
    std::unique_ptr<ComboBox> Target_0;
    std::unique_ptr<ComboBox> Target_1;
    std::unique_ptr<ComboBox> Target_2;
    std::unique_ptr<ComboBox> Target_3;
    std::unique_ptr<ComboBox> Target_4;
    std::unique_ptr<ComboBox> Target_5;
    std::unique_ptr<Slider> Amount_0;
    std::unique_ptr<Slider> Amount_1;
    std::unique_ptr<Slider> Amount_2;
    std::unique_ptr<Slider> Amount_3;
    std::unique_ptr<Slider> Amount_4;
    std::unique_ptr<Slider> Amount_5;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

