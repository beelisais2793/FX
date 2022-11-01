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

//[Headers] You can add your own extra header files here...
#include "AttachmentFactory.h"
#include "Model.h"
#include "ModMatrix.h"
//[/Headers]

#include "ModPanel.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ModPanel::ModPanel (ModMatrix* matrix,Model* model, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->model = model;
	this->factory = factory;
	this->matrix = matrix;
    //[/Constructor_pre]

    Source_1.reset (new ComboBox ("Source_1"));
    addAndMakeVisible (Source_1.get());
    Source_1->setEditableText (false);
    Source_1->setJustificationType (Justification::centredLeft);
    Source_1->setTextWhenNothingSelected (String());
    Source_1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Source_1->addListener (this);

    Source_1->setBounds (8, 40, 100, 24);

    Source_2.reset (new ComboBox ("Source_2"));
    addAndMakeVisible (Source_2.get());
    Source_2->setEditableText (false);
    Source_2->setJustificationType (Justification::centredLeft);
    Source_2->setTextWhenNothingSelected (String());
    Source_2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Source_2->addListener (this);

    Source_2->setBounds (8, 72, 100, 24);

    Source_3.reset (new ComboBox ("Source_3"));
    addAndMakeVisible (Source_3.get());
    Source_3->setEditableText (false);
    Source_3->setJustificationType (Justification::centredLeft);
    Source_3->setTextWhenNothingSelected (String());
    Source_3->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Source_3->addListener (this);

    Source_3->setBounds (8, 104, 100, 24);

    Source_4.reset (new ComboBox ("Source_4"));
    addAndMakeVisible (Source_4.get());
    Source_4->setEditableText (false);
    Source_4->setJustificationType (Justification::centredLeft);
    Source_4->setTextWhenNothingSelected (String());
    Source_4->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Source_4->addListener (this);

    Source_4->setBounds (8, 136, 100, 24);

    Source_5.reset (new ComboBox ("Source_5"));
    addAndMakeVisible (Source_5.get());
    Source_5->setEditableText (false);
    Source_5->setJustificationType (Justification::centredLeft);
    Source_5->setTextWhenNothingSelected (String());
    Source_5->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Source_5->addListener (this);

    Source_5->setBounds (8, 168, 100, 24);

    Source_0.reset (new ComboBox ("Source_0"));
    addAndMakeVisible (Source_0.get());
    Source_0->setEditableText (false);
    Source_0->setJustificationType (Justification::centredLeft);
    Source_0->setTextWhenNothingSelected (String());
    Source_0->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Source_0->addListener (this);

    Source_0->setBounds (8, 8, 100, 24);

    Target_0.reset (new ComboBox ("Target_0"));
    addAndMakeVisible (Target_0.get());
    Target_0->setEditableText (false);
    Target_0->setJustificationType (Justification::centredLeft);
    Target_0->setTextWhenNothingSelected (String());
    Target_0->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Target_0->addListener (this);

    Target_0->setBounds (120, 8, 100, 24);

    Target_1.reset (new ComboBox ("Target_1"));
    addAndMakeVisible (Target_1.get());
    Target_1->setEditableText (false);
    Target_1->setJustificationType (Justification::centredLeft);
    Target_1->setTextWhenNothingSelected (String());
    Target_1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Target_1->addListener (this);

    Target_1->setBounds (120, 40, 100, 24);

    Target_2.reset (new ComboBox ("Target_2"));
    addAndMakeVisible (Target_2.get());
    Target_2->setEditableText (false);
    Target_2->setJustificationType (Justification::centredLeft);
    Target_2->setTextWhenNothingSelected (String());
    Target_2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Target_2->addListener (this);

    Target_2->setBounds (120, 72, 100, 24);

    Target_3.reset (new ComboBox ("Target_3"));
    addAndMakeVisible (Target_3.get());
    Target_3->setEditableText (false);
    Target_3->setJustificationType (Justification::centredLeft);
    Target_3->setTextWhenNothingSelected (String());
    Target_3->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Target_3->addListener (this);

    Target_3->setBounds (120, 104, 100, 24);

    Target_4.reset (new ComboBox ("Target_4"));
    addAndMakeVisible (Target_4.get());
    Target_4->setEditableText (false);
    Target_4->setJustificationType (Justification::centredLeft);
    Target_4->setTextWhenNothingSelected (String());
    Target_4->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Target_4->addListener (this);

    Target_4->setBounds (120, 136, 100, 24);

    Target_5.reset (new ComboBox ("Target_5"));
    addAndMakeVisible (Target_5.get());
    Target_5->setEditableText (false);
    Target_5->setJustificationType (Justification::centredLeft);
    Target_5->setTextWhenNothingSelected (String());
    Target_5->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    Target_5->addListener (this);

    Target_5->setBounds (120, 168, 100, 24);

    Amount_0.reset (new Slider ("Amount_0"));
    addAndMakeVisible (Amount_0.get());
    Amount_0->setRange (0, 10, 0);
    Amount_0->setSliderStyle (Slider::LinearBar);
    Amount_0->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    Amount_0->addListener (this);

    Amount_0->setBounds (232, 8, 232, 24);

    Amount_1.reset (new Slider ("Amount_1"));
    addAndMakeVisible (Amount_1.get());
    Amount_1->setRange (0, 10, 0);
    Amount_1->setSliderStyle (Slider::LinearBar);
    Amount_1->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    Amount_1->addListener (this);

    Amount_1->setBounds (232, 40, 232, 24);

    Amount_2.reset (new Slider ("Amount_2"));
    addAndMakeVisible (Amount_2.get());
    Amount_2->setRange (0, 10, 0);
    Amount_2->setSliderStyle (Slider::LinearBar);
    Amount_2->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    Amount_2->addListener (this);

    Amount_2->setBounds (232, 72, 232, 24);

    Amount_3.reset (new Slider ("Amount_3"));
    addAndMakeVisible (Amount_3.get());
    Amount_3->setRange (0, 10, 0);
    Amount_3->setSliderStyle (Slider::LinearBar);
    Amount_3->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    Amount_3->addListener (this);

    Amount_3->setBounds (232, 104, 232, 24);

    Amount_4.reset (new Slider ("Amount_4"));
    addAndMakeVisible (Amount_4.get());
    Amount_4->setRange (0, 10, 0);
    Amount_4->setSliderStyle (Slider::LinearBar);
    Amount_4->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    Amount_4->addListener (this);

    Amount_4->setBounds (232, 136, 232, 24);

    Amount_5.reset (new Slider ("Amount_5"));
    addAndMakeVisible (Amount_5.get());
    Amount_5->setRange (0, 10, 0);
    Amount_5->setSliderStyle (Slider::LinearBar);
    Amount_5->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    Amount_5->addListener (this);

    Amount_5->setBounds (232, 168, 232, 24);


    //[UserPreSize]
	populateSources(Source_0.get());
	populateSources(Source_1.get());
	populateSources(Source_2.get());
	populateSources(Source_3.get());
	populateSources(Source_4.get());
	populateSources(Source_5.get());

	populateTargets(Target_0.get());
	populateTargets(Target_1.get());
	populateTargets(Target_2.get());
	populateTargets(Target_3.get());
	populateTargets(Target_4.get());
	populateTargets(Target_5.get());

    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ModPanel::~ModPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    Source_1 = nullptr;
    Source_2 = nullptr;
    Source_3 = nullptr;
    Source_4 = nullptr;
    Source_5 = nullptr;
    Source_0 = nullptr;
    Target_0 = nullptr;
    Target_1 = nullptr;
    Target_2 = nullptr;
    Target_3 = nullptr;
    Target_4 = nullptr;
    Target_5 = nullptr;
    Amount_0 = nullptr;
    Amount_1 = nullptr;
    Amount_2 = nullptr;
    Amount_3 = nullptr;
    Amount_4 = nullptr;
    Amount_5 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ModPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ModPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ModPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged->getSelectedItemIndex() < 1) {
		return;
	}

    //[UsercomboBoxChanged_Pre]

    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == Source_0.get())
    {
        Modulator* modulator = matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Source_0] -- add your combo box handling code here..
        matrix->getModulations()[0]->getTarget()->setModulator(modulator);
		matrix->getModulations()[0]->setModulator(modulator);
        
        //[/UserComboBoxCode_Source_0]
    }
    else if (comboBoxThatHasChanged == Source_1.get())
    {
	    Modulator* modulator = matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Source_1] -- add your combo box handling code here..
        matrix->getModulations()[1]->getTarget()->setModulator(modulator);
        matrix->getModulations()[1]->setModulator(modulator);
        //[/UserComboBoxCode_Source_1]
    }
    else if (comboBoxThatHasChanged == Source_2.get())
    {
        Modulator* modulator = matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Source_2] -- add your combo box handling code here..
        matrix->getModulations()[2]->getTarget()->setModulator(modulator);
		matrix->getModulations()[2]->setModulator(modulator);
        //[/UserComboBoxCode_Source_2]
    }
    else if (comboBoxThatHasChanged == Source_3.get())
    {
        Modulator* modulator = matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Source_3] -- add your combo box handling code here..
        matrix->getModulations()[3]->getTarget()->setModulator(modulator);
		matrix->getModulations()[3]->setModulator(modulator);
        //[/UserComboBoxCode_Source_3]
    }
    else if (comboBoxThatHasChanged == Source_4.get())
    {
        Modulator* modulator = matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Source_4] -- add your combo box handling code here..
        matrix->getModulations()[4]->getTarget()->setModulator(modulator);
		matrix->getModulations()[4]->setModulator(modulator);
        //[/UserComboBoxCode_Source_4]
    }
    else if (comboBoxThatHasChanged == Source_5.get())
    {
        Modulator* modulator = matrix->getModulators().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Source_5] -- add your combo box handling code here..
        matrix->getModulations()[5]->getTarget()->setModulator(modulator);
		matrix->getModulations()[5]->setModulator(modulator);
        //[/UserComboBoxCode_Source_5]
    }
    else if (comboBoxThatHasChanged == Target_0.get())
    {
        ModTarget* target = matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Target_0] -- add your combo box handling code here..

        Modulator* modulator = matrix->getModulations()[0]->getTarget()->getModulator();
        matrix->getModulations()[0]->getTarget()->setModulator(nullptr);
        target->setModulator(modulator);
        matrix->getModulations()[1]->setTarget(target);
        //[/UserComboBoxCode_Target_0]
    }
    else if (comboBoxThatHasChanged == Target_1.get())
    {
        ModTarget* target = matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Target_1] -- add your combo box handling code here..
        Modulator* modulator = matrix->getModulations()[1]->getTarget()->getModulator();
        matrix->getModulations()[1]->getTarget()->setModulator(nullptr);
        target->setModulator(modulator);

		matrix->getModulations()[1]->setTarget(target);
        //[/UserComboBoxCode_Target_1]
    }
    else if (comboBoxThatHasChanged == Target_2.get())
    {
        ModTarget* target = matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Target_2] -- add your combo box handling code here..
        Modulator* modulator = matrix->getModulations()[2]->getTarget()->getModulator();
        matrix->getModulations()[2]->getTarget()->setModulator(nullptr);
        target->setModulator(modulator);

		matrix->getModulations()[2]->setTarget(target);
        //[/UserComboBoxCode_Target_2]
    }
    else if (comboBoxThatHasChanged == Target_3.get())
    {
        ModTarget* target = matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Target_3] -- add your combo box handling code here..
        Modulator* modulator = matrix->getModulations()[3]->getTarget()->getModulator();
        matrix->getModulations()[3]->getTarget()->setModulator(nullptr);
        target->setModulator(modulator);

		matrix->getModulations()[3]->setTarget(target);
        //[/UserComboBoxCode_Target_3]
    }
    else if (comboBoxThatHasChanged == Target_4.get())
    {
        ModTarget* target = matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Target_4] -- add your combo box handling code here..
        Modulator* modulator = matrix->getModulations()[4]->getTarget()->getModulator();
        matrix->getModulations()[4]->getTarget()->setModulator(nullptr);
        target->setModulator(modulator);

		matrix->getModulations()[4]->setTarget(target);
        //[/UserComboBoxCode_Target_4]
    }
    else if (comboBoxThatHasChanged == Target_5.get())
    {
        ModTarget* target = matrix->getModTargets().at(comboBoxThatHasChanged->getSelectedItemIndex());
        //[UserComboBoxCode_Target_5] -- add your combo box handling code here..
        Modulator* modulator = matrix->getModulations()[5]->getTarget()->getModulator();
        matrix->getModulations()[5]->getTarget()->setModulator(nullptr);
        target->setModulator(modulator);

		matrix->getModulations()[5]->setTarget(target);
        //[/UserComboBoxCode_Target_5]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void ModPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == Amount_0.get())
    {
        //[UserSliderCode_Amount_0] -- add your slider handling code here..
		if (matrix->getModulations()[0]->getModulator() != nullptr)
			matrix->getModulations()[0]->getModulator()->setModAmount(sliderThatWasMoved->getValue());
        //[/UserSliderCode_Amount_0]
    }
    else if (sliderThatWasMoved == Amount_1.get())
    {
        //[UserSliderCode_Amount_1] -- add your slider handling code here..
		if (matrix->getModulations()[1]->getModulator() != nullptr)
			matrix->getModulations()[1]->getModulator()->setModAmount(sliderThatWasMoved->getValue());
        //[/UserSliderCode_Amount_1]
    }
    else if (sliderThatWasMoved == Amount_2.get())
    {
        //[UserSliderCode_Amount_2] -- add your slider handling code here..
		if (matrix->getModulations()[2]->getModulator() != nullptr)
			matrix->getModulations()[2]->getModulator()->setModAmount(sliderThatWasMoved->getValue());
        //[/UserSliderCode_Amount_2]
    }
    else if (sliderThatWasMoved == Amount_3.get())
    {
        //[UserSliderCode_Amount_3] -- add your slider handling code here..
		if (matrix->getModulations()[3]->getModulator() != nullptr)
			matrix->getModulations()[3]->getModulator()->setModAmount(sliderThatWasMoved->getValue());
        //[/UserSliderCode_Amount_3]
    }
    else if (sliderThatWasMoved == Amount_4.get())
    {
        //[UserSliderCode_Amount_4] -- add your slider handling code here..
		if (matrix->getModulations()[4]->getModulator() != nullptr)
			matrix->getModulations()[4]->getModulator()->setModAmount(sliderThatWasMoved->getValue());
        //[/UserSliderCode_Amount_4]
    }
    else if (sliderThatWasMoved == Amount_5.get())
    {
        //[UserSliderCode_Amount_5] -- add your slider handling code here..
		if (matrix->getModulations()[5]->getModulator() != nullptr)
			matrix->getModulations()[5]->getModulator()->setModAmount(sliderThatWasMoved->getValue());
        //[/UserSliderCode_Amount_5]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void ModPanel::initAttachments()
{
	factory->createComboAttachment("Source_0", Source_0.get());
	factory->createComboAttachment("Source_1", Source_1.get());
	factory->createComboAttachment("Source_2", Source_2.get());
	factory->createComboAttachment("Source_3", Source_3.get());
	factory->createComboAttachment("Source_4", Source_4.get());
	factory->createComboAttachment("Source_5", Source_5.get());

	factory->createComboAttachment("Target_0", Target_0.get());
	factory->createComboAttachment("Target_1", Target_1.get());
	factory->createComboAttachment("Target_2", Target_2.get());
	factory->createComboAttachment("Target_3", Target_3.get());
	factory->createComboAttachment("Target_4", Target_4.get());
	factory->createComboAttachment("Target_5", Target_5.get());

	factory->createSliderAttachment("Amount_0", Amount_0.get());
	factory->createSliderAttachment("Amount_1", Amount_1.get());
	factory->createSliderAttachment("Amount_2", Amount_2.get());
	factory->createSliderAttachment("Amount_3", Amount_3.get());
	factory->createSliderAttachment("Amount_4", Amount_4.get());
	factory->createSliderAttachment("Amount_5", Amount_5.get());

}
void ModPanel::populateSources(ComboBox * combo)
{
	for (std::map<int, String>::iterator it = matrix->getSources()->begin(); it != matrix->getSources()->end();) {
		combo->addItem(it->second, it->first);
		it++;
	}

}
void ModPanel::populateTargets(ComboBox * combo)
{
	for (std::map<int, String>::iterator it = matrix->getTargets()->begin(); it != matrix->getTargets()->end();) {
		combo->addItem(it->second, it->first);
		it++;
	}
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModPanel" componentName=""
                 parentClasses="public Component" constructorParams="ModMatrix* matrix,Model* model, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <COMBOBOX name="Source_1" id="9505a7b158d3a69f" memberName="Source_1" virtualName=""
            explicitFocusOrder="0" pos="8 40 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Source_2" id="2be8322b975af6c6" memberName="Source_2" virtualName=""
            explicitFocusOrder="0" pos="8 72 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Source_3" id="f1f1045a90519d5b" memberName="Source_3" virtualName=""
            explicitFocusOrder="0" pos="8 104 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Source_4" id="caa7c635bdcabd9a" memberName="Source_4" virtualName=""
            explicitFocusOrder="0" pos="8 136 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Source_5" id="40faa7b1e68f4acd" memberName="Source_5" virtualName=""
            explicitFocusOrder="0" pos="8 168 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Source_0" id="bc83b8154feb9a7b" memberName="Source_0" virtualName=""
            explicitFocusOrder="0" pos="8 8 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Target_0" id="3eb156cdc793cc83" memberName="Target_0" virtualName=""
            explicitFocusOrder="0" pos="120 8 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Target_1" id="f2bbe2cc58c5d401" memberName="Target_1" virtualName=""
            explicitFocusOrder="0" pos="120 40 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Target_2" id="3e76ee0604cfb170" memberName="Target_2" virtualName=""
            explicitFocusOrder="0" pos="120 72 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Target_3" id="6d69abc048bdface" memberName="Target_3" virtualName=""
            explicitFocusOrder="0" pos="120 104 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Target_4" id="6ac98b77f668a6b4" memberName="Target_4" virtualName=""
            explicitFocusOrder="0" pos="120 136 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Target_5" id="b5c91e009118f4a6" memberName="Target_5" virtualName=""
            explicitFocusOrder="0" pos="120 168 100 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <SLIDER name="Amount_0" id="26600c3d4c130021" memberName="Amount_0" virtualName=""
          explicitFocusOrder="0" pos="232 8 232 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Amount_1" id="3bcc52a0f6d2c978" memberName="Amount_1" virtualName=""
          explicitFocusOrder="0" pos="232 40 232 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Amount_2" id="9de9e14e7e9a7b2a" memberName="Amount_2" virtualName=""
          explicitFocusOrder="0" pos="232 72 232 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Amount_3" id="e4e9409760323df7" memberName="Amount_3" virtualName=""
          explicitFocusOrder="0" pos="232 104 232 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Amount_4" id="1b85c0938b624bdd" memberName="Amount_4" virtualName=""
          explicitFocusOrder="0" pos="232 136 232 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Amount_5" id="a8bd00acc32cf980" memberName="Amount_5" virtualName=""
          explicitFocusOrder="0" pos="232 168 232 24" min="0.0" max="10.0"
          int="0.0" style="LinearBar" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

