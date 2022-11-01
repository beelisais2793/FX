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

//[Headers] You can add your own extra header files here...
#include "Model.h"
#include "AttachmentFactory.h"
//[/Headers]

#include "ChorusPanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ChorusPanel::ChorusPanel (Model* model, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
	this->factory = factory;
    //[/Constructor_pre]

    chorusGroup.reset (new GroupComponent ("chorusGroup",
                                           TRANS("Chorus")));
    addAndMakeVisible (chorusGroup.get());

    chorusGroup->setBounds (0, 0, 240, 112);

    delay.reset (new Slider ("delay"));
    addAndMakeVisible (delay.get());
    delay->setRange (0, 1, 0.01);
    delay->setSliderStyle (Slider::RotaryVerticalDrag);
    delay->setTextBoxStyle (Slider::NoTextBox, false, 80, 12);
    delay->addListener (this);

    delay->setBounds (8, 24, 56, 64);

    delayLabel.reset (new Label ("delayLabel",
                                 TRANS("Delay\n")));
    addAndMakeVisible (delayLabel.get());
    delayLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    delayLabel->setJustificationType (Justification::centredLeft);
    delayLabel->setEditable (false, false, false);
    delayLabel->setColour (TextEditor::textColourId, Colours::black);
    delayLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    delayLabel->setBounds (16, 80, 40, 24);

    modulation.reset (new Slider ("modulation"));
    addAndMakeVisible (modulation.get());
    modulation->setRange (0, 1, 0.01);
    modulation->setSliderStyle (Slider::RotaryVerticalDrag);
    modulation->setTextBoxStyle (Slider::NoTextBox, false, 80, 12);
    modulation->addListener (this);

    modulation->setBounds (64, 24, 56, 64);

    modLabel.reset (new Label ("modLabel",
                               TRANS("Modulation\n")));
    addAndMakeVisible (modLabel.get());
    modLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    modLabel->setJustificationType (Justification::centredLeft);
    modLabel->setEditable (false, false, false);
    modLabel->setColour (TextEditor::textColourId, Colours::black);
    modLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    modLabel->setBounds (56, 80, 72, 24);

    feedback.reset (new Slider ("feedback"));
    addAndMakeVisible (feedback.get());
    feedback->setRange (0, 1, 0.01);
    feedback->setSliderStyle (Slider::RotaryVerticalDrag);
    feedback->setTextBoxStyle (Slider::NoTextBox, false, 80, 12);
    feedback->addListener (this);

    feedback->setBounds (120, 24, 56, 64);

    fblabel.reset (new Label ("fblabel",
                              TRANS("Feedback")));
    addAndMakeVisible (fblabel.get());
    fblabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    fblabel->setJustificationType (Justification::centredLeft);
    fblabel->setEditable (false, false, false);
    fblabel->setColour (TextEditor::textColourId, Colours::black);
    fblabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fblabel->setBounds (120, 80, 56, 24);

    mix.reset (new Slider ("mix"));
    addAndMakeVisible (mix.get());
    mix->setRange (0, 1, 0.01);
    mix->setSliderStyle (Slider::RotaryVerticalDrag);
    mix->setTextBoxStyle (Slider::NoTextBox, false, 80, 12);
    mix->addListener (this);

    mix->setBounds (176, 24, 56, 64);

    mixLabel.reset (new Label ("mixLabel",
                               TRANS("Mix\n")));
    addAndMakeVisible (mixLabel.get());
    mixLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    mixLabel->setJustificationType (Justification::centredLeft);
    mixLabel->setEditable (false, false, false);
    mixLabel->setColour (TextEditor::textColourId, Colours::black);
    mixLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    mixLabel->setBounds (192, 80, 32, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ChorusPanel::~ChorusPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    chorusGroup = nullptr;
    delay = nullptr;
    delayLabel = nullptr;
    modulation = nullptr;
    modLabel = nullptr;
    feedback = nullptr;
    fblabel = nullptr;
    mix = nullptr;
    mixLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ChorusPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ChorusPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ChorusPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == delay.get())
    {
        //[UserSliderCode_delay] -- add your slider handling code here..
		model->chrDelay = sliderThatWasMoved->getValue();
        //[/UserSliderCode_delay]
    }
    else if (sliderThatWasMoved == modulation.get())
    {
        //[UserSliderCode_modulation] -- add your slider handling code here..
		model->chrModulation = sliderThatWasMoved->getValue();
        //[/UserSliderCode_modulation]
    }
    else if (sliderThatWasMoved == feedback.get())
    {
        //[UserSliderCode_feedback] -- add your slider handling code here..
		model->chrFeedback = sliderThatWasMoved->getValue();
        //[/UserSliderCode_feedback]
    }
    else if (sliderThatWasMoved == mix.get())
    {
        //[UserSliderCode_mix] -- add your slider handling code here..
		model->chrMix = sliderThatWasMoved->getValue();
        //[/UserSliderCode_mix]
    }

    //[UsersliderValueChanged_Post]
	sendChangeMessage();
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ChorusPanel::initAttachments()
{
	factory->createSliderAttachment("chrDelay", delay.get());
	factory->createSliderAttachment("chrModulation", modulation.get());
	factory->createSliderAttachment("chrFeedback", feedback.get());
	factory->createSliderAttachment("chrMix", mix.get());
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChorusPanel" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams="Model* model, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <GROUPCOMPONENT name="chorusGroup" id="eacd1de0d050c8e2" memberName="chorusGroup"
                  virtualName="" explicitFocusOrder="0" pos="0 0 240 112" title="Chorus"/>
  <SLIDER name="delay" id="abab2f5032a25de2" memberName="delay" virtualName=""
          explicitFocusOrder="0" pos="8 24 56 64" min="0.0" max="1.0" int="0.01"
          style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="12" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="delayLabel" id="94f67f48984c9459" memberName="delayLabel"
         virtualName="" explicitFocusOrder="0" pos="16 80 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Delay&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="modulation" id="fb30fdbeb23f1267" memberName="modulation"
          virtualName="" explicitFocusOrder="0" pos="64 24 56 64" min="0.0"
          max="1.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="12" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="modLabel" id="37dfd528d1bda78" memberName="modLabel" virtualName=""
         explicitFocusOrder="0" pos="56 80 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Modulation&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="feedback" id="2216e6c2e45b40a1" memberName="feedback" virtualName=""
          explicitFocusOrder="0" pos="120 24 56 64" min="0.0" max="1.0"
          int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="12" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="fblabel" id="1084255963638e9d" memberName="fblabel" virtualName=""
         explicitFocusOrder="0" pos="120 80 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Feedback" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="mix" id="f1be319ec0c8f888" memberName="mix" virtualName=""
          explicitFocusOrder="0" pos="176 24 56 64" min="0.0" max="1.0"
          int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="12" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="mixLabel" id="d3b292080de81ca2" memberName="mixLabel" virtualName=""
         explicitFocusOrder="0" pos="192 80 32 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

