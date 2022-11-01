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

#include "FilterPanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
FilterPanel::FilterPanel (Model* model, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
	this->factory = factory;
    //[/Constructor_pre]

    fltCutoff.reset (new Slider ("fltCutoff"));
    addAndMakeVisible (fltCutoff.get());
    fltCutoff->setRange (0, 18000, 10);
    fltCutoff->setSliderStyle (Slider::RotaryVerticalDrag);
    fltCutoff->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    fltCutoff->addListener (this);

    fltCutoff->setBounds (5, 8, 64, 64);

    fltResonance.reset (new Slider ("fltResonance"));
    addAndMakeVisible (fltResonance.get());
    fltResonance->setRange (0, 5, 0.01);
    fltResonance->setSliderStyle (Slider::RotaryVerticalDrag);
    fltResonance->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    fltResonance->addListener (this);

    fltResonance->setBounds (93, 8, 64, 64);

    Cutoff.reset (new Label ("Cutoff",
                             TRANS("Cutoff\n")));
    addAndMakeVisible (Cutoff.get());
    Cutoff->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    Cutoff->setJustificationType (Justification::centredLeft);
    Cutoff->setEditable (false, false, false);
    Cutoff->setColour (TextEditor::textColourId, Colours::black);
    Cutoff->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    Cutoff->setBounds (13, 64, 56, 24);

    Res.reset (new Label ("Res",
                          TRANS("Q")));
    addAndMakeVisible (Res.get());
    Res->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    Res->setJustificationType (Justification::centredLeft);
    Res->setEditable (false, false, false);
    Res->setColour (TextEditor::textColourId, Colours::black);
    Res->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    Res->setBounds (117, 64, 24, 24);

    envAmt.reset (new Slider ("envAmt"));
    addAndMakeVisible (envAmt.get());
    envAmt->setRange (0, 1, 0.01);
    envAmt->setSliderStyle (Slider::RotaryVerticalDrag);
    envAmt->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    envAmt->addListener (this);

    envAmt->setBounds (181, 8, 64, 64);

    Amt.reset (new Label ("Amt",
                          TRANS("Env. Amount")));
    addAndMakeVisible (Amt.get());
    Amt->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    Amt->setJustificationType (Justification::centredLeft);
    Amt->setEditable (false, false, false);
    Amt->setColour (TextEditor::textColourId, Colours::black);
    Amt->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    Amt->setBounds (173, 64, 88, 24);

    filterMode.reset (new ComboBox ("filterMode"));
    addAndMakeVisible (filterMode.get());
    filterMode->setEditableText (false);
    filterMode->setJustificationType (Justification::centredLeft);
    filterMode->setTextWhenNothingSelected (String());
    filterMode->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    filterMode->addItem (TRANS("LP"), 1);
    filterMode->addItem (TRANS("HP"), 2);
    filterMode->addListener (this);

    filterMode->setBounds (261, 32, 64, 24);

    filterModeLabel.reset (new Label ("filterModeLabel",
                                      TRANS("Filter Mode")));
    addAndMakeVisible (filterModeLabel.get());
    filterModeLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    filterModeLabel->setJustificationType (Justification::centredLeft);
    filterModeLabel->setEditable (false, false, false);
    filterModeLabel->setColour (TextEditor::textColourId, Colours::black);
    filterModeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    filterModeLabel->setBounds (261, 64, 88, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

FilterPanel::~FilterPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fltCutoff = nullptr;
    fltResonance = nullptr;
    Cutoff = nullptr;
    Res = nullptr;
    envAmt = nullptr;
    Amt = nullptr;
    filterMode = nullptr;
    filterModeLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FilterPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FilterPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FilterPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == fltCutoff.get())
    {
        //[UserSliderCode_fltCutoff] -- add your slider handling code here..
        //[/UserSliderCode_fltCutoff]
    }
    else if (sliderThatWasMoved == fltResonance.get())
    {
        //[UserSliderCode_fltResonance] -- add your slider handling code here..
        //[/UserSliderCode_fltResonance]
    }
    else if (sliderThatWasMoved == envAmt.get())
    {
        //[UserSliderCode_envAmt] -- add your slider handling code here..
        //[/UserSliderCode_envAmt]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void FilterPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == filterMode.get())
    {
        //[UserComboBoxCode_filterMode] -- add your combo box handling code here..
        //[/UserComboBoxCode_filterMode]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void FilterPanel::initAttachments()
{
	if (getName().endsWith("1")) {
		factory->createSliderAttachment("cutoff1", fltCutoff.get());
		factory->createSliderAttachment("resonance1", fltResonance.get());
		factory->createSliderAttachment("envAmt1", envAmt.get());
		factory->createComboAttachment("filterMode1", filterMode.get());
	}
	else {
		factory->createSliderAttachment("cutoff2", fltCutoff.get());
		factory->createSliderAttachment("resonance2", fltResonance.get());
		factory->createSliderAttachment("envAmt2", envAmt.get());
		factory->createComboAttachment("filterMode2", filterMode.get());		
	}


}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FilterPanel" componentName=""
                 parentClasses="public Component" constructorParams="Model* model, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <SLIDER name="fltCutoff" id="820aba5f43f549e0" memberName="fltCutoff"
          virtualName="" explicitFocusOrder="0" pos="5 8 64 64" min="0.0"
          max="18000.0" int="10.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="fltResonance" id="e9128200d848646a" memberName="fltResonance"
          virtualName="" explicitFocusOrder="0" pos="93 8 64 64" min="0.0"
          max="5.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="Cutoff" id="3a68e51a612b8d7" memberName="Cutoff" virtualName=""
         explicitFocusOrder="0" pos="13 64 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Cutoff&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Res" id="25ed1f5df93f7ca0" memberName="Res" virtualName=""
         explicitFocusOrder="0" pos="117 64 24 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Q" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="envAmt" id="ee160c38e2a36d41" memberName="envAmt" virtualName=""
          explicitFocusOrder="0" pos="181 8 64 64" min="0.0" max="1.0"
          int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="Amt" id="8a3f810d8c37fde8" memberName="Amt" virtualName=""
         explicitFocusOrder="0" pos="173 64 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Env. Amount" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="filterMode" id="57cc12196967d2de" memberName="filterMode"
            virtualName="" explicitFocusOrder="0" pos="261 32 64 24" editable="0"
            layout="33" items="LP&#10;HP" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="filterModeLabel" id="d585a6de95ddee95" memberName="filterModeLabel"
         virtualName="" explicitFocusOrder="0" pos="261 64 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter Mode" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

