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

#include "PresetDialog.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PresetDialog::PresetDialog (ComboBox* presetBox, Model* model)
{
    //[Constructor_pre] You can add your own custom stuff here..
    this->presetBox = presetBox;
    //[/Constructor_pre]

    okButton.reset (new TextButton ("okButton"));
    addAndMakeVisible (okButton.get());
    okButton->setExplicitFocusOrder (2);
    okButton->setButtonText (TRANS("Ok"));
    okButton->addListener (this);

    okButton->setBounds (289, 352, 150, 24);

    cancelButton.reset (new TextButton ("cancelButton"));
    addAndMakeVisible (cancelButton.get());
    cancelButton->setExplicitFocusOrder (3);
    cancelButton->setButtonText (TRANS("Cancel"));
    cancelButton->addListener (this);

    cancelButton->setBounds (473, 352, 150, 24);

    textEditor.reset (new TextEditor ("textEditor"));
    addAndMakeVisible (textEditor.get());
    textEditor->setExplicitFocusOrder (1);
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (String());

    textEditor->setBounds (297, 296, 320, 24);

    label.reset (new Label ("new label",
                            TRANS("Please enter a name for the preset")));
    addAndMakeVisible (label.get());
    label->setFont (Font (24.30f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centred);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0xffe28600));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label->setBounds (257, 248, 400, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (910, 600);


    //[Constructor] You can add your own custom stuff here..
    label->setColour (TextEditor::textColourId, Colours::darkorange);
    //[/Constructor]
}

PresetDialog::~PresetDialog()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    okButton = nullptr;
    cancelButton = nullptr;
    textEditor = nullptr;
    label = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PresetDialog::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff1d1f20));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PresetDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PresetDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == okButton.get())
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        String presetName = this->textEditor->getTextValue().toString();
        String appDataPath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName();

        String basePath = appDataPath + "/Audio/Presets/pueski/";
        String presetPath = basePath +"Lupo/";

        File baseDir = File(basePath);
        File presetDir = File(presetPath);

        if (!baseDir.exists()) {
            baseDir.createDirectory();
        }

        if (!presetDir.exists()){
            presetDir.createDirectory();
        }

        File preset = File(presetPath + presetName+".xml");

        bool proceed = true;
        bool presetExists = false;

        if (preset.exists()) {
            proceed = false;
            presetExists = true;
        }
        /*
        if (!proceed) {
            proceed = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Warning", "A preset with this name exists already, overwrite?", "Ok", "Fuck! No!",this);
        }
         */

        if (proceed) {

            xml.get()->writeToFile(preset,"");

            if (!presetExists) {

                int itemId = 0;

                for (int i = 0; i < presetBox->getNumItems();i++) {
                    if (presetBox->getItemId(i) > itemId) {
                        itemId = presetBox->getItemId(i);
                    }
                }

                itemId++;

                this->presetBox->addItem(presetName,itemId);
                // this->presetBox->setSelectedId(itemId);

            }

            xml = nullptr;
            setVisible(false);

        }

        //[/UserButtonCode_okButton]
    }
    else if (buttonThatWasClicked == cancelButton.get())
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        setVisible(false);
        //[/UserButtonCode_cancelButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PresetDialog::setData(ScopedPointer<juce::XmlElement> xml) {
    this->xml = xml;
}
void PresetDialog::visibilityChanged() {
    if (isVisible()) {
        this->textEditor->setWantsKeyboardFocus(true);
        this->textEditor->grabKeyboardFocus();
    }
}


void PresetDialog::setVisible (bool shouldBeVisible) {
    Component::setVisible(shouldBeVisible);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PresetDialog" componentName=""
                 parentClasses="public Component" constructorParams="ComboBox* presetBox, Model* model"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="910" initialHeight="600">
  <BACKGROUND backgroundColour="ff1d1f20"/>
  <TEXTBUTTON name="okButton" id="49a4f2dd33e5242c" memberName="okButton" virtualName=""
              explicitFocusOrder="2" pos="289 352 150 24" buttonText="Ok" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="cancelButton" id="a405d41efa6d9eac" memberName="cancelButton"
              virtualName="" explicitFocusOrder="3" pos="473 352 150 24" buttonText="Cancel"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTEDITOR name="textEditor" id="c0cb19bb007bf5b3" memberName="textEditor"
              virtualName="" explicitFocusOrder="1" pos="297 296 320 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="99e05866c15b2314" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="257 248 400 24" textCol="ffe28600"
         edTextCol="ff000000" edBkgCol="0" labelText="Please enter a name for the preset"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="24.3" kerning="0.0" bold="0"
         italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

