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
#include "AttachmentFactory.h"
#include "Model.h"
//[/Headers]

#include "DistortionPanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
DistortionPanel::DistortionPanel (Model* model, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
	this->factory = factory;
    //[/Constructor_pre]

    driveSlider.reset (new Slider ("driveSlider"));
    addAndMakeVisible (driveSlider.get());
    driveSlider->setRange (0, 5, 0.01);
    driveSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    driveSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    driveSlider->addListener (this);

    driveSlider->setBounds (0, 0, 56, 56);

    mixSlider.reset (new Slider ("mixSlider"));
    addAndMakeVisible (mixSlider.get());
    mixSlider->setRange (0, 1, 0.01);
    mixSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    mixSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    mixSlider->addListener (this);

    mixSlider->setBounds (56, 0, 56, 56);

    driveLabel.reset (new Label ("driveLabel",
                                 TRANS("Drive\n")));
    addAndMakeVisible (driveLabel.get());
    driveLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    driveLabel->setJustificationType (Justification::centredLeft);
    driveLabel->setEditable (false, false, false);
    driveLabel->setColour (TextEditor::textColourId, Colours::black);
    driveLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    driveLabel->setBounds (8, 48, 40, 24);

    mixLabel.reset (new Label ("mixLabel",
                               TRANS("Mix\n")));
    addAndMakeVisible (mixLabel.get());
    mixLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    mixLabel->setJustificationType (Justification::centredLeft);
    mixLabel->setEditable (false, false, false);
    mixLabel->setColour (TextEditor::textColourId, Colours::black);
    mixLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    mixLabel->setBounds (72, 48, 40, 24);

    modeCombo.reset (new ComboBox ("modeCombo"));
    addAndMakeVisible (modeCombo.get());
    modeCombo->setEditableText (false);
    modeCombo->setJustificationType (Justification::centredLeft);
    modeCombo->setTextWhenNothingSelected (String());
    modeCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    modeCombo->addItem (TRANS("Sofr clip"), 1);
    modeCombo->addItem (TRANS("Hard clip"), 2);
    modeCombo->addItem (TRANS("Cubic"), 3);
    modeCombo->addListener (this);

    modeCombo->setBounds (120, 16, 80, 24);

    modeLabel.reset (new Label ("modeLabel",
                                TRANS("Mode")));
    addAndMakeVisible (modeLabel.get());
    modeLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    modeLabel->setJustificationType (Justification::centredLeft);
    modeLabel->setEditable (false, false, false);
    modeLabel->setColour (TextEditor::textColourId, Colours::black);
    modeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    modeLabel->setBounds (136, 48, 40, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

DistortionPanel::~DistortionPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    driveSlider = nullptr;
    mixSlider = nullptr;
    driveLabel = nullptr;
    mixLabel = nullptr;
    modeCombo = nullptr;
    modeLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void DistortionPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void DistortionPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DistortionPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == driveSlider.get())
    {
        //[UserSliderCode_driveSlider] -- add your slider handling code here..
		model->distDrive = sliderThatWasMoved->getValue();
        //[/UserSliderCode_driveSlider]
    }
    else if (sliderThatWasMoved == mixSlider.get())
    {
        //[UserSliderCode_mixSlider] -- add your slider handling code here..
		model->distMix = sliderThatWasMoved->getValue();
        //[/UserSliderCode_mixSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void DistortionPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == modeCombo.get())
    {
        //[UserComboBoxCode_modeCombo] -- add your combo box handling code here..
		model->distMode = comboBoxThatHasChanged->getSelectedItemIndex();
        //[/UserComboBoxCode_modeCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void DistortionPanel::initAttachments()
{
	factory->createSliderAttachment("distDrive", driveSlider.get());
	factory->createSliderAttachment("distMix", mixSlider.get());
	factory->createComboAttachment("distMode", modeCombo.get());
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DistortionPanel" componentName=""
                 parentClasses="public Component" constructorParams="Model* model, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <SLIDER name="driveSlider" id="43919afc1dc4425" memberName="driveSlider"
          virtualName="" explicitFocusOrder="0" pos="0 0 56 56" min="0.0"
          max="5.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="mixSlider" id="630e2971f244c01a" memberName="mixSlider"
          virtualName="" explicitFocusOrder="0" pos="56 0 56 56" min="0.0"
          max="1.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="driveLabel" id="23cb67ac075d7979" memberName="driveLabel"
         virtualName="" explicitFocusOrder="0" pos="8 48 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Drive&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="mixLabel" id="37b542e51d929190" memberName="mixLabel" virtualName=""
         explicitFocusOrder="0" pos="72 48 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mix&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="modeCombo" id="db8b400f3893a4b8" memberName="modeCombo"
            virtualName="" explicitFocusOrder="0" pos="120 16 80 24" editable="0"
            layout="33" items="Sofr clip&#10;Hard clip&#10;Cubic" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <LABEL name="modeLabel" id="d321910e71ce58b" memberName="modeLabel"
         virtualName="" explicitFocusOrder="0" pos="136 48 40 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Mode" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

