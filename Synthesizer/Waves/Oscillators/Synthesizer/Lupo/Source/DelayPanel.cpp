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

#include "DelayPanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
DelayPanel::DelayPanel (Model* model, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
	this->factory = factory;
    //[/Constructor_pre]

    delayGroup.reset (new GroupComponent ("delayGroup",
                                          TRANS("Stereo delay")));
    addAndMakeVisible (delayGroup.get());

    delayGroup->setBounds (8, 0, 248, 112);

    timeLeft.reset (new Slider ("timeLeft"));
    addAndMakeVisible (timeLeft.get());
    timeLeft->setRange (0, 1000, 5);
    timeLeft->setSliderStyle (Slider::RotaryVerticalDrag);
    timeLeft->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    timeLeft->addListener (this);

    timeLeft->setBounds (24, 24, 56, 64);

    timeRight.reset (new Slider ("timeRight"));
    addAndMakeVisible (timeRight.get());
    timeRight->setRange (0, 1000, 5);
    timeRight->setSliderStyle (Slider::RotaryVerticalDrag);
    timeRight->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    timeRight->addListener (this);

    timeRight->setBounds (80, 24, 56, 64);

    feedback.reset (new Slider ("feedback"));
    addAndMakeVisible (feedback.get());
    feedback->setRange (0, 1, 0.01);
    feedback->setSliderStyle (Slider::RotaryVerticalDrag);
    feedback->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    feedback->addListener (this);

    feedback->setBounds (136, 24, 56, 64);

    mix.reset (new Slider ("mix"));
    addAndMakeVisible (mix.get());
    mix->setRange (0, 1, 0.01);
    mix->setSliderStyle (Slider::RotaryVerticalDrag);
    mix->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    mix->addListener (this);

    mix->setBounds (192, 24, 56, 64);

    labelTime.reset (new Label ("labelTime",
                                TRANS("Time L\n")));
    addAndMakeVisible (labelTime.get());
    labelTime->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelTime->setJustificationType (Justification::centred);
    labelTime->setEditable (false, false, false);
    labelTime->setColour (TextEditor::textColourId, Colours::black);
    labelTime->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    labelTime->setBounds (24, 80, 48, 24);

    labelTime2.reset (new Label ("labelTime",
                                 TRANS("Time R\n")));
    addAndMakeVisible (labelTime2.get());
    labelTime2->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelTime2->setJustificationType (Justification::centred);
    labelTime2->setEditable (false, false, false);
    labelTime2->setColour (TextEditor::textColourId, Colours::black);
    labelTime2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    labelTime2->setBounds (80, 80, 56, 24);

    labelFb.reset (new Label ("labelFb",
                              TRANS("Feedback")));
    addAndMakeVisible (labelFb.get());
    labelFb->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelFb->setJustificationType (Justification::centredLeft);
    labelFb->setEditable (false, false, false);
    labelFb->setColour (TextEditor::textColourId, Colours::black);
    labelFb->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    labelFb->setBounds (136, 80, 56, 24);

    labelMix.reset (new Label ("labelMix",
                               TRANS("Mix")));
    addAndMakeVisible (labelMix.get());
    labelMix->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelMix->setJustificationType (Justification::centredLeft);
    labelMix->setEditable (false, false, false);
    labelMix->setColour (TextEditor::textColourId, Colours::black);
    labelMix->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    labelMix->setBounds (208, 80, 48, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

DelayPanel::~DelayPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    delayGroup = nullptr;
    timeLeft = nullptr;
    timeRight = nullptr;
    feedback = nullptr;
    mix = nullptr;
    labelTime = nullptr;
    labelTime2 = nullptr;
    labelFb = nullptr;
    labelMix = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void DelayPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void DelayPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DelayPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == timeLeft.get())
    {
        //[UserSliderCode_timeLeft] -- add your slider handling code here..
		model->dlyTimeLeft = sliderThatWasMoved->getValue();
        //[/UserSliderCode_timeLeft]
    }
    else if (sliderThatWasMoved == timeRight.get())
    {
        //[UserSliderCode_timeRight] -- add your slider handling code here..
		model->dlyTimeRight = sliderThatWasMoved->getValue();
        //[/UserSliderCode_timeRight]
    }
    else if (sliderThatWasMoved == feedback.get())
    {
        //[UserSliderCode_feedback] -- add your slider handling code here..
		model->dlyFeedback = sliderThatWasMoved->getValue();
        //[/UserSliderCode_feedback]
    }
    else if (sliderThatWasMoved == mix.get())
    {
        //[UserSliderCode_mix] -- add your slider handling code here..
		model->dlyMix = sliderThatWasMoved->getValue();
        //[/UserSliderCode_mix]
    }

    //[UsersliderValueChanged_Post]
	sendChangeMessage();
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void DelayPanel::initAttachments() {
	factory->createSliderAttachment("dlyTimeLeft", timeLeft.get());
	factory->createSliderAttachment("dlyTimeRight", timeRight.get());
	factory->createSliderAttachment("dlyFeedback", feedback.get());
	factory->createSliderAttachment("dlyMix", mix.get());
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DelayPanel" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams="Model* model, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <GROUPCOMPONENT name="delayGroup" id="5e1dad4ec7bae1e2" memberName="delayGroup"
                  virtualName="" explicitFocusOrder="0" pos="8 0 248 112" title="Stereo delay"/>
  <SLIDER name="timeLeft" id="f2df016a0268b524" memberName="timeLeft" virtualName=""
          explicitFocusOrder="0" pos="24 24 56 64" min="0.0" max="1000.0"
          int="5.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="timeRight" id="eabe77dba6904595" memberName="timeRight"
          virtualName="" explicitFocusOrder="0" pos="80 24 56 64" min="0.0"
          max="1000.0" int="5.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="feedback" id="ef0b4be3871ae3a4" memberName="feedback" virtualName=""
          explicitFocusOrder="0" pos="136 24 56 64" min="0.0" max="1.0"
          int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="mix" id="7a09499f272a836f" memberName="mix" virtualName=""
          explicitFocusOrder="0" pos="192 24 56 64" min="0.0" max="1.0"
          int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="labelTime" id="4db47c9320323e6a" memberName="labelTime"
         virtualName="" explicitFocusOrder="0" pos="24 80 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Time L&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="labelTime" id="91bc68e5f5fbba29" memberName="labelTime2"
         virtualName="" explicitFocusOrder="0" pos="80 80 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Time R&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="labelFb" id="4e9ae61b560dbb17" memberName="labelFb" virtualName=""
         explicitFocusOrder="0" pos="136 80 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Feedback" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="labelMix" id="cf219d26a0150a09" memberName="labelMix" virtualName=""
         explicitFocusOrder="0" pos="208 80 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

