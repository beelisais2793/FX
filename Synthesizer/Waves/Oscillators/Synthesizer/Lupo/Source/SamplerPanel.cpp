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
//[/Headers]

#include "SamplerPanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SamplerPanel::SamplerPanel (Model* model)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
    //[/Constructor_pre]

    sampler.reset (new GroupComponent ("sampler",
                                       TRANS("Sampler")));
    addAndMakeVisible (sampler.get());

    sampler->setBounds (0, 0, 208, 240);

    oscPitch.reset (new Slider ("oscPitch"));
    addAndMakeVisible (oscPitch.get());
    oscPitch->setRange (-36, 36, 1);
    oscPitch->setSliderStyle (Slider::RotaryVerticalDrag);
    oscPitch->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    oscPitch->addListener (this);

    oscPitch->setBounds (16, 16, 80, 80);

    pitchlabel1.reset (new Label ("pitchlabel1",
                                  TRANS("Pitch")));
    addAndMakeVisible (pitchlabel1.get());
    pitchlabel1->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    pitchlabel1->setJustificationType (Justification::centredLeft);
    pitchlabel1->setEditable (false, false, false);
    pitchlabel1->setColour (TextEditor::textColourId, Colours::black);
    pitchlabel1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    pitchlabel1->setBounds (40, 96, 39, 24);

    oscFine.reset (new Slider ("oscFine"));
    addAndMakeVisible (oscFine.get());
    oscFine->setRange (-1, 1, 0.01);
    oscFine->setSliderStyle (Slider::RotaryVerticalDrag);
    oscFine->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    oscFine->addListener (this);

    oscFine->setBounds (112, 32, 64, 64);

    fineLabel.reset (new Label ("fineLabel",
                                TRANS("Fine\n")));
    addAndMakeVisible (fineLabel.get());
    fineLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    fineLabel->setJustificationType (Justification::centredLeft);
    fineLabel->setEditable (false, false, false);
    fineLabel->setColour (TextEditor::textColourId, Colours::black);
    fineLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fineLabel->setBounds (136, 96, 39, 24);

    sampleLabel.reset (new Label ("sampleLabel",
                                  TRANS("Sample\n")));
    addAndMakeVisible (sampleLabel.get());
    sampleLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    sampleLabel->setJustificationType (Justification::centredLeft);
    sampleLabel->setEditable (false, false, false);
    sampleLabel->setColour (TextEditor::textColourId, Colours::black);
    sampleLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    sampleLabel->setBounds (16, 120, 72, 24);

    sampleName.reset (new TextEditor ("new text editor"));
    addAndMakeVisible (sampleName.get());
    sampleName->setMultiLine (false);
    sampleName->setReturnKeyStartsNewLine (false);
    sampleName->setReadOnly (false);
    sampleName->setScrollbarsShown (true);
    sampleName->setCaretVisible (true);
    sampleName->setPopupMenuEnabled (true);
    sampleName->setText (String());

    sampleName->setBounds (16, 152, 112, 24);

    browseButton.reset (new TextButton ("browseButton"));
    addAndMakeVisible (browseButton.get());
    browseButton->setButtonText (TRANS("Browse"));
    browseButton->addListener (this);

    browseButton->setBounds (136, 152, 56, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

SamplerPanel::~SamplerPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sampler = nullptr;
    oscPitch = nullptr;
    pitchlabel1 = nullptr;
    oscFine = nullptr;
    fineLabel = nullptr;
    sampleLabel = nullptr;
    sampleName = nullptr;
    browseButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SamplerPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SamplerPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SamplerPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == oscPitch.get())
    {
        //[UserSliderCode_oscPitch] -- add your slider handling code here..
        //[/UserSliderCode_oscPitch]
    }
    else if (sliderThatWasMoved == oscFine.get())
    {
        //[UserSliderCode_oscFine] -- add your slider handling code here..
        //[/UserSliderCode_oscFine]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void SamplerPanel::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == browseButton.get())
    {
        //[UserButtonCode_browseButton] -- add your button handler code here..
		FileChooser chooser("Select file to open", File(), "*");

		if (chooser.browseForFileToOpen()) {

			File file = chooser.getResult();
			FileInputStream* is = new FileInputStream(file);


		}

        //[/UserButtonCode_browseButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SamplerPanel" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams="Model* model"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <GROUPCOMPONENT name="sampler" id="da45ea08bdb19564" memberName="sampler" virtualName=""
                  explicitFocusOrder="0" pos="0 0 208 240" title="Sampler"/>
  <SLIDER name="oscPitch" id="57e273c20e9f4d5e" memberName="oscPitch" virtualName=""
          explicitFocusOrder="0" pos="16 16 80 80" min="-36.0" max="36.0"
          int="1.0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="pitchlabel1" id="abdc5c483d1b1ed5" memberName="pitchlabel1"
         virtualName="" explicitFocusOrder="0" pos="40 96 39 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Pitch" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="oscFine" id="533f1365e15f5919" memberName="oscFine" virtualName=""
          explicitFocusOrder="0" pos="112 32 64 64" min="-1.0" max="1.0"
          int="0.01" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="fineLabel" id="bd9e25fff1c8157f" memberName="fineLabel"
         virtualName="" explicitFocusOrder="0" pos="136 96 39 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Fine&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="sampleLabel" id="b3b3369f166262b1" memberName="sampleLabel"
         virtualName="" explicitFocusOrder="0" pos="16 120 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Sample&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="54dd8636422212da" memberName="sampleName"
              virtualName="" explicitFocusOrder="0" pos="16 152 112 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTBUTTON name="browseButton" id="c063e5cb1067c73a" memberName="browseButton"
              virtualName="" explicitFocusOrder="0" pos="136 152 56 24" buttonText="Browse"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

