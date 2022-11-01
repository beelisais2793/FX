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
#include "MessageBus/MessageBus.h"
#include "AttachmentFactory.h"
//[/Headers]

#include "EnvelopePanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
EnvelopePanel::EnvelopePanel (Model* model, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
	this->factory = factory;
    //[/Constructor_pre]

    amp_attack.reset (new Slider ("amp_attack"));
    addAndMakeVisible (amp_attack.get());
    amp_attack->setRange (0, 10, 0.01);
    amp_attack->setSliderStyle (Slider::RotaryVerticalDrag);
    amp_attack->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    amp_attack->addListener (this);

    amp_attack->setBounds (0, 8, 64, 64);

    amp_decay.reset (new Slider ("amp_decay"));
    addAndMakeVisible (amp_decay.get());
    amp_decay->setRange (0, 5, 0.01);
    amp_decay->setSliderStyle (Slider::RotaryVerticalDrag);
    amp_decay->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    amp_decay->addListener (this);

    amp_decay->setBounds (64, 8, 64, 64);

    amp_sustain.reset (new Slider ("amp_sustain"));
    addAndMakeVisible (amp_sustain.get());
    amp_sustain->setRange (0, 1, 0.01);
    amp_sustain->setSliderStyle (Slider::RotaryVerticalDrag);
    amp_sustain->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    amp_sustain->addListener (this);

    amp_sustain->setBounds (128, 8, 64, 64);

    amp_release.reset (new Slider ("amp_release"));
    addAndMakeVisible (amp_release.get());
    amp_release->setRange (0, 10, 0.01);
    amp_release->setSliderStyle (Slider::RotaryVerticalDrag);
    amp_release->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    amp_release->addListener (this);

    amp_release->setBounds (192, 8, 64, 64);

    attack.reset (new Label ("attack",
                             TRANS("Attack")));
    addAndMakeVisible (attack.get());
    attack->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    attack->setJustificationType (Justification::centredLeft);
    attack->setEditable (false, false, false);
    attack->setColour (TextEditor::textColourId, Colours::black);
    attack->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    attack->setBounds (8, 64, 56, 24);

    decay.reset (new Label ("decay",
                            TRANS("Decay\n")));
    addAndMakeVisible (decay.get());
    decay->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    decay->setJustificationType (Justification::centredLeft);
    decay->setEditable (false, false, false);
    decay->setColour (TextEditor::textColourId, Colours::black);
    decay->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    decay->setBounds (72, 64, 56, 24);

    sustain.reset (new Label ("sustain",
                              TRANS("Sustain")));
    addAndMakeVisible (sustain.get());
    sustain->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    sustain->setJustificationType (Justification::centredLeft);
    sustain->setEditable (false, false, false);
    sustain->setColour (TextEditor::textColourId, Colours::black);
    sustain->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    sustain->setBounds (136, 64, 56, 24);

    Release.reset (new Label ("Release",
                              TRANS("Release")));
    addAndMakeVisible (Release.get());
    Release->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    Release->setJustificationType (Justification::centredLeft);
    Release->setEditable (false, false, false);
    Release->setColour (TextEditor::textColourId, Colours::black);
    Release->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    Release->setBounds (200, 64, 56, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

EnvelopePanel::~EnvelopePanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    amp_attack = nullptr;
    amp_decay = nullptr;
    amp_sustain = nullptr;
    amp_release = nullptr;
    attack = nullptr;
    decay = nullptr;
    sustain = nullptr;
    Release = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void EnvelopePanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void EnvelopePanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void EnvelopePanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == amp_attack.get())
    {
        //[UserSliderCode_amp_attack] -- add your slider handling code here..

		if (getName() == "ampEnvelope") {
			model->ampAttack = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope1") {
			model->fltAttack = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope2") {
			model->auxAttack = sliderThatWasMoved->getValue();
		}


        //[/UserSliderCode_amp_attack]
    }
    else if (sliderThatWasMoved == amp_decay.get())
    {
        //[UserSliderCode_amp_decay] -- add your slider handling code here..
		if (getName() == "ampEnvelope") {
			model->ampDecay = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope1") {
			model->fltDecay = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope2") {
			model->auxDecay = sliderThatWasMoved->getValue();
		}

        //[/UserSliderCode_amp_decay]
    }
    else if (sliderThatWasMoved == amp_sustain.get())
    {
        //[UserSliderCode_amp_sustain] -- add your slider handling code here..
		if (getName() == "ampEnvelope") {
			model->ampSustain = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope1") {
			model->fltSustain = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope2") {
			model->auxSustain = sliderThatWasMoved->getValue();
		}

        //[/UserSliderCode_amp_sustain]
    }
    else if (sliderThatWasMoved == amp_release.get())
    {
        //[UserSliderCode_amp_release] -- add your slider handling code here..
		if (getName() == "ampEnvelope") {
			model->ampRelease = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope1") {
			model->fltRelease = sliderThatWasMoved->getValue();
		}
		if (getName() == "auxEnvelope2") {
			model->ampRelease = sliderThatWasMoved->getValue();
		}
        //[/UserSliderCode_amp_release]
    }

    //[UsersliderValueChanged_Post]
	sendChangeMessage();
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void EnvelopePanel::initAttachments()
{
	if (getName() == "ampEnvelope") {
		factory->createSliderAttachment("ampAttack", amp_attack.get());
		factory->createSliderAttachment("ampDecay", amp_decay.get());
		factory->createSliderAttachment("ampSustain", amp_sustain.get());
		factory->createSliderAttachment("ampRelease", amp_release.get());
	}
	else if (getName() == "auxEnvelope1") {
		factory->createSliderAttachment("auxAttack1", amp_attack.get());
		factory->createSliderAttachment("auxDecay1", amp_decay.get());
		factory->createSliderAttachment("auxSustain1", amp_sustain.get());
		factory->createSliderAttachment("auxRelease1", amp_release.get());
	}
	else if (getName() == "auxEnvelope2") {
		factory->createSliderAttachment("auxAttack2", amp_attack.get());
		factory->createSliderAttachment("auxDecay2", amp_decay.get());
		factory->createSliderAttachment("auxSustain2", amp_sustain.get());
		factory->createSliderAttachment("auxRelease2", amp_release.get());
	}


}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="EnvelopePanel" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams="Model* model, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <SLIDER name="amp_attack" id="dd56ce6b5eccf295" memberName="amp_attack"
          virtualName="" explicitFocusOrder="0" pos="0 8 64 64" min="0.0"
          max="10.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="amp_decay" id="336632529c646979" memberName="amp_decay"
          virtualName="" explicitFocusOrder="0" pos="64 8 64 64" min="0.0"
          max="5.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="amp_sustain" id="cefe10e4212549e2" memberName="amp_sustain"
          virtualName="" explicitFocusOrder="0" pos="128 8 64 64" min="0.0"
          max="1.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="amp_release" id="9ced331028b46d97" memberName="amp_release"
          virtualName="" explicitFocusOrder="0" pos="192 8 64 64" min="0.0"
          max="10.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="attack" id="4938c10e2b6678d9" memberName="attack" virtualName=""
         explicitFocusOrder="0" pos="8 64 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Attack" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="decay" id="b3fe82ab215c0350" memberName="decay" virtualName=""
         explicitFocusOrder="0" pos="72 64 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Decay&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="sustain" id="3a9c1db62849c1a4" memberName="sustain" virtualName=""
         explicitFocusOrder="0" pos="136 64 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Sustain" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Release" id="a532e9811921ca43" memberName="Release" virtualName=""
         explicitFocusOrder="0" pos="200 64 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Release" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

