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
#include "AudioEngine/LupoSynth.h"
#include "OscillatorPanel.h"
#include "LFOPanel.h"
#include "ChorusPanel.h"
#include "Model.h"
#include "AttachmentFactory.h"
#include "PresetDialog.h"
#include "AudioEngine/Oszillator.h"
#include "Panel.h"
#include "ModMatrixPanel.h"
#include <math.h>
//[/Headers]

#include "MainUI.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MainUI::MainUI (LupoAudioProcessor* processor, AttachmentFactory* factory)
{
    //[Constructor_pre] You can add your own custom stuff here..
	this->processor = processor;
	this->model = processor->getModel();
	this->synth = processor->getSynth();
	this->matrixModel = new ModMatrixModel(this->synth->getModMatrix(), factory);
	Arpeggiator* arp = this->synth->getArpeggiator();
	Logger::getCurrentLogger()->writeToLog("GUI instance created.");

	for (int i = 0; i < factory->getSliderParams().size(); i++) {
		processor->parameters->addParameterListener(factory->getSliderParams().getReference(i),this);
	}
	for (int i = 0; i < 6; i++) {
		processor->parameters->addParameterListener("Source_" + String(i), this);
		processor->parameters->addParameterListener("Target_" + String(i), this);
		processor->parameters->addParameterListener("Amount_" + String(i), this);
	}
    //[/Constructor_pre]

    GlassPanel.reset (new Panel());
    addAndMakeVisible (GlassPanel.get());
    GlassPanel->setName ("GlassPanel");

    GlassPanel->setBounds (0, 0, 992, 840);

    ModulationGroup.reset (new GroupComponent ("ModulationGroup",
                                               TRANS("Modulation")));
    addAndMakeVisible (ModulationGroup.get());

    ModulationGroup->setBounds (8, 544, 792, 144);

    groupComponent.reset (new GroupComponent ("new group",
                                              TRANS("Amplifier")));
    addAndMakeVisible (groupComponent.get());

    groupComponent->setBounds (592, 48, 392, 104);

    groupComponent3.reset (new GroupComponent ("new group",
                                               TRANS("Oscilators")));
    addAndMakeVisible (groupComponent3.get());

    groupComponent3->setBounds (8, 48, 456, 496);

    filterGroup1.reset (new GroupComponent ("filterGroup1",
                                            TRANS("Filter 1")));
    addAndMakeVisible (filterGroup1.get());

    filterGroup1->setBounds (592, 208, 392, 112);

    osc1Panel.reset (new OscillatorPanel (model, factory));
    addAndMakeVisible (osc1Panel.get());
    osc1Panel->setName ("osc1Panel");

    osc1Panel->setBounds (16, 64, 216, 232);

    osc3Panel.reset (new OscillatorPanel (model, factory));
    addAndMakeVisible (osc3Panel.get());
    osc3Panel->setName ("osc3Panel");

    osc3Panel->setBounds (16, 304, 216, 232);

    osc2Panel.reset (new OscillatorPanel (model, factory));
    addAndMakeVisible (osc2Panel.get());
    osc2Panel->setName ("osc2Panel");

    osc2Panel->setBounds (240, 64, 216, 232);

    ampEnvelope.reset (new EnvelopePanel (model, factory));
    addAndMakeVisible (ampEnvelope.get());
    ampEnvelope->setName ("ampEnvelope");

    ampEnvelope->setBounds (608, 64, 288, 80);

    mainVolume.reset (new Slider ("mainVolume"));
    addAndMakeVisible (mainVolume.get());
    mainVolume->setRange (0, 2, 0.01);
    mainVolume->setSliderStyle (Slider::RotaryVerticalDrag);
    mainVolume->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    mainVolume->addListener (this);

    mainVolume->setBounds (904, 72, 56, 56);

    volumeLabel.reset (new Label ("volumeLabel",
                                  TRANS("Main volume")));
    addAndMakeVisible (volumeLabel.get());
    volumeLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    volumeLabel->setJustificationType (Justification::centredLeft);
    volumeLabel->setEditable (false, false, false);
    volumeLabel->setColour (TextEditor::textColourId, Colours::black);
    volumeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    volumeLabel->setBounds (904, 120, 64, 24);

    mixerGroiup.reset (new GroupComponent ("mixerGroiup",
                                           TRANS("Mixer")));
    addAndMakeVisible (mixerGroiup.get());

    mixerGroiup->setBounds (464, 48, 128, 496);

    ch1Panel.reset (new MixerChannelPanel (model, factory));
    addAndMakeVisible (ch1Panel.get());
    ch1Panel->setName ("channel1");

    ch1Panel->setBounds (472, 64, 112, 96);

    ch2Panel.reset (new MixerChannelPanel (model, factory));
    addAndMakeVisible (ch2Panel.get());
    ch2Panel->setName ("channel2");

    ch2Panel->setBounds (472, 160, 112, 96);

    ch3Panel.reset (new MixerChannelPanel (model, factory));
    addAndMakeVisible (ch3Panel.get());
    ch3Panel->setName ("channel3");

    ch3Panel->setBounds (472, 256, 112, 96);

    ch4Panel.reset (new MixerChannelPanel (model, factory));
    addAndMakeVisible (ch4Panel.get());
    ch4Panel->setName ("channel4");

    ch4Panel->setBounds (472, 352, 112, 96);

    osc4Panel.reset (new OscillatorPanel (model, factory));
    addAndMakeVisible (osc4Panel.get());
    osc4Panel->setName ("osc4Panel");

    osc4Panel->setBounds (240, 304, 216, 232);

    FXGroup.reset (new GroupComponent ("FXGroup",
                                       TRANS("FX")));
    addAndMakeVisible (FXGroup.get());

    FXGroup->setBounds (8, 688, 976, 136);

    reverbPanel.reset (new ReverbPanel (model, factory));
    addAndMakeVisible (reverbPanel.get());
    reverbPanel->setName ("reverbPanel");

    reverbPanel->setBounds (16, 704, 352, 112);

    delayPanel.reset (new DelayPanel (model, factory));
    addAndMakeVisible (delayPanel.get());
    delayPanel->setName ("delayPanel");

    delayPanel->setBounds (376, 704, 264, 112);

    chorusPanel.reset (new ChorusPanel (model, factory));
    addAndMakeVisible (chorusPanel.get());
    chorusPanel->setName ("new component");

    chorusPanel->setBounds (648, 704, 320, 112);

    presetButton.reset (new TextButton ("presetButton"));
    addAndMakeVisible (presetButton.get());
    presetButton->setButtonText (TRANS("Presets"));
    presetButton->addListener (this);
    presetButton->setColour (TextButton::buttonColourId, Colours::grey);

    presetButton->setBounds (16, 16, 64, 24);

    saveButton.reset (new TextButton ("saveButton"));
    addAndMakeVisible (saveButton.get());
    saveButton->setButtonText (TRANS("Save"));
    saveButton->addListener (this);
    saveButton->setColour (TextButton::buttonColourId, Colours::grey);

    saveButton->setBounds (320, 16, 64, 24);

    presetCombo.reset (new ComboBox ("presetCombo"));
    addAndMakeVisible (presetCombo.get());
    presetCombo->setEditableText (true);
    presetCombo->setJustificationType (Justification::centredLeft);
    presetCombo->setTextWhenNothingSelected (String());
    presetCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    presetCombo->addListener (this);

    presetCombo->setBounds (88, 16, 224, 24);

    fmSlider.reset (new Slider ("fmSlider"));
    addAndMakeVisible (fmSlider.get());
    fmSlider->setRange (0, 1, 0);
    fmSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    fmSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    fmSlider->addListener (this);

    fmSlider->setBounds (496, 464, 64, 56);

    distGroup.reset (new GroupComponent ("distGroup",
                                         TRANS("Distortion")));
    addAndMakeVisible (distGroup.get());

    distGroup->setBounds (592, 456, 392, 88);

    distortionPanel.reset (new DistortionPanel (model, factory));
    addAndMakeVisible (distortionPanel.get());
    distortionPanel->setName ("distortionPanel");

    distortionPanel->setBounds (608, 472, 288, 64);

    arpGroup.reset (new GroupComponent ("arpGroup",
                                        TRANS("Arpeggiator")));
    addAndMakeVisible (arpGroup.get());

    arpGroup->setBounds (800, 544, 184, 144);

    arpPanel.reset (new ArpPanel (factory,arp));
    addAndMakeVisible (arpPanel.get());
    arpPanel->setName ("arpPanel");

    arpPanel->setBounds (808, 560, 168, 120);

    modulationTab.reset (new TabbedComponent (TabbedButtonBar::TabsAtBottom));
    addAndMakeVisible (modulationTab.get());
    modulationTab->setTabBarDepth (22);
    modulationTab->addTab (TRANS("LFO 1"), Colour (0x00000000), new LFOPanel (model, factory), true);
    modulationTab->addTab (TRANS("LFO 2"), Colour (0x00000000), new LFOPanel (model, factory), true);
    modulationTab->addTab (TRANS("LFO 3"), Colour (0x00000000), new LFOPanel (model, factory), true);
    modulationTab->addTab (TRANS("ENV 1"), Colour (0x00000000), new EnvelopePanel (model, factory), true);
    modulationTab->addTab (TRANS("ENV 2"), Colour (0x00000000), new EnvelopePanel (model, factory), true);
    modulationTab->setCurrentTabIndex (0);

    modulationTab->setBounds (24, 568, 264, 120);

    filterPanel1.reset (new FilterPanel (model, factory));
    addAndMakeVisible (filterPanel1.get());
    filterPanel1->setName ("filterPanel1");

    filterPanel1->setBounds (608, 224, 360, 88);

    filterGroup2.reset (new GroupComponent ("filterGroup2",
                                            TRANS("Filter 2")));
    addAndMakeVisible (filterGroup2.get());

    filterGroup2->setBounds (592, 344, 392, 112);

    filterPanel2.reset (new FilterPanel (model, factory));
    addAndMakeVisible (filterPanel2.get());
    filterPanel2->setName ("filterPanel2");

    filterPanel2->setBounds (608, 360, 360, 80);

    filterModeLabel.reset (new Label ("filterModeLabel",
                                      TRANS("Filter mode")));
    addAndMakeVisible (filterModeLabel.get());
    filterModeLabel->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    filterModeLabel->setJustificationType (Justification::centredLeft);
    filterModeLabel->setEditable (false, false, false);
    filterModeLabel->setColour (TextEditor::textColourId, Colours::black);
    filterModeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    filterModeLabel->setBounds (824, 324, 64, 24);

    filterModeCombo.reset (new ComboBox ("new combo box"));
    addAndMakeVisible (filterModeCombo.get());
    filterModeCombo->setEditableText (false);
    filterModeCombo->setJustificationType (Justification::centredLeft);
    filterModeCombo->setTextWhenNothingSelected (String());
    filterModeCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    filterModeCombo->addItem (TRANS("Serial"), 1);
    filterModeCombo->addItem (TRANS("Parallel"), 2);
    filterModeCombo->addListener (this);

    filterModeCombo->setBounds (896, 324, 88, 24);

    cutoffLink.reset (new ToggleButton ("cutoffLink"));
    addAndMakeVisible (cutoffLink.get());
    cutoffLink->setButtonText (String());
    cutoffLink->addListener (this);

    cutoffLink->setBounds (688, 324, 32, 24);

    label.reset (new Label ("new label",
                            TRANS("Cutoff link")));
    addAndMakeVisible (label.get());
    label->setFont (Font (12.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    label->setBounds (720, 324, 64, 24);

    mainDisplay.reset (new TextEditor ("mainDisplay"));
    addAndMakeVisible (mainDisplay.get());
    mainDisplay->setMultiLine (false);
    mainDisplay->setReturnKeyStartsNewLine (false);
    mainDisplay->setReadOnly (true);
    mainDisplay->setScrollbarsShown (false);
    mainDisplay->setCaretVisible (false);
    mainDisplay->setPopupMenuEnabled (false);
    mainDisplay->setColour (TextEditor::textColourId, Colour (0xffff4400));
    mainDisplay->setColour (TextEditor::backgroundColourId, Colours::black);
    mainDisplay->setText (String());

    mainDisplay->setBounds (592, 152, 392, 56);

    Viewport* vp = new Viewport("ModMatrix");
    vp->setBounds(296, 560, 496, 120);
    modMatrix.reset (new ModPanel (this->synth->getModMatrix(), model, factory));

    vp->setViewedComponent(modMatrix.get());
    vp->setScrollBarsShown(true, false);
    addAndMakeVisible (vp);
    modMatrix->setName ("modMatrix");

    modMatrix->setBounds (0, 0, 496, 240);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (1300, 900);

    //[Constructor] You can add your own custom stuff here..

	int x = getScreenX();
	int y = getScreenY();
	dlg = new PresetDialog(presetCombo.get(), model);
	dlg->setBounds(x, y, getWidth(), getHeight());
	addChildComponent(dlg);

	osc1Panel.get()->SetTitle("Osc 1");
	osc2Panel.get()->SetTitle("Osc 2");
	osc3Panel.get()->SetTitle("Osc 3");
	osc4Panel.get()->SetTitle("Osc 4");

	ch1Panel.get()->SetTitle("Ch 1");
	ch2Panel.get()->SetTitle("Ch 2");
	ch3Panel.get()->SetTitle("Ch 3");
	ch4Panel.get()->SetTitle("Ch 4");

	osc1Panel.get()->addChangeListener(this);
	osc2Panel.get()->addChangeListener(this);
	osc3Panel.get()->addChangeListener(this);
	osc4Panel.get()->addChangeListener(this);

	ch1Panel.get()->addChangeListener(this);
	ch2Panel.get()->addChangeListener(this);
	ch3Panel.get()->addChangeListener(this);
	ch4Panel.get()->addChangeListener(this);

	ampEnvelope.get()->addChangeListener(this);

	LFOPanel* lfo1 = dynamic_cast<LFOPanel*> (modulationTab.get()->getTabContentComponent(0));
	LFOPanel* lfo2 = dynamic_cast<LFOPanel*> (modulationTab.get()->getTabContentComponent(1));
	LFOPanel* lfo3 = dynamic_cast<LFOPanel*> (modulationTab.get()->getTabContentComponent(2));

	EnvelopePanel* auxEnvelope1 = dynamic_cast<EnvelopePanel*> (modulationTab.get()->getTabContentComponent(3));
	auxEnvelope1->setName("auxEnvelope1");

	EnvelopePanel* auxEnvelope2 = dynamic_cast<EnvelopePanel*> (modulationTab.get()->getTabContentComponent(4));
	auxEnvelope2->setName("auxEnvelope2");


	auxEnvelope1->addChangeListener(this);
	auxEnvelope2->addChangeListener(this);
	delayPanel.get()->addChangeListener(this);
	reverbPanel.get()->addChangeListener(this);
	chorusPanel.get()->addChangeListener(this);

	addChangeListener(synth);
	// addMouseListener(this, true);

	factory->initState();

	factory->createSliderAttachment("fmAmount", fmSlider.get());
	factory->createSliderAttachment("mainVolume", mainVolume.get());
	factory->createComboAttachment("filterMode", filterModeCombo.get());

	factory->createButtonAttachment("cutoffLink", cutoffLink.get());

	filterPanel1->initAttachments();
	filterPanel2->initAttachments();

	osc1Panel.get()->initAttachments();
	osc2Panel.get()->initAttachments();
	osc3Panel.get()->initAttachments();
	osc4Panel.get()->initAttachments();

	ch1Panel.get()->initAttachments();
	ch2Panel.get()->initAttachments();
	ch3Panel.get()->initAttachments();
	ch4Panel.get()->initAttachments();

	ampEnvelope.get()->initAttachments();
	auxEnvelope1->initAttachments();
	auxEnvelope2->initAttachments();

	lfo1->setName("lfo1");
	lfo2->setName("lfo2");
	lfo3->setName("lfo3");
	lfo1->initAttachments();
	lfo2->initAttachments();
	lfo3->initAttachments();

	reverbPanel.get()->initAttachments();
	chorusPanel.get()->initAttachments();
	delayPanel.get()->initAttachments();
	distortionPanel.get()->initAttachments();
	arpPanel.get()->initAttachments();
	modMatrix.get()->initAttachments();

	synth->addChangeListener(this);

	presetCombo->setText("init");
	GlassPanel.get()->setVisible(false);

	mainDisplay.get()->setFont(Font(Typeface::createSystemTypefaceFor(BinaryData::DSEG14ClassicRegular_ttf,
		BinaryData::DSEG14ClassicRegular_ttfSize)));

	mainDisplay.get()->setText("Lupo V1.0");

	resized();

    //[/Constructor]
}

MainUI::~MainUI()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
	removeAllChangeListeners();
	delete dlg;

    //[/Destructor_pre]

    GlassPanel = nullptr;
    ModulationGroup = nullptr;
    groupComponent = nullptr;
    groupComponent3 = nullptr;
    filterGroup1 = nullptr;
    osc1Panel = nullptr;
    osc3Panel = nullptr;
    osc2Panel = nullptr;
    ampEnvelope = nullptr;
    mainVolume = nullptr;
    volumeLabel = nullptr;
    mixerGroiup = nullptr;
    ch1Panel = nullptr;
    ch2Panel = nullptr;
    ch3Panel = nullptr;
    ch4Panel = nullptr;
    osc4Panel = nullptr;
    FXGroup = nullptr;
    reverbPanel = nullptr;
    delayPanel = nullptr;
    chorusPanel = nullptr;
    presetButton = nullptr;
    saveButton = nullptr;
    presetCombo = nullptr;
    fmSlider = nullptr;
    distGroup = nullptr;
    distortionPanel = nullptr;
    arpGroup = nullptr;
    arpPanel = nullptr;
    modulationTab = nullptr;
    filterPanel1 = nullptr;
    filterGroup2 = nullptr;
    filterPanel2 = nullptr;
    filterModeLabel = nullptr;
    filterModeCombo = nullptr;
    cutoffLink = nullptr;
    label = nullptr;
    mainDisplay = nullptr;
    modMatrix = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MainUI::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff606060));

    //[UserPaint] Add your own custom painting code here..

	g.setColour(Colours::white);





    //[/UserPaint]
}

void MainUI::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainUI::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == mainVolume.get())
    {
        //[UserSliderCode_mainVolume] -- add your slider handling code here..
		model->mainVolume = sliderThatWasMoved->getValue();
        //[/UserSliderCode_mainVolume]
    }
    else if (sliderThatWasMoved == fmSlider.get())
    {
        //[UserSliderCode_fmSlider] -- add your slider handling code here..
        //[/UserSliderCode_fmSlider]
    }

    //[UsersliderValueChanged_Post]
	sendChangeMessage();
    //[/UsersliderValueChanged_Post]
}

void MainUI::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == presetButton.get())
    {
        //[UserButtonCode_presetButton] -- add your button handler code here..
        //[/UserButtonCode_presetButton]
    }
    else if (buttonThatWasClicked == saveButton.get())
    {
        //[UserButtonCode_saveButton] -- add your button handler code here..

		processor->getValueTreeState()->getParameter("osc1Shape")->setValue(model->osc1Shape);
		processor->getValueTreeState()->getParameter("osc2Shape")->setValue(model->osc2Shape);
		processor->getValueTreeState()->getParameter("osc3Shape")->setValue(model->osc3Shape);
		processor->getValueTreeState()->getParameter("osc4Shape")->setValue(model->osc4Shape);

		std::unique_ptr<XmlElement> xml(processor->getValueTreeState()->state.createXml());

		String appDataPath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName();

		String basePath = appDataPath + "/Audio/Presets/pueski/";
		String presetPath = basePath + "Lupo/";
		String presetName = presetCombo->getText();

		File baseDir = File(basePath);
		File presetDir = File(presetPath);

		if (!baseDir.exists()) {
			baseDir.createDirectory();
		}

		if (!presetDir.exists()) {
			presetDir.createDirectory();
		}

		File preset = File(presetPath + presetName + ".xml");

		bool proceed = true;
		bool presetExists = false;

		if (preset.exists()) {
			proceed = false;
			presetExists = true;
		}

		if (!proceed) {
			proceed = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Warning", "A preset with this name exists already, overwrite?", "Ok", "Fuck! No!",this);
		}


		if (proceed) {

			xml.get()->writeToFile(preset, "");

			// DEBUG
			ValueTree state = ValueTree::fromXml(*xml.get());

			for (int i = 0; i < state.getNumChildren(); i++) {

				String id = state.getChild(i).getProperty("id").toString();
				String value = state.getChild(i).getProperty("value").toString();
				Logger::getCurrentLogger()->writeToLog("Storing : " + id + " with value " + value);
			}

			if (!presetExists) {

				int itemId = 0;

				for (int i = 0; i < presetCombo->getNumItems(); i++) {
					if (presetCombo->getItemId(i) > itemId) {
						itemId = presetCombo->getItemId(i);
					}
				}

				itemId++;

				// this->presetBox->setSelectedId(itemId);

			}

			xml = nullptr;
		}

		updatePresetList();
        //[/UserButtonCode_saveButton]
    }
    else if (buttonThatWasClicked == cutoffLink.get())
    {
        //[UserButtonCode_cutoffLink] -- add your button handler code here..
        //[/UserButtonCode_cutoffLink]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MainUI::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == presetCombo.get())
    {
        //[UserComboBoxCode_presetCombo] -- add your combo box handling code here..
		processor->setSelectedProgram(comboBoxThatHasChanged->getText());
        //[/UserComboBoxCode_presetCombo]
    }
    else if (comboBoxThatHasChanged == filterModeCombo.get())
    {
        //[UserComboBoxCode_filterModeCombo] -- add your combo box handling code here..
        //[/UserComboBoxCode_filterModeCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void MainUI::changeListenerCallback(ChangeBroadcaster* source) {
	sendChangeMessage();
}

void MainUI::updatePresetList() {
	presetCombo->clear();
	for (int i = 0; i < processor->getNumPrograms(); i++) {
		presetCombo->addItem(processor->getProgramName(i), i + 1);
	}
	for (int i = 0; i < presetCombo.get()->getNumItems(); i++) {
		if (presetCombo.get()->getItemText(i) == processor->selectedProgram) {
			presetCombo->setSelectedItemIndex(i,juce::NotificationType::dontSendNotification);
			break;
		}
	}

}

Component * MainUI::findComponentAtMousePosition(Point<int> mousePos, Component* parent)
{
	for (int i = 0; i < parent->getNumChildComponents(); i++) {
		Component* child = parent->getChildComponent(i);
		if (child->getBounds().contains(mousePos)) {
			return child;
		}
		else {
			return findComponentAtMousePosition(mousePos, child);
		}
	}

	return nullptr;
}

void MainUI::parameterChanged(const String & parameterID, float newValue)
{
	for (int i = 0; i < processor->getNumParameters(); i++) {
		if (processor->getParameterID(i) == parameterID) {
			mainDisplay->setText(processor->getParameters().getUnchecked(i)->getLabel() + " - " + String(newValue,2));
			break;
		}
	}

}



void MainUI::mouseDown(const MouseEvent & event)
{
	/*
	GlassPanel.get()->setVisible(true);
	mouseDownPos = event.getPosition();
	if (event.mods.middleButtonModifier) {
		draggingConnection = true;
		modSource = findComponentAtMousePosition(mouseDownPos, this);
	}
	*/
}

void MainUI::mouseUp(const MouseEvent & event)
{
	/*
	GlassPanel.get()->setVisible(false);
	if (event.mods.middleButtonModifier) {
		draggingConnection = false;
		if (modSource != nullptr && modTarget != nullptr) {
			Logger::getCurrentLogger()->writeToLog("Connection from " + modSource->getName() + " to " + modTarget->getName());
		}

	}
	*/
}

void MainUI::mouseDrag(const MouseEvent & event)
{
	/*
	if (GlassPanel.get()->isVisible()) {
		GlassPanel.get()->updateConnector(mouseDownPos, mousePos);
	}
	mousePos = event.getPosition();
	repaint();
	*/
}

void MainUI::mouseMove(const MouseEvent & event)
{
	/*
	 mousePos = event.getPosition();
	 if (draggingConnection) {
		repaint();
		modTarget = findComponentAtMousePosition(mouseDownPos, this);
	 }
	 */
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainUI" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster, public ChangeListener, public AudioProcessorValueTreeState::Listener"
                 constructorParams="LupoAudioProcessor* processor, AttachmentFactory* factory"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="1300" initialHeight="900">
  <BACKGROUND backgroundColour="ff606060"/>
  <GENERICCOMPONENT name="GlassPanel" id="341b1345fa2fc01f" memberName="GlassPanel"
                    virtualName="" explicitFocusOrder="0" pos="0 0 992 840" class="Panel"
                    params=""/>
  <GROUPCOMPONENT name="ModulationGroup" id="bbbc621090753013" memberName="ModulationGroup"
                  virtualName="" explicitFocusOrder="0" pos="8 544 792 144" title="Modulation"/>
  <GROUPCOMPONENT name="new group" id="fddf59086c1c83a4" memberName="groupComponent"
                  virtualName="" explicitFocusOrder="0" pos="592 48 392 104" title="Amplifier"/>
  <GROUPCOMPONENT name="new group" id="be488b129ef124bc" memberName="groupComponent3"
                  virtualName="" explicitFocusOrder="0" pos="8 48 456 496" title="Oscilators"/>
  <GROUPCOMPONENT name="filterGroup1" id="d21f32d49ecf1836" memberName="filterGroup1"
                  virtualName="" explicitFocusOrder="0" pos="592 208 392 112" title="Filter 1"/>
  <GENERICCOMPONENT name="osc1Panel" id="de6e8c64c8286c8d" memberName="osc1Panel"
                    virtualName="" explicitFocusOrder="0" pos="16 64 216 232" class="OscillatorPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="osc3Panel" id="6bf55931fc2de34d" memberName="osc3Panel"
                    virtualName="" explicitFocusOrder="0" pos="16 304 216 232" class="OscillatorPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="osc2Panel" id="ffc4e13aca23b88e" memberName="osc2Panel"
                    virtualName="" explicitFocusOrder="0" pos="240 64 216 232" class="OscillatorPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="ampEnvelope" id="2e9f4701bbd4595c" memberName="ampEnvelope"
                    virtualName="" explicitFocusOrder="0" pos="608 64 288 80" class="EnvelopePanel"
                    params="model, factory"/>
  <SLIDER name="mainVolume" id="8750195bcc6f4ab5" memberName="mainVolume"
          virtualName="" explicitFocusOrder="0" pos="904 72 56 56" min="0.0"
          max="2.0" int="0.01" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="volumeLabel" id="594cc943c91cef9d" memberName="volumeLabel"
         virtualName="" explicitFocusOrder="0" pos="904 120 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Main volume" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <GROUPCOMPONENT name="mixerGroiup" id="3a76ea384b35b47d" memberName="mixerGroiup"
                  virtualName="" explicitFocusOrder="0" pos="464 48 128 496" title="Mixer"/>
  <GENERICCOMPONENT name="channel1" id="a653fea415c89991" memberName="ch1Panel" virtualName=""
                    explicitFocusOrder="0" pos="472 64 112 96" class="MixerChannelPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="channel2" id="f3b06b13d2a0d971" memberName="ch2Panel" virtualName=""
                    explicitFocusOrder="0" pos="472 160 112 96" class="MixerChannelPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="channel3" id="6786cb4ee8a2b52d" memberName="ch3Panel" virtualName=""
                    explicitFocusOrder="0" pos="472 256 112 96" class="MixerChannelPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="channel4" id="fff0d24cb459d4de" memberName="ch4Panel" virtualName=""
                    explicitFocusOrder="0" pos="472 352 112 96" class="MixerChannelPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="osc4Panel" id="e90f31bb4430d4b4" memberName="osc4Panel"
                    virtualName="" explicitFocusOrder="0" pos="240 304 216 232" class="OscillatorPanel"
                    params="model, factory"/>
  <GROUPCOMPONENT name="FXGroup" id="85cf2fc9be4f7bcf" memberName="FXGroup" virtualName=""
                  explicitFocusOrder="0" pos="8 688 976 136" title="FX"/>
  <GENERICCOMPONENT name="reverbPanel" id="c574c98e1dd9ac8a" memberName="reverbPanel"
                    virtualName="" explicitFocusOrder="0" pos="16 704 352 112" class="ReverbPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="delayPanel" id="f7f617a6d8249b19" memberName="delayPanel"
                    virtualName="" explicitFocusOrder="0" pos="376 704 264 112" class="DelayPanel"
                    params="model, factory"/>
  <GENERICCOMPONENT name="new component" id="958d847dca958b2b" memberName="chorusPanel"
                    virtualName="" explicitFocusOrder="0" pos="648 704 320 112" class="ChorusPanel"
                    params="model, factory"/>
  <TEXTBUTTON name="presetButton" id="eace43cbe17dcd89" memberName="presetButton"
              virtualName="" explicitFocusOrder="0" pos="16 16 64 24" bgColOff="ff808080"
              buttonText="Presets" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="saveButton" id="18cedefaea6c1f80" memberName="saveButton"
              virtualName="" explicitFocusOrder="0" pos="320 16 64 24" bgColOff="ff808080"
              buttonText="Save" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="presetCombo" id="d63a422738bbc6a6" memberName="presetCombo"
            virtualName="" explicitFocusOrder="0" pos="88 16 224 24" editable="1"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <SLIDER name="fmSlider" id="69f0d2ae761ae29c" memberName="fmSlider" virtualName=""
          explicitFocusOrder="0" pos="496 464 64 56" min="0.0" max="1.0"
          int="0.0" style="RotaryVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <GROUPCOMPONENT name="distGroup" id="7ba7ed3f872d0a19" memberName="distGroup"
                  virtualName="" explicitFocusOrder="0" pos="592 456 392 88" title="Distortion"/>
  <GENERICCOMPONENT name="distortionPanel" id="aca878e23294d78d" memberName="distortionPanel"
                    virtualName="" explicitFocusOrder="0" pos="608 472 288 64" class="DistortionPanel"
                    params="model, factory"/>
  <GROUPCOMPONENT name="arpGroup" id="9465491d90d794f8" memberName="arpGroup" virtualName=""
                  explicitFocusOrder="0" pos="800 544 184 144" title="Arpeggiator"/>
  <GENERICCOMPONENT name="arpPanel" id="28f6a1e250ee9be9" memberName="arpPanel" virtualName=""
                    explicitFocusOrder="0" pos="808 560 168 120" class="ArpPanel"
                    params="factory,arp"/>
  <TABBEDCOMPONENT name="modulationTab" id="272892ff46aad47f" memberName="modulationTab"
                   virtualName="" explicitFocusOrder="0" pos="24 568 264 120" orientation="bottom"
                   tabBarDepth="22" initialTab="0">
    <TAB name="LFO 1" colour="0" useJucerComp="0" contentClassName="LFOPanel"
         constructorParams="model, factory" jucerComponentFile=""/>
    <TAB name="LFO 2" colour="0" useJucerComp="0" contentClassName="LFOPanel"
         constructorParams="model, factory" jucerComponentFile=""/>
    <TAB name="LFO 3" colour="0" useJucerComp="0" contentClassName="LFOPanel"
         constructorParams="model, factory" jucerComponentFile=""/>
    <TAB name="ENV 1" colour="0" useJucerComp="0" contentClassName="EnvelopePanel"
         constructorParams="model, factory" jucerComponentFile=""/>
    <TAB name="ENV 2" colour="0" useJucerComp="0" contentClassName="EnvelopePanel"
         constructorParams="model, factory" jucerComponentFile=""/>
  </TABBEDCOMPONENT>
  <GENERICCOMPONENT name="filterPanel1" id="ab18dcaf405ed80c" memberName="filterPanel1"
                    virtualName="" explicitFocusOrder="0" pos="608 224 360 88" class="FilterPanel"
                    params="model, factory"/>
  <GROUPCOMPONENT name="filterGroup2" id="9faaaa60818a688b" memberName="filterGroup2"
                  virtualName="" explicitFocusOrder="0" pos="592 344 392 112" title="Filter 2"/>
  <GENERICCOMPONENT name="filterPanel2" id="a87879f1db2ff94f" memberName="filterPanel2"
                    virtualName="" explicitFocusOrder="0" pos="608 360 360 80" class="FilterPanel"
                    params="model, factory"/>
  <LABEL name="filterModeLabel" id="fbc4b7abb7ac3ff4" memberName="filterModeLabel"
         virtualName="" explicitFocusOrder="0" pos="824 324 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter mode" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="new combo box" id="626a4dd7c895070d" memberName="filterModeCombo"
            virtualName="" explicitFocusOrder="0" pos="896 324 88 24" editable="0"
            layout="33" items="Serial&#10;Parallel" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="cutoffLink" id="9912bdda16f7b389" memberName="cutoffLink"
                virtualName="" explicitFocusOrder="0" pos="688 324 32 24" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="new label" id="266b573f0945ea7c" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="720 324 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Cutoff link" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="mainDisplay" id="f9a1e2075d65330a" memberName="mainDisplay"
              virtualName="" explicitFocusOrder="0" pos="592 152 392 56" textcol="ffff4400"
              bkgcol="ff000000" initialText="" multiline="0" retKeyStartsLine="0"
              readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <GENERICCOMPONENT name="modMatrix" id="19567bfe8c90898e" memberName="modMatrix"
                    virtualName="" explicitFocusOrder="0" pos="296 560 496 120" class="ModPanel"
                    params="this-&gt;synth-&gt;getModMatrix(), model, factory"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: logo_png, 19035, "../Images/logo.png"
static const unsigned char resource_MainUI_logo_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,4,107,0,0,0,224,8,6,0,0,0,180,17,235,149,0,0,0,9,112,72,89,115,0,0,11,19,0,0,11,19,1,0,154,156,
24,0,0,6,76,105,84,88,116,88,77,76,58,99,111,109,46,97,100,111,98,101,46,120,109,112,0,0,0,0,0,60,63,120,112,97,99,107,101,116,32,98,101,103,105,110,61,34,239,187,191,34,32,105,100,61,34,87,53,77,48,77,
112,67,101,104,105,72,122,114,101,83,122,78,84,99,122,107,99,57,100,34,63,62,32,60,120,58,120,109,112,109,101,116,97,32,120,109,108,110,115,58,120,61,34,97,100,111,98,101,58,110,115,58,109,101,116,97,
47,34,32,120,58,120,109,112,116,107,61,34,65,100,111,98,101,32,88,77,80,32,67,111,114,101,32,53,46,54,45,99,49,52,56,32,55,57,46,49,54,52,48,51,54,44,32,50,48,49,57,47,48,56,47,49,51,45,48,49,58,48,54,
58,53,55,32,32,32,32,32,32,32,32,34,62,32,60,114,100,102,58,82,68,70,32,120,109,108,110,115,58,114,100,102,61,34,104,116,116,112,58,47,47,119,119,119,46,119,51,46,111,114,103,47,49,57,57,57,47,48,50,47,
50,50,45,114,100,102,45,115,121,110,116,97,120,45,110,115,35,34,62,32,60,114,100,102,58,68,101,115,99,114,105,112,116,105,111,110,32,114,100,102,58,97,98,111,117,116,61,34,34,32,120,109,108,110,115,58,
120,109,112,61,34,104,116,116,112,58,47,47,110,115,46,97,100,111,98,101,46,99,111,109,47,120,97,112,47,49,46,48,47,34,32,120,109,108,110,115,58,120,109,112,77,77,61,34,104,116,116,112,58,47,47,110,115,
46,97,100,111,98,101,46,99,111,109,47,120,97,112,47,49,46,48,47,109,109,47,34,32,120,109,108,110,115,58,115,116,69,118,116,61,34,104,116,116,112,58,47,47,110,115,46,97,100,111,98,101,46,99,111,109,47,
120,97,112,47,49,46,48,47,115,84,121,112,101,47,82,101,115,111,117,114,99,101,69,118,101,110,116,35,34,32,120,109,108,110,115,58,112,104,111,116,111,115,104,111,112,61,34,104,116,116,112,58,47,47,110,
115,46,97,100,111,98,101,46,99,111,109,47,112,104,111,116,111,115,104,111,112,47,49,46,48,47,34,32,120,109,108,110,115,58,100,99,61,34,104,116,116,112,58,47,47,112,117,114,108,46,111,114,103,47,100,99,
47,101,108,101,109,101,110,116,115,47,49,46,49,47,34,32,120,109,112,58,67,114,101,97,116,111,114,84,111,111,108,61,34,65,100,111,98,101,32,80,104,111,116,111,115,104,111,112,32,50,49,46,48,32,40,87,105,
110,100,111,119,115,41,34,32,120,109,112,58,67,114,101,97,116,101,68,97,116,101,61,34,50,48,50,48,45,48,51,45,50,54,84,50,50,58,50,48,58,50,51,43,48,49,58,48,48,34,32,120,109,112,58,77,101,116,97,100,
97,116,97,68,97,116,101,61,34,50,48,50,48,45,48,51,45,50,54,84,50,50,58,50,48,58,50,51,43,48,49,58,48,48,34,32,120,109,112,58,77,111,100,105,102,121,68,97,116,101,61,34,50,48,50,48,45,48,51,45,50,54,84,
50,50,58,50,48,58,50,51,43,48,49,58,48,48,34,32,120,109,112,77,77,58,73,110,115,116,97,110,99,101,73,68,61,34,120,109,112,46,105,105,100,58,52,100,50,99,55,48,98,99,45,57,97,97,101,45,49,51,52,101,45,
57,101,49,101,45,54,55,55,101,49,52,54,49,99,52,99,100,34,32,120,109,112,77,77,58,68,111,99,117,109,101,110,116,73,68,61,34,97,100,111,98,101,58,100,111,99,105,100,58,112,104,111,116,111,115,104,111,112,
58,56,99,48,102,52,49,48,56,45,51,56,101,48,45,101,49,52,48,45,56,51,99,48,45,48,97,57,54,102,55,99,55,101,100,100,97,34,32,120,109,112,77,77,58,79,114,105,103,105,110,97,108,68,111,99,117,109,101,110,
116,73,68,61,34,120,109,112,46,100,105,100,58,54,101,100,56,100,100,52,49,45,97,97,51,97,45,101,100,52,54,45,57,97,51,48,45,57,99,48,99,99,98,54,100,53,53,51,50,34,32,112,104,111,116,111,115,104,111,112,
58,67,111,108,111,114,77,111,100,101,61,34,51,34,32,100,99,58,102,111,114,109,97,116,61,34,105,109,97,103,101,47,112,110,103,34,62,32,60,120,109,112,77,77,58,72,105,115,116,111,114,121,62,32,60,114,100,
102,58,83,101,113,62,32,60,114,100,102,58,108,105,32,115,116,69,118,116,58,97,99,116,105,111,110,61,34,99,114,101,97,116,101,100,34,32,115,116,69,118,116,58,105,110,115,116,97,110,99,101,73,68,61,34,120,
109,112,46,105,105,100,58,54,101,100,56,100,100,52,49,45,97,97,51,97,45,101,100,52,54,45,57,97,51,48,45,57,99,48,99,99,98,54,100,53,53,51,50,34,32,115,116,69,118,116,58,119,104,101,110,61,34,50,48,50,
48,45,48,51,45,50,54,84,50,50,58,50,48,58,50,51,43,48,49,58,48,48,34,32,115,116,69,118,116,58,115,111,102,116,119,97,114,101,65,103,101,110,116,61,34,65,100,111,98,101,32,80,104,111,116,111,115,104,111,
112,32,50,49,46,48,32,40,87,105,110,100,111,119,115,41,34,47,62,32,60,114,100,102,58,108,105,32,115,116,69,118,116,58,97,99,116,105,111,110,61,34,115,97,118,101,100,34,32,115,116,69,118,116,58,105,110,
115,116,97,110,99,101,73,68,61,34,120,109,112,46,105,105,100,58,52,100,50,99,55,48,98,99,45,57,97,97,101,45,49,51,52,101,45,57,101,49,101,45,54,55,55,101,49,52,54,49,99,52,99,100,34,32,115,116,69,118,
116,58,119,104,101,110,61,34,50,48,50,48,45,48,51,45,50,54,84,50,50,58,50,48,58,50,51,43,48,49,58,48,48,34,32,115,116,69,118,116,58,115,111,102,116,119,97,114,101,65,103,101,110,116,61,34,65,100,111,98,
101,32,80,104,111,116,111,115,104,111,112,32,50,49,46,48,32,40,87,105,110,100,111,119,115,41,34,32,115,116,69,118,116,58,99,104,97,110,103,101,100,61,34,47,34,47,62,32,60,47,114,100,102,58,83,101,113,
62,32,60,47,120,109,112,77,77,58,72,105,115,116,111,114,121,62,32,60,112,104,111,116,111,115,104,111,112,58,84,101,120,116,76,97,121,101,114,115,62,32,60,114,100,102,58,66,97,103,62,32,60,114,100,102,
58,108,105,32,112,104,111,116,111,115,104,111,112,58,76,97,121,101,114,78,97,109,101,61,34,76,85,80,79,34,32,112,104,111,116,111,115,104,111,112,58,76,97,121,101,114,84,101,120,116,61,34,76,85,80,79,34,
47,62,32,60,47,114,100,102,58,66,97,103,62,32,60,47,112,104,111,116,111,115,104,111,112,58,84,101,120,116,76,97,121,101,114,115,62,32,60,47,114,100,102,58,68,101,115,99,114,105,112,116,105,111,110,62,
32,60,47,114,100,102,58,82,68,70,62,32,60,47,120,58,120,109,112,109,101,116,97,62,32,60,63,120,112,97,99,107,101,116,32,101,110,100,61,34,114,34,63,62,88,27,197,220,0,0,67,181,73,68,65,84,120,156,237,
221,121,124,84,213,253,255,241,87,18,2,33,11,36,36,132,45,44,145,132,29,69,81,196,8,130,214,42,86,84,172,244,91,23,44,10,254,190,84,251,109,221,80,91,251,176,182,86,91,247,173,90,90,43,160,168,88,23,20,
106,17,177,130,200,90,16,132,24,214,16,36,33,33,132,64,22,178,145,61,191,63,238,36,44,222,153,36,100,238,189,51,153,247,243,241,184,143,153,220,115,231,158,15,45,134,51,159,123,206,231,4,53,52,52,32,34,
34,34,34,34,34,34,34,190,33,216,233,0,68,68,68,68,68,68,68,68,228,4,37,107,68,68,68,68,68,68,68,68,124,136,146,53,34,34,34,34,34,34,34,34,62,68,201,26,17,17,17,17,17,17,17,17,31,162,100,141,136,136,136,
136,136,136,136,136,15,81,178,70,68,68,68,68,68,68,68,196,135,40,89,35,34,34,34,34,34,34,34,226,67,148,172,17,17,17,17,17,17,17,17,241,33,29,188,117,163,160,160,32,111,221,74,68,68,68,68,68,68,164,181,
218,242,165,180,193,107,81,72,64,105,104,176,230,175,142,215,146,53,34,34,34,34,34,34,34,167,233,0,68,187,142,174,174,215,46,174,163,179,235,136,6,194,92,239,187,186,94,59,185,206,227,106,11,115,189,239,
138,145,148,233,228,186,174,81,39,215,113,166,142,3,53,174,247,13,192,49,215,251,74,215,129,235,92,131,235,218,74,215,207,238,222,151,187,94,143,1,197,39,189,175,104,67,140,18,64,130,188,149,5,210,204,
26,17,17,17,17,17,145,118,45,12,136,3,122,0,221,93,239,187,3,241,174,247,141,175,209,156,72,204,68,56,16,167,47,171,225,68,2,167,16,40,0,242,129,35,174,247,5,192,97,224,168,235,125,14,70,226,71,124,148,
85,51,107,148,172,17,17,17,17,17,17,145,88,160,63,208,27,72,112,189,158,254,62,214,177,232,2,91,25,144,235,58,242,48,18,56,121,174,159,15,0,89,192,33,160,206,169,0,3,153,191,39,107,148,201,9,44,90,239,
217,118,250,111,38,112,248,226,127,47,250,251,23,88,172,254,59,168,191,79,210,82,190,248,251,80,164,61,137,0,6,2,137,192,0,224,44,215,107,227,251,72,135,226,18,239,217,143,145,184,201,196,72,226,100,2,
251,128,189,24,201,28,177,128,63,38,107,174,6,198,157,116,72,224,88,123,210,177,212,225,88,252,197,201,255,173,140,199,88,195,43,129,193,87,254,123,209,239,236,192,101,197,223,65,253,125,146,51,81,98,
114,174,216,205,117,245,174,247,141,245,35,78,174,47,81,139,241,20,26,78,212,154,168,0,170,49,150,18,212,0,165,24,79,160,27,239,85,124,210,117,37,174,207,87,184,14,37,145,196,223,244,3,6,1,67,93,175,67,
128,193,64,95,39,131,18,199,149,3,233,24,137,155,189,64,134,235,231,29,156,248,253,41,103,192,223,146,53,127,2,126,227,149,27,139,191,123,1,184,207,233,32,124,220,67,192,147,78,7,33,62,97,43,112,39,176,
209,198,62,123,0,115,128,235,109,236,83,124,215,159,129,135,219,120,15,141,1,164,189,41,225,68,226,166,152,239,23,12,61,249,181,8,163,198,68,99,173,137,2,180,44,65,172,19,13,156,13,156,115,210,235,112,
32,220,193,152,196,63,229,0,105,192,78,215,235,118,96,23,42,134,220,34,254,148,172,121,18,227,203,167,72,163,141,192,88,167,131,240,81,74,212,136,153,177,216,147,176,233,1,164,186,94,69,26,181,37,97,163,
68,141,200,247,53,22,16,109,44,26,154,135,177,28,225,244,186,19,249,40,177,35,238,197,3,99,92,199,121,192,72,140,25,52,62,47,52,52,148,228,228,100,122,245,234,69,108,108,44,177,177,177,116,237,218,149,
168,168,40,58,119,238,76,88,88,24,81,81,81,116,234,212,137,78,157,58,17,25,25,73,167,78,157,8,13,13,37,50,50,178,233,30,29,59,118,4,32,60,60,156,160,160,32,58,116,232,208,116,174,241,154,208,208,208,51,
142,179,186,186,154,218,218,218,166,159,43,42,42,154,206,215,212,24,155,68,149,151,27,117,126,171,170,170,168,174,174,166,188,188,188,233,125,105,105,41,85,85,85,84,85,85,81,82,82,66,69,69,5,197,197,197,
20,21,21,113,244,232,81,142,28,57,66,118,118,54,249,249,249,103,28,163,3,210,129,175,93,199,22,96,27,39,102,46,138,139,191,36,107,174,5,150,120,229,134,210,222,104,134,205,247,141,3,214,56,29,132,248,
164,173,24,3,49,171,125,132,102,212,136,185,201,180,126,73,212,213,192,191,45,136,69,36,144,28,224,68,157,137,147,95,247,187,222,87,59,20,151,216,43,2,24,13,92,8,92,128,145,160,233,239,104,68,46,49,49,
49,12,26,52,136,254,253,251,211,179,103,79,226,226,226,136,139,139,163,91,183,110,68,71,71,19,19,19,67,151,46,93,136,140,140,36,60,60,156,240,240,112,194,194,194,154,191,113,0,169,173,173,165,162,162,
130,138,138,10,74,75,75,41,45,45,165,168,168,136,226,226,226,166,228,206,225,195,135,57,114,228,8,121,121,121,164,166,166,146,151,151,231,116,216,39,219,133,145,184,249,47,198,18,234,237,4,120,162,217,
95,146,53,203,128,73,94,185,161,180,55,149,64,103,167,131,240,49,191,198,120,130,45,98,230,34,140,127,4,173,210,19,21,154,19,247,254,134,177,36,175,53,222,5,110,180,32,22,17,57,97,63,39,234,77,164,187,
142,93,24,5,69,197,127,197,96,60,196,187,220,245,106,199,3,155,239,25,50,100,8,35,71,142,164,111,223,190,36,36,36,16,31,31,79,247,238,221,233,222,189,59,177,177,177,196,196,196,16,21,21,229,68,104,1,175,
178,178,146,194,194,66,10,11,11,41,42,42,34,63,63,191,41,153,147,149,149,197,174,93,187,216,188,121,51,117,117,142,229,76,214,115,162,246,221,215,192,113,167,2,113,130,191,36,107,74,81,21,113,113,239,
2,96,179,211,65,248,144,127,99,60,137,22,49,243,0,240,172,133,247,159,10,124,96,225,253,197,191,109,199,152,98,223,26,217,24,219,187,138,136,253,202,48,106,77,52,214,155,72,3,190,193,88,122,37,190,167,
39,70,82,102,162,235,245,28,91,58,237,217,147,177,99,199,146,156,156,76,255,254,253,73,72,72,160,87,175,94,244,232,209,131,30,61,122,104,6,140,159,171,171,171,163,160,160,128,252,252,124,242,242,242,200,
203,203,35,39,39,135,140,140,12,182,109,219,198,150,45,91,236,12,103,51,70,242,102,153,235,181,93,215,190,241,135,100,205,185,24,255,40,156,98,234,212,169,124,240,129,190,15,4,146,57,115,230,112,215,93,
119,153,53,221,135,177,28,74,12,199,48,217,245,169,188,188,156,240,112,213,133,11,20,155,54,109,226,194,11,47,52,107,250,23,112,157,133,93,191,4,252,234,244,147,139,23,47,230,186,235,172,236,86,124,77,
114,114,50,25,25,25,102,77,221,48,138,165,182,68,34,240,221,233,39,39,76,152,192,170,85,171,206,60,56,9,40,141,245,33,90,114,174,113,252,218,88,63,162,161,161,161,233,125,93,93,29,199,143,27,15,117,171,
171,171,169,170,170,162,178,178,146,154,154,26,142,31,63,78,109,109,45,101,101,101,212,213,213,81,90,90,74,125,125,61,69,69,69,28,63,126,156,178,178,50,10,10,10,40,40,40,224,232,209,163,190,182,244,224,
76,100,98,140,207,183,98,124,121,250,47,230,59,108,137,181,66,48,18,51,215,0,151,0,231,90,217,89,74,74,10,163,70,141,98,200,144,33,12,24,48,128,126,253,250,209,167,79,31,226,226,226,172,236,86,124,92,
101,101,37,135,14,29,226,224,193,131,100,103,103,147,153,153,201,158,61,123,88,189,122,53,251,247,239,183,186,251,198,153,55,31,97,239,70,26,182,176,42,89,211,193,146,187,138,136,136,136,136,180,130,217,
131,10,95,120,120,81,81,81,209,148,244,41,47,47,167,188,188,188,169,214,68,121,121,121,83,205,137,99,199,142,81,92,92,220,180,52,33,59,59,155,157,59,119,58,185,44,1,96,128,235,248,241,73,231,182,3,235,
48,190,56,173,195,88,90,37,222,55,2,99,214,204,21,192,85,128,215,167,173,12,28,56,144,137,19,39,114,206,57,231,48,104,208,32,6,14,28,72,223,190,125,233,212,169,147,183,187,146,118,32,44,44,140,196,196,
68,18,19,19,191,215,86,80,80,64,86,86,22,251,247,239,103,207,158,61,164,166,166,178,106,213,42,111,22,67,78,113,29,15,98,20,83,255,12,248,24,227,247,208,81,111,117,210,222,40,89,35,34,34,34,34,226,70,
99,145,212,51,85,86,86,118,74,17,209,194,194,194,166,157,97,114,114,114,56,120,240,32,251,246,237,99,219,182,109,118,37,118,70,184,142,89,174,159,115,48,190,48,125,225,122,221,99,71,16,237,212,24,140,
101,198,227,48,106,207,121,205,200,145,35,249,225,15,127,200,121,231,157,199,224,193,131,25,48,96,128,102,202,136,215,52,238,210,117,222,121,39,202,37,53,52,52,112,232,208,33,210,211,211,217,177,99,7,
91,182,108,97,249,242,229,228,230,230,182,181,187,120,224,103,174,163,1,35,105,179,4,227,247,207,247,102,233,6,50,37,107,68,68,68,68,68,44,18,25,25,73,100,100,36,189,122,245,242,120,93,125,125,61,133,
133,133,28,57,114,132,195,135,15,55,213,155,56,112,224,0,123,246,236,97,221,186,117,77,75,189,188,44,1,163,56,120,99,129,240,93,192,151,192,39,192,87,4,88,161,208,51,144,130,49,115,233,7,192,40,111,220,
176,103,207,158,92,125,245,213,140,25,51,134,225,195,135,51,104,208,32,186,119,239,238,141,91,139,180,88,80,80,16,189,123,247,166,119,239,222,76,156,56,17,48,18,56,7,15,30,36,35,35,131,212,212,84,86,175,
94,205,146,37,75,218,146,104,14,194,248,239,167,113,246,223,103,192,219,24,137,155,128,47,156,174,100,141,136,136,136,136,136,195,130,131,131,155,182,65,30,58,116,232,247,218,235,235,235,57,122,244,40,
185,185,185,228,228,228,144,153,153,201,222,189,123,249,230,155,111,88,187,118,173,55,67,25,234,58,238,194,40,92,252,17,90,174,112,186,65,24,179,103,126,6,76,104,235,205,250,247,239,207,148,41,83,184,
248,226,139,57,251,236,179,73,74,74,34,36,36,164,205,65,138,120,91,80,80,16,9,9,9,36,36,36,48,113,226,68,238,190,251,110,42,42,42,216,189,123,55,219,182,109,99,253,250,245,44,94,188,152,130,130,51,174,
109,62,201,117,212,3,239,0,255,196,72,30,7,100,210,88,201,26,17,17,17,17,17,31,23,28,28,76,124,124,60,241,241,241,140,26,53,234,148,182,202,202,74,178,179,179,201,202,202,34,35,35,131,237,219,183,179,
98,197,10,118,239,222,221,214,110,35,57,177,92,161,2,88,8,188,15,252,167,173,55,246,67,29,48,234,207,220,12,220,210,150,27,197,199,199,115,227,141,55,50,97,194,4,70,141,26,69,98,98,98,227,206,186,34,126,
39,60,60,156,243,206,59,143,243,206,59,143,25,51,102,240,183,191,253,141,221,187,119,179,105,211,38,150,47,95,206,162,69,139,206,100,230,77,48,112,171,235,56,132,177,131,233,123,24,133,138,3,134,146,53,
34,34,34,34,34,126,44,44,44,140,228,228,100,146,147,147,185,252,242,203,155,206,231,231,231,179,111,223,62,246,236,217,195,214,173,91,89,186,116,41,251,246,237,59,211,110,194,129,59,92,71,42,176,28,120,
29,216,219,198,240,125,221,96,96,6,198,78,78,223,159,242,212,66,147,38,77,226,218,107,175,101,236,216,177,140,24,49,130,208,208,80,175,5,40,226,75,58,116,232,192,136,17,35,24,49,98,4,51,102,204,160,180,
180,148,212,212,84,214,175,95,207,162,69,139,216,180,105,83,107,111,217,11,99,7,211,95,1,139,128,121,192,231,64,173,119,35,247,61,74,214,136,136,136,136,136,180,67,141,51,113,46,186,232,34,110,187,237,
54,94,124,241,69,178,179,179,217,177,99,7,155,55,111,102,197,138,21,124,245,213,87,103,114,235,115,92,199,175,128,215,48,150,42,108,240,98,232,190,96,2,240,63,24,201,169,142,173,253,112,104,104,40,211,
167,79,231,186,235,174,99,236,216,177,42,6,44,1,43,42,42,138,113,227,198,49,110,220,56,30,120,224,1,50,50,50,88,189,122,53,31,124,240,1,203,151,47,111,237,237,110,112,29,187,48,234,106,253,157,118,92,
148,88,201,26,17,17,17,17,145,0,16,20,20,68,191,126,253,232,215,175,31,87,93,117,21,143,60,242,8,121,121,121,108,217,178,133,53,107,214,240,254,251,239,179,127,127,171,118,242,14,227,196,19,239,185,24,
51,109,254,107,65,232,118,250,33,240,115,78,221,238,188,69,34,34,34,184,227,142,59,152,52,105,18,41,41,41,116,233,210,197,251,209,137,248,177,160,160,160,166,89,128,51,103,206,228,224,193,131,172,93,187,
150,15,63,252,144,15,63,252,176,53,183,106,172,173,245,11,96,14,48,31,216,105,65,200,142,10,118,58,0,17,17,17,17,17,113,70,227,206,67,79,62,249,36,25,25,25,124,251,237,183,188,252,242,203,140,25,51,166,
181,183,154,137,49,187,230,175,192,72,175,7,106,189,43,128,79,49,150,87,180,42,81,51,125,250,116,150,45,91,198,225,195,135,121,241,197,23,153,52,105,146,18,53,34,45,208,167,79,31,126,250,211,159,242,193,
7,31,112,224,192,1,254,241,143,127,112,225,133,23,182,230,22,17,192,108,224,91,224,57,96,180,21,113,58,69,201,26,17,17,17,17,17,33,56,56,152,145,35,71,242,203,95,254,146,141,27,55,146,158,158,206,43,175,
188,194,136,17,35,90,115,155,59,49,190,56,221,11,116,178,36,80,239,186,20,35,73,179,28,184,170,165,31,74,73,73,97,222,188,121,28,58,116,136,55,222,120,131,73,147,38,17,17,17,97,89,144,34,237,93,223,190,
125,185,227,142,59,216,176,97,3,223,126,251,45,127,250,211,159,232,217,179,103,75,63,30,2,220,7,108,6,158,1,206,178,42,78,59,41,89,35,34,34,34,34,34,223,147,156,156,204,47,126,241,11,182,110,221,202,250,
245,235,185,231,158,123,90,179,165,244,243,24,73,155,203,172,139,176,77,70,97,20,42,93,73,11,147,52,33,33,33,220,123,239,189,108,221,186,149,181,107,215,114,251,237,183,183,230,203,164,136,180,64,80,80,
16,35,71,142,228,55,191,249,13,25,25,25,124,252,241,199,167,20,78,111,129,217,24,69,208,239,194,40,140,238,183,148,172,17,17,17,17,17,17,183,58,116,232,192,69,23,93,196,11,47,188,64,118,118,54,47,188,
240,66,75,147,20,131,128,21,192,239,45,13,176,117,250,3,79,2,91,129,219,91,242,129,196,196,68,94,125,245,85,114,114,114,120,254,249,231,25,53,106,148,182,218,22,177,65,68,68,4,83,166,76,225,243,207,63,
103,235,214,173,220,127,255,253,45,77,24,71,2,175,98,36,140,175,181,52,72,11,41,89,35,34,34,34,34,34,45,210,171,87,47,238,185,231,30,246,238,221,203,130,5,11,72,72,72,104,201,199,30,197,40,60,124,174,
181,209,53,235,38,140,39,238,15,181,228,226,243,207,63,159,197,139,23,179,115,231,78,238,186,235,46,205,162,17,113,72,80,80,16,163,70,141,226,217,103,159,37,43,43,139,39,158,120,130,176,176,176,150,124,
116,32,176,4,120,15,104,213,122,78,95,160,100,141,136,136,136,136,136,180,74,100,100,36,183,222,122,43,187,118,237,98,238,220,185,196,199,199,55,247,145,11,129,111,48,10,17,219,237,44,224,111,192,66,160,
107,115,23,143,27,55,142,101,203,150,177,97,195,6,174,187,238,186,150,126,41,20,17,27,244,233,211,135,135,31,126,152,172,172,44,158,127,254,249,150,22,243,254,31,140,89,54,179,172,141,206,187,148,172,
17,17,17,17,17,145,51,18,25,25,201,140,25,51,216,177,99,7,191,253,237,111,91,242,145,215,129,95,91,28,214,201,166,97,204,166,105,246,75,218,133,23,94,200,178,101,203,88,181,106,21,147,38,77,162,67,135,
14,214,71,39,34,103,36,62,62,158,123,239,189,151,172,172,44,158,121,230,153,150,36,85,131,48,146,182,171,240,147,89,54,74,214,136,136,136,136,136,72,155,196,197,197,241,248,227,143,147,154,154,202,196,
137,19,155,187,252,207,192,99,214,71,197,159,129,183,48,234,87,184,213,179,103,79,222,122,235,45,86,175,94,205,164,73,147,90,83,68,89,68,28,22,29,29,205,236,217,179,217,187,119,47,247,223,127,127,75,62,
50,1,63,153,101,163,100,141,136,136,136,136,136,120,197,217,103,159,205,178,101,203,120,234,169,167,154,187,244,17,224,9,139,194,24,6,124,69,51,51,120,130,131,131,121,226,137,39,216,177,99,7,211,166,77,
163,99,199,142,22,133,35,34,86,75,72,72,224,217,103,159,37,45,45,141,169,83,167,54,119,121,227,44,155,151,172,143,236,204,41,89,35,34,34,34,34,34,94,19,22,22,198,131,15,62,200,186,117,235,232,221,187,
183,167,75,31,6,238,243,114,247,51,48,150,61,93,226,233,162,31,253,232,71,236,220,185,147,135,31,126,152,110,221,186,121,57,4,17,113,202,136,17,35,120,239,189,247,248,240,195,15,91,82,20,252,87,192,6,
96,176,245,145,181,158,146,53,34,34,34,34,34,226,117,41,41,41,172,95,191,158,9,19,38,120,186,236,57,224,54,47,117,121,31,48,23,112,91,108,38,44,44,140,121,243,230,177,100,201,18,6,15,246,201,239,103,34,
210,70,193,193,193,220,112,195,13,164,165,165,113,247,221,119,55,119,249,88,140,4,239,116,235,35,107,29,37,107,68,68,68,68,68,196,18,253,251,247,231,163,143,62,98,242,228,201,158,46,155,3,12,106,99,87,
143,97,36,126,220,154,50,101,10,187,118,237,226,246,219,111,87,241,96,145,0,16,23,23,199,139,47,190,200,151,95,126,73,98,98,162,167,75,59,1,111,224,99,117,108,148,172,17,17,17,17,17,17,203,116,235,214,
141,133,11,23,50,101,202,20,119,151,132,97,20,3,62,83,127,198,168,129,227,214,11,47,188,192,7,31,124,192,128,1,3,218,208,141,136,248,163,137,19,39,178,105,211,38,166,79,111,118,242,204,223,128,95,218,
16,82,139,40,89,35,34,34,34,34,34,150,138,138,138,98,238,220,185,140,25,51,198,221,37,63,6,110,62,131,91,63,129,135,66,194,61,123,246,228,203,47,191,228,158,123,238,209,108,26,145,0,22,23,23,199,220,185,
115,121,245,213,87,155,187,244,101,224,255,108,8,169,89,74,214,136,136,136,136,136,136,229,186,117,235,198,63,255,249,79,98,99,99,221,93,242,87,160,127,43,110,249,32,70,145,98,83,99,198,140,97,253,250,
245,45,217,74,92,68,2,64,72,72,8,119,221,117,23,43,86,172,32,38,38,198,211,165,127,193,40,86,238,40,37,107,68,68,68,68,68,196,22,137,137,137,204,157,59,215,93,115,87,224,206,22,222,106,38,224,118,127,
240,201,147,39,243,233,167,159,54,87,167,66,68,2,208,101,151,93,198,218,181,107,25,56,112,160,167,203,230,2,55,217,20,146,41,37,107,68,68,68,68,68,196,54,215,94,123,45,51,102,184,125,104,125,19,70,210,
198,147,17,192,107,238,26,111,184,225,6,22,46,92,232,105,6,143,136,4,184,97,195,134,177,114,229,74,70,142,28,233,233,178,57,180,110,182,159,87,41,89,35,34,34,34,34,34,182,9,10,10,226,225,135,31,38,56,
216,244,171,72,63,96,92,51,183,152,131,155,239,49,83,166,76,97,254,252,249,68,69,69,181,49,74,17,105,239,250,245,235,199,210,165,75,25,49,98,132,187,75,186,98,44,183,116,132,170,108,137,136,136,136,136,
136,173,6,14,28,200,131,15,62,200,147,79,62,105,214,252,115,96,169,155,143,62,132,155,100,206,184,113,227,148,168,9,80,85,85,85,100,100,100,176,127,255,126,50,51,51,201,205,205,229,192,129,3,28,61,122,
148,188,188,60,202,203,203,79,185,190,99,199,142,244,238,221,155,248,248,120,226,227,227,233,215,175,31,253,250,245,35,41,41,137,129,3,7,18,25,25,233,208,159,68,236,214,183,111,95,62,250,232,35,46,185,
228,18,242,242,242,204,46,185,11,216,0,188,109,111,100,74,214,136,136,136,136,136,136,3,166,79,159,238,46,89,51,25,24,6,236,60,237,252,88,192,244,3,189,123,247,102,225,194,133,68,71,71,123,53,70,241,93,
185,185,185,172,92,185,146,79,63,253,148,15,63,252,144,154,154,154,86,125,126,231,206,211,255,122,25,130,131,131,153,58,117,42,215,94,123,45,87,94,121,37,113,113,113,222,8,87,124,88,114,114,50,31,127,
252,49,23,95,124,49,245,245,245,102,151,204,1,214,3,223,217,25,151,150,65,137,136,136,136,136,136,237,134,12,25,194,117,215,93,231,174,217,108,246,140,219,157,159,222,125,247,93,250,246,237,235,149,184,
196,183,109,219,182,141,59,238,184,131,62,125,250,112,235,173,183,242,238,187,239,182,58,81,227,73,125,125,61,239,191,255,62,211,166,77,163,119,239,222,60,247,220,115,148,149,149,121,237,254,226,155,198,
142,29,203,107,175,185,45,133,21,73,203,139,159,123,141,146,53,34,34,34,34,34,226,136,233,211,167,187,107,154,124,218,207,87,0,215,152,93,248,251,223,255,158,75,46,185,196,155,97,137,15,170,174,174,230,
183,191,253,45,231,158,123,174,167,29,197,188,170,166,166,134,217,179,103,243,163,31,253,136,156,156,28,91,250,20,231,220,118,219,109,220,120,227,141,238,154,103,3,231,217,24,142,150,65,137,136,136,136,
136,119,204,155,55,143,63,255,249,207,223,59,223,161,67,7,122,244,232,1,64,84,84,20,97,97,97,116,236,216,145,168,168,40,98,98,98,8,15,15,39,60,60,156,168,168,40,162,163,163,233,218,181,107,211,251,152,
152,24,98,99,99,233,216,177,163,221,127,28,177,193,197,23,95,236,174,233,202,211,126,190,221,236,162,33,67,134,112,255,253,247,123,53,38,241,61,13,13,13,60,240,192,3,188,252,242,203,142,244,191,102,205,
26,166,77,155,198,210,165,75,137,136,136,112,36,6,177,94,72,72,8,127,250,211,159,88,188,120,49,149,149,149,102,151,220,2,124,99,87,60,74,214,136,136,136,136,136,87,52,22,249,52,179,123,247,238,54,221,
123,208,160,65,140,28,57,146,97,195,134,145,148,148,68,98,98,34,201,201,201,244,236,217,179,77,247,21,103,197,199,199,115,237,181,215,242,175,127,253,235,244,166,142,192,24,96,147,235,213,244,113,247,
179,207,62,171,98,176,1,224,211,79,63,117,44,81,211,232,171,175,190,98,193,130,5,220,121,167,237,171,97,196,70,137,137,137,60,241,196,19,238,146,192,55,2,191,3,202,205,26,189,77,201,26,17,17,17,17,241,
121,233,233,233,164,167,167,179,104,209,162,83,206,15,25,50,132,41,83,166,112,249,229,151,51,102,204,24,237,4,228,135,174,186,234,42,179,100,13,192,120,140,100,205,84,179,198,113,227,198,49,105,210,36,
43,67,19,31,80,85,85,197,111,126,243,155,150,92,186,1,99,214,67,58,144,3,28,4,138,93,71,245,105,215,134,0,49,174,35,14,232,15,12,196,40,98,237,118,186,215,35,143,60,194,207,126,246,51,205,174,105,231,
102,204,152,193,31,254,240,7,74,74,74,78,111,234,13,76,0,62,181,35,14,213,172,17,17,17,17,17,191,181,123,247,110,158,124,242,73,46,191,252,114,146,146,146,120,238,185,231,56,122,244,168,211,97,73,43,156,
115,206,57,238,154,82,92,175,151,153,53,222,125,247,221,132,132,132,88,18,147,248,142,117,235,214,145,150,150,230,174,121,23,112,27,144,132,241,247,229,255,128,151,129,143,128,141,192,30,224,48,80,116,
218,113,20,216,139,145,12,252,20,99,183,159,217,24,133,173,207,6,158,49,235,172,160,160,128,85,171,86,181,253,15,37,62,45,58,58,154,135,30,122,200,93,243,143,237,138,67,201,26,17,17,17,17,105,23,242,243,
243,153,61,123,54,231,158,123,46,75,151,46,117,58,28,105,161,161,67,135,186,107,186,24,232,9,140,62,189,33,52,52,148,43,174,184,194,202,176,196,71,124,248,225,135,238,154,254,141,145,88,121,19,216,231,
197,46,211,128,7,129,23,205,26,87,172,88,225,197,174,196,87,121,216,169,206,52,121,108,5,37,107,68,68,68,68,164,93,201,201,201,97,242,228,201,188,244,210,75,78,135,34,45,16,29,29,205,133,23,94,104,214,
212,3,243,45,188,153,53,107,22,93,186,116,177,52,46,113,94,117,117,53,239,188,243,142,187,230,231,128,90,11,187,255,192,236,228,187,239,190,75,125,125,189,133,221,138,47,24,54,108,24,35,71,142,52,107,
74,4,220,102,152,189,73,53,107,68,68,68,68,196,106,247,1,139,93,239,195,49,138,199,134,2,17,64,180,235,53,28,232,10,196,2,221,92,71,119,160,23,208,23,8,107,109,167,247,220,115,15,241,241,241,220,116,211,
77,109,139,94,44,55,122,244,104,54,110,220,104,214,100,250,20,251,210,75,47,181,54,32,241,9,233,233,233,102,117,67,192,88,190,180,202,226,238,215,3,59,129,97,39,159,204,203,203,35,55,55,151,132,132,4,
139,187,23,39,5,5,5,113,253,245,215,187,91,130,55,14,99,9,158,165,148,172,17,17,17,17,17,59,236,111,227,231,187,1,253,128,193,174,99,40,112,1,70,81,80,183,166,79,159,78,74,74,10,253,251,247,111,99,247,
98,165,97,195,134,185,107,50,157,89,51,106,212,40,203,98,17,223,177,119,239,94,119,77,235,108,10,225,91,78,75,214,0,236,223,191,95,201,154,0,48,126,252,120,119,77,227,128,127,88,221,191,150,65,137,136,
136,136,136,63,40,4,182,1,239,1,143,1,55,97,20,21,29,11,188,4,52,152,125,168,166,166,134,57,115,230,216,20,162,156,169,1,3,6,184,107,250,222,58,132,129,3,7,146,152,152,104,105,60,226,27,190,251,238,59,
119,77,187,109,10,193,52,201,156,155,155,107,83,247,226,164,179,207,62,219,93,147,219,44,142,55,41,89,35,34,34,34,34,254,108,35,112,15,70,161,209,53,102,23,60,245,212,83,20,20,20,216,25,147,180,82,124,
124,124,139,175,189,242,202,43,9,10,10,178,48,26,241,21,30,102,214,184,109,240,178,3,102,39,149,172,9,12,241,241,241,12,28,104,58,121,51,17,99,9,175,165,148,172,17,17,17,17,145,246,96,59,48,29,40,53,107,
92,179,198,52,143,35,62,34,54,54,182,197,215,122,216,234,91,218,153,236,236,108,119,77,121,54,133,96,154,149,81,178,38,112,120,216,117,238,123,187,212,121,155,146,53,34,34,34,34,210,94,236,7,126,107,214,
240,245,215,95,219,28,138,180,70,107,146,53,67,134,12,177,48,18,241,37,30,146,53,118,77,149,43,54,59,89,84,84,100,83,247,226,180,115,207,61,215,93,147,146,53,34,34,34,34,34,173,240,149,217,201,205,155,
55,219,29,135,180,66,107,182,225,246,80,223,70,218,153,29,59,118,184,107,58,106,83,8,197,102,39,181,172,50,112,184,89,6,5,48,220,234,190,149,172,17,17,17,17,145,246,196,180,34,233,246,237,219,237,142,
67,90,33,40,40,168,69,187,235,4,7,7,211,171,87,47,27,34,18,167,85,87,87,83,95,95,111,214,84,9,152,54,88,192,116,89,165,102,214,4,142,62,125,250,184,107,74,182,186,111,37,107,68,68,68,68,164,61,41,51,59,
153,155,155,75,67,131,233,134,81,226,35,250,246,237,219,236,53,99,199,142,37,52,52,212,134,104,196,105,181,181,181,238,154,170,237,140,67,2,155,135,100,205,96,171,251,86,178,70,68,68,68,68,218,155,18,
179,147,199,143,31,183,59,14,105,133,238,221,187,55,123,141,135,173,116,69,68,188,46,50,50,146,65,131,6,153,53,117,115,29,150,81,178,70,68,68,68,68,68,28,215,177,99,199,102,175,73,74,74,178,33,18,145,
38,197,102,39,15,30,60,104,115,24,226,164,81,163,70,185,107,74,180,178,95,37,107,68,68,68,68,68,196,113,225,225,225,205,94,211,163,71,15,27,34,17,17,57,193,195,18,205,222,86,246,171,100,141,136,136,136,
136,136,56,174,115,231,206,205,94,211,146,165,82,34,34,222,228,161,110,77,243,85,209,219,64,201,26,17,17,17,17,17,241,11,74,214,136,136,221,60,236,64,167,100,141,136,136,136,136,136,72,108,108,172,211,
33,136,72,128,137,139,139,115,215,164,101,80,34,34,34,34,34,34,209,209,209,78,135,32,34,1,38,38,38,198,93,83,188,149,253,42,89,35,34,34,34,34,34,126,33,50,50,210,233,16,68,36,192,116,233,210,197,93,147,
165,83,253,148,172,17,17,17,17,17,17,159,151,144,144,64,72,72,136,211,97,136,72,128,241,144,36,238,102,101,191,74,214,136,136,136,136,136,136,207,75,76,76,116,58,4,17,9,64,81,81,81,238,154,52,179,70,68,
68,68,68,68,2,155,135,237,115,69,68,44,19,17,17,65,112,176,105,234,164,27,22,230,84,148,172,17,17,17,17,17,17,159,231,161,200,167,136,136,101,130,130,130,56,235,172,179,220,53,119,181,170,95,37,107,68,
68,68,68,68,196,231,117,237,106,217,119,34,17,17,143,186,119,239,238,174,201,178,170,231,74,214,136,136,136,136,136,136,207,211,78,80,34,226,20,15,51,251,148,172,17,17,17,17,17,145,192,21,30,30,238,116,
8,34,18,160,60,108,223,29,97,85,159,74,214,136,136,136,136,72,123,211,209,236,100,135,14,29,236,142,67,188,168,115,231,206,78,135,32,34,1,202,195,50,76,183,91,69,181,149,146,53,34,34,34,34,210,158,4,1,
97,102,13,29,59,154,230,112,196,79,104,25,148,136,56,197,195,204,26,183,13,109,165,100,141,136,136,136,136,180,39,209,102,39,147,146,146,108,14,67,188,77,51,107,68,196,41,33,33,33,238,154,180,117,183,
136,136,136,136,72,11,244,53,59,57,98,196,8,187,227,16,47,211,50,54,17,113,138,135,101,80,209,86,245,169,100,141,136,136,136,136,180,39,103,153,157,28,60,120,176,221,113,136,151,41,89,35,34,129,68,201,
26,17,17,17,17,105,79,110,52,59,57,106,212,40,155,195,16,111,211,50,40,17,113,138,135,223,63,150,109,83,167,100,141,136,136,136,136,180,23,55,1,63,53,107,24,61,122,180,205,161,136,136,72,123,225,161,64,
189,101,149,235,149,172,17,17,17,17,145,246,224,28,96,142,89,195,196,137,19,85,96,216,15,148,150,150,122,108,15,15,183,236,1,182,136,136,71,30,150,97,134,90,213,167,146,53,34,34,34,34,226,207,70,1,207,
0,219,0,211,10,144,119,221,117,23,65,65,65,54,134,36,103,162,186,186,218,99,123,112,176,190,186,136,136,51,60,44,131,138,176,170,79,85,233,18,17,17,17,17,127,18,11,12,5,70,3,87,1,87,122,186,56,37,37,133,
235,174,187,206,142,184,164,141,10,11,11,157,14,65,68,196,103,40,89,35,34,34,34,34,118,136,241,208,22,13,116,2,58,3,93,92,63,199,97,36,102,122,96,108,199,221,15,72,4,226,91,218,97,112,112,48,47,191,252,
178,167,90,3,226,67,10,10,10,156,14,65,68,196,103,40,89,35,34,34,34,34,86,123,222,117,216,234,195,15,63,84,97,97,63,81,91,91,75,106,106,170,211,97,136,136,248,12,45,252,20,17,17,17,145,118,165,119,239,
222,124,249,229,151,92,127,253,245,78,135,34,45,116,244,232,209,102,175,81,205,26,17,9,36,250,141,39,34,34,34,34,237,66,88,88,24,143,61,246,24,105,105,105,76,156,56,209,233,112,164,21,90,178,4,74,187,
65,137,72,32,209,50,40,17,17,17,17,241,107,63,249,201,79,152,50,101,10,147,38,77,162,91,183,110,78,135,35,103,32,59,59,219,233,16,68,68,220,242,176,117,183,101,57,21,37,107,68,68,68,68,196,111,12,25,50,
132,139,46,186,136,209,163,71,115,246,217,103,51,124,248,112,37,104,218,129,29,59,118,56,29,130,136,136,91,30,182,238,142,180,170,79,37,107,68,68,68,68,196,43,186,118,237,202,132,9,19,154,126,46,46,46,
166,188,188,188,217,207,245,233,211,135,206,157,59,19,25,25,73,116,116,52,49,49,49,196,197,197,17,19,19,67,108,108,44,61,122,244,32,62,62,158,248,248,120,186,118,237,106,229,31,65,28,178,113,227,70,167,
67,16,17,241,41,74,214,136,136,136,136,136,87,220,124,243,205,220,124,243,205,78,135,33,126,166,166,166,134,79,62,249,196,233,48,68,68,124,138,10,12,139,136,136,136,136,136,99,118,238,220,73,101,101,165,
211,97,136,136,248,20,37,107,68,68,68,68,68,196,49,107,214,172,113,58,4,17,17,159,163,100,141,136,136,136,136,136,56,162,161,161,129,183,222,122,203,233,48,68,68,124,142,146,53,34,34,34,34,34,226,136,
180,180,52,54,109,218,228,116,24,34,34,62,71,201,26,17,17,17,17,17,113,196,123,239,189,231,116,8,34,34,62,73,201,26,17,17,17,17,17,177,93,94,94,30,79,61,245,148,211,97,136,136,248,36,37,107,68,68,68,68,
68,196,118,175,191,254,58,117,117,117,102,77,233,118,199,34,34,226,107,148,172,17,17,17,17,17,17,91,237,219,183,143,71,31,125,212,93,115,158,157,177,136,136,248,34,37,107,68,68,68,68,68,196,54,117,117,
117,60,244,208,67,212,215,215,155,53,87,217,29,143,136,136,47,82,178,70,68,68,68,68,68,108,243,230,155,111,178,104,209,34,119,205,47,218,24,138,136,136,207,82,178,70,68,68,68,68,68,108,177,105,211,38,
254,247,127,255,215,93,243,49,96,142,141,225,136,136,248,44,37,107,68,68,68,68,68,196,114,25,25,25,92,127,253,245,238,138,10,3,220,9,100,217,24,146,136,136,207,82,178,70,68,68,68,68,68,44,181,119,239,
94,38,77,154,68,110,110,174,187,75,230,0,239,218,24,146,136,136,79,83,178,70,68,68,68,68,68,44,179,113,227,70,46,185,228,18,246,237,219,231,238,146,18,224,105,27,67,18,17,241,121,74,214,136,136,136,136,
136,136,215,213,213,213,49,111,222,60,82,82,82,200,203,243,184,27,247,157,64,166,61,81,137,136,248,7,37,107,68,68,68,68,68,196,171,178,178,178,184,249,230,155,153,57,115,166,187,45,186,27,221,1,44,180,
41,44,17,17,191,209,193,233,0,68,68,68,68,68,164,125,40,43,43,227,205,55,223,100,246,236,217,84,86,86,54,119,249,207,129,185,54,132,37,34,226,119,148,172,17,17,17,17,17,145,54,41,43,43,99,241,226,197,
252,225,15,127,32,35,35,163,37,31,153,9,204,179,56,44,17,17,191,165,100,141,136,136,136,136,136,156,145,226,226,98,222,123,239,61,30,127,252,113,114,114,114,90,242,145,99,24,53,106,180,243,147,136,136,
7,74,214,136,136,136,136,136,72,139,53,52,52,240,205,55,223,176,104,209,34,94,126,249,101,202,203,203,91,250,209,57,192,83,64,150,117,209,137,136,180,15,74,214,136,136,136,136,136,72,179,14,28,56,192,
202,149,43,249,219,223,254,198,198,141,27,91,243,209,50,140,217,52,111,91,19,153,136,72,251,163,100,141,136,136,136,136,136,152,218,187,119,47,95,125,245,21,255,252,231,63,89,177,98,197,153,220,226,57,
224,175,192,119,222,141,76,68,164,125,83,178,70,68,68,68,68,68,0,40,44,44,100,235,214,173,172,95,191,158,143,63,254,152,173,91,183,158,233,173,230,3,255,0,54,120,47,58,17,145,192,161,100,141,136,136,136,
136,72,128,58,124,248,48,187,119,239,102,219,182,109,252,231,63,255,97,233,210,165,109,189,229,43,192,66,148,164,17,17,105,19,37,107,68,68,68,68,68,2,64,105,105,41,153,153,153,236,217,179,135,77,155,54,
241,249,231,159,147,154,154,234,141,91,239,1,254,141,145,164,249,198,27,55,20,17,9,116,74,214,136,136,136,136,136,180,35,181,181,181,28,58,116,136,3,7,14,144,158,158,78,90,90,26,235,214,173,99,211,166,
77,222,238,234,45,140,45,184,151,121,251,198,34,34,129,78,201,26,17,17,17,17,17,63,83,87,87,71,81,81,17,135,15,31,38,59,59,155,204,204,76,210,211,211,73,77,77,101,205,154,53,212,212,212,88,209,109,61,
176,8,248,24,88,11,100,91,209,137,136,136,40,89,35,34,34,34,34,226,83,26,26,26,40,41,41,161,184,184,152,194,194,66,142,28,57,210,148,148,217,191,127,63,187,119,239,102,227,198,141,86,37,100,78,151,13,
124,1,252,11,35,65,115,212,142,78,69,68,2,157,146,53,34,34,34,34,34,22,170,175,175,167,172,172,140,178,178,50,74,74,74,40,41,41,225,216,177,99,77,71,81,81,17,121,121,121,228,230,230,178,111,223,62,190,
253,246,91,42,43,43,157,10,183,4,248,18,35,65,179,22,216,230,84,32,34,34,190,226,248,241,227,238,154,202,172,234,83,201,26,17,17,17,17,9,56,149,149,149,212,215,215,3,70,141,151,234,234,106,0,106,106,106,
154,222,87,87,87,83,83,83,67,77,77,13,85,85,85,28,63,126,156,218,218,90,202,202,202,168,173,173,229,248,241,227,28,63,126,156,202,202,74,74,75,75,41,45,45,165,184,184,248,148,25,49,7,15,30,36,63,63,223,
177,63,103,11,124,7,252,23,88,15,172,1,190,117,54,28,17,17,223,83,91,91,235,182,201,170,62,149,172,17,17,17,17,17,175,152,55,111,30,127,254,243,159,29,141,161,174,174,142,253,251,247,59,26,131,15,43,0,
82,129,45,192,58,140,36,205,97,71,35,50,87,98,118,178,188,188,220,238,56,68,68,28,163,100,141,136,136,136,136,120,69,85,85,21,25,25,25,78,135,33,134,29,192,78,140,228,204,54,215,113,208,193,120,90,163,
222,236,100,67,67,131,221,113,136,136,56,70,201,26,17,17,17,17,17,255,116,0,216,15,236,57,233,216,13,100,97,225,212,124,17,17,177,158,146,53,34,34,34,34,34,190,39,31,200,3,14,1,57,24,187,50,101,158,116,
28,68,9,25,17,145,118,75,201,26,17,17,17,17,17,107,213,99,212,139,41,116,189,30,117,29,39,159,203,115,29,185,24,137,26,91,246,229,22,17,17,223,164,100,141,136,136,136,136,4,162,114,160,206,245,190,26,
168,112,189,175,2,26,247,104,173,116,29,141,231,202,48,146,40,199,78,250,76,185,171,173,196,117,28,59,233,125,227,209,120,111,105,25,213,172,17,17,159,82,81,225,246,215,184,182,238,22,105,135,52,16,17,
17,145,64,113,31,240,134,77,125,213,227,102,55,161,211,232,31,92,223,165,221,160,68,196,167,212,213,213,185,107,210,214,221,34,237,80,9,16,125,250,201,227,199,143,19,17,17,97,127,52,34,34,34,214,42,114,
58,0,17,17,145,51,81,93,93,237,182,201,170,62,131,173,186,177,136,136,136,136,136,200,25,48,93,86,112,252,248,113,179,211,34,34,150,243,240,251,199,178,101,174,74,214,136,56,199,116,25,84,125,189,233,
105,17,17,17,145,64,97,186,172,160,182,86,155,95,137,72,224,80,178,70,196,57,166,235,177,245,212,72,68,68,68,2,156,233,178,2,37,107,68,196,41,37,37,110,75,161,29,179,170,79,37,107,68,68,68,68,68,196,151,
152,46,43,208,3,45,17,113,74,77,77,141,187,38,203,178,200,74,214,136,56,199,116,23,10,237,6,37,34,34,34,1,206,180,102,141,135,173,115,69,68,44,85,90,90,234,182,201,170,62,149,172,17,113,142,233,148,57,
13,68,68,68,68,36,192,153,38,107,180,117,183,136,56,165,168,200,237,134,134,110,215,71,181,149,146,53,34,206,81,129,97,17,113,92,93,93,157,187,38,253,50,18,17,167,152,102,101,244,64,75,68,156,226,161,
102,141,101,89,100,37,107,68,156,99,250,95,188,6,34,34,98,167,253,251,247,187,107,178,236,73,145,136,72,51,76,151,21,148,149,153,78,184,17,17,177,156,135,100,141,182,238,22,105,135,42,205,78,86,87,155,
110,128,32,34,34,34,18,40,76,191,21,29,59,102,217,166,43,34,34,30,29,62,124,216,93,147,150,65,137,180,67,74,214,136,136,136,136,124,159,233,204,26,37,107,68,196,9,13,13,13,236,219,183,207,180,9,40,182,
170,95,37,107,68,156,99,58,16,209,50,40,17,17,17,9,112,71,205,78,230,230,230,218,29,135,136,136,167,37,152,133,86,246,171,100,141,136,115,170,204,78,106,102,141,136,216,229,248,241,227,238,154,74,49,158,
22,137,136,56,193,52,89,227,161,198,150,136,136,101,148,172,17,9,60,166,115,121,85,60,79,68,236,226,97,39,168,90,59,227,16,17,57,77,9,38,191,135,178,178,178,60,253,222,18,17,177,68,105,169,233,130,8,128,
2,43,251,85,178,70,196,57,166,201,26,15,191,12,68,68,188,170,178,210,180,116,22,184,89,166,41,34,98,35,211,39,214,122,168,37,34,118,243,240,253,76,51,107,68,218,169,98,179,147,42,158,39,34,118,169,169,
169,113,215,100,186,76,83,68,196,70,71,204,78,122,216,62,87,196,10,81,102,39,227,227,227,237,142,67,28,84,80,224,118,2,205,33,43,251,85,178,70,196,57,166,89,25,13,66,68,196,46,205,212,172,17,17,113,210,
65,179,147,30,190,52,137,88,33,196,236,100,104,104,168,221,113,136,131,142,30,53,45,163,5,74,214,136,180,91,166,201,154,226,226,98,155,195,16,145,64,229,97,57,65,145,157,113,136,136,152,200,49,59,121,
228,136,233,132,27,17,17,203,28,62,124,216,93,147,105,82,217,91,148,172,17,113,142,105,138,54,47,47,207,238,56,68,36,64,41,89,35,34,62,204,244,75,80,126,126,190,221,113,136,72,128,59,120,208,109,78,70,
201,26,145,118,202,244,209,208,129,3,7,236,142,67,2,147,233,239,255,224,96,253,179,16,72,60,20,204,43,182,49,12,17,17,51,185,102,39,149,172,17,155,153,214,172,137,137,137,177,59,14,113,80,86,86,150,187,
38,211,25,128,222,162,81,185,136,115,76,147,53,223,126,251,45,13,13,13,118,199,34,129,167,139,217,201,136,136,8,187,227,16,7,121,168,145,101,233,238,6,34,34,45,240,157,233,201,239,76,79,75,59,228,161,
46,76,39,27,195,48,29,24,117,233,98,58,140,146,118,234,235,175,191,118,215,180,207,202,126,149,172,17,113,78,29,38,9,155,186,186,58,109,223,45,34,182,240,176,236,210,210,39,69,34,34,45,144,110,118,50,
53,53,213,238,56,196,33,161,161,161,132,132,152,214,247,237,132,155,194,191,22,48,77,214,68,70,70,218,212,189,56,237,216,177,99,238,102,214,228,3,150,238,12,163,100,141,136,179,180,45,165,56,165,163,217,
73,237,110,16,88,60,44,39,80,241,44,17,113,90,182,217,201,245,235,215,83,91,91,107,119,44,226,144,196,196,68,119,77,118,77,109,137,54,59,169,101,80,129,195,195,131,45,75,103,213,128,146,53,98,191,32,29,
167,28,166,5,106,180,30,59,112,120,168,17,99,245,19,35,211,53,216,157,59,119,182,184,91,241,37,217,217,166,223,133,192,226,130,121,34,34,45,80,7,236,248,222,201,186,58,14,29,178,116,183,92,241,33,241,
241,241,238,154,76,199,49,22,136,51,59,25,27,27,107,83,247,226,52,15,197,133,247,90,221,183,146,53,98,167,231,129,122,29,167,28,147,204,254,135,210,142,80,129,35,60,60,220,93,147,213,131,16,211,39,82,
30,226,145,118,104,239,94,183,227,12,45,131,18,17,95,176,219,236,164,135,68,179,180,51,189,122,245,114,215,212,205,166,16,76,179,69,74,214,4,142,244,116,211,21,153,0,150,175,201,84,178,70,196,7,229,230,
154,110,128,32,237,144,135,228,72,87,139,187,54,125,82,164,130,121,129,163,166,166,134,255,254,247,191,238,154,149,172,17,17,95,176,197,236,228,238,221,166,57,28,105,135,122,247,238,237,174,169,135,77,
33,152,102,139,226,226,76,135,81,210,14,109,219,182,205,93,211,86,171,251,86,178,70,196,7,105,251,238,192,225,161,64,157,213,139,161,77,7,31,90,131,29,56,242,242,242,168,175,175,55,107,218,129,49,243,
79,68,196,105,166,201,154,173,91,45,255,142,36,62,194,195,50,40,183,13,94,118,150,217,73,15,73,36,105,103,190,248,226,11,119,77,74,214,136,4,162,93,187,118,57,29,130,216,36,58,58,218,93,83,79,11,187,237,
140,73,193,188,216,216,88,109,221,29,64,114,114,220,78,158,217,99,103,28,34,34,30,152,38,107,60,124,121,146,118,166,95,191,126,238,154,220,86,30,246,178,129,102,39,123,246,180,114,152,38,190,34,55,55,
151,125,251,76,235,8,167,3,197,86,247,175,100,141,136,15,250,252,243,207,221,61,241,150,118,166,67,135,14,156,115,206,57,102,77,29,177,110,138,111,178,217,201,241,227,199,91,212,157,248,162,61,123,220,
230,100,244,200,90,68,124,69,1,176,243,244,147,187,119,239,214,44,228,0,145,144,144,224,174,201,52,137,226,101,97,102,253,196,196,196,208,189,123,119,27,186,23,167,125,251,237,183,238,154,214,216,209,
191,146,53,34,62,168,164,164,68,117,107,2,200,185,231,158,235,174,41,201,162,46,135,152,157,28,57,114,164,69,221,137,47,242,48,0,81,178,70,68,124,201,90,179,147,155,55,111,182,59,14,113,128,135,100,205,
32,27,186,191,216,236,228,53,215,92,67,72,136,213,155,118,138,47,240,80,219,207,244,247,146,183,41,89,35,94,119,231,157,119,210,208,208,160,163,133,199,212,169,83,77,255,119,204,200,200,176,249,255,57,
113,138,155,153,53,0,110,179,56,109,116,169,217,201,225,195,135,91,212,157,248,162,85,171,86,185,107,50,93,118,32,34,226,144,85,102,39,191,250,234,43,155,195,16,39,36,38,38,18,22,22,102,214,52,22,55,245,
247,188,104,138,217,201,49,99,198,88,220,173,248,130,134,134,6,22,45,90,228,174,89,201,26,145,64,166,39,70,129,195,195,140,150,159,90,208,221,89,192,173,102,13,30,146,70,210,206,228,228,228,184,43,208,
185,23,200,179,57,28,17,17,79,76,191,20,189,241,198,27,148,151,151,219,29,139,216,44,52,52,148,107,175,189,214,93,243,101,22,118,61,20,248,185,89,195,69,23,93,100,97,183,226,43,182,111,223,206,246,237,
219,205,154,50,1,91,158,170,43,89,35,226,163,150,47,95,238,116,8,98,19,15,203,160,198,1,15,226,189,218,53,227,128,199,129,239,85,17,78,74,74,34,41,201,170,85,87,226,107,190,249,230,27,119,77,182,60,41,
18,17,105,133,108,76,150,103,150,148,148,176,98,197,10,7,194,17,187,93,117,213,85,238,154,254,14,220,2,184,221,90,243,12,196,0,55,3,255,5,58,156,222,152,144,144,160,101,227,1,226,131,15,62,112,215,100,
91,133,243,239,253,5,20,17,223,240,197,23,95,144,155,155,171,173,1,3,64,183,110,221,184,253,246,219,153,63,127,190,89,243,83,192,108,224,95,192,46,224,59,32,23,40,2,10,129,186,211,174,15,197,24,104,196,
96,36,121,6,96,204,166,25,131,49,101,216,212,172,89,179,232,208,65,255,36,4,138,149,43,87,186,107,250,210,206,56,68,68,90,232,75,76,150,6,191,244,210,75,76,158,60,153,224,96,61,127,110,207,38,76,152,224,
174,41,2,120,27,56,2,172,198,24,39,101,2,57,24,197,169,139,48,118,236,57,125,215,142,32,140,113,82,55,215,209,23,99,172,52,20,248,33,38,15,181,26,61,240,192,3,132,134,134,158,209,159,67,252,199,225,195,
135,121,230,153,103,220,53,127,100,87,28,26,153,139,248,176,207,62,251,140,25,51,102,56,29,134,216,224,182,219,110,115,151,172,1,232,14,204,180,170,239,144,144,16,110,186,233,38,171,110,47,62,166,164,
164,132,191,255,253,239,238,154,53,179,70,68,124,209,34,224,190,211,79,174,92,185,146,47,190,248,130,43,174,184,194,129,144,196,46,137,137,137,158,30,106,129,49,78,186,193,234,56,98,99,99,185,241,198,
27,173,238,70,124,192,220,185,115,169,172,172,52,107,218,135,141,15,182,148,134,22,241,97,207,61,247,28,85,85,85,78,135,33,54,24,63,126,60,55,223,124,179,35,125,63,251,236,179,244,233,211,199,145,190,
197,126,95,126,249,165,187,1,200,54,96,191,189,209,136,136,180,200,122,96,177,89,195,195,15,63,76,69,69,133,189,209,136,237,124,97,70,203,235,175,191,78,124,124,188,163,49,136,245,50,50,50,120,228,145,
71,220,53,127,12,152,14,162,172,160,100,141,136,15,219,185,115,167,167,245,146,210,142,4,5,5,241,210,75,47,49,110,220,56,91,251,253,213,175,126,197,47,126,241,11,91,251,20,231,212,213,213,241,226,139,
47,186,107,182,109,13,182,136,200,25,48,157,18,184,101,203,22,94,125,245,85,187,99,17,155,13,29,58,148,165,75,151,58,146,176,9,9,9,225,173,183,222,98,202,148,41,182,247,45,246,170,173,173,101,246,236,
217,212,215,159,190,114,14,128,26,96,174,157,241,40,89,35,226,227,238,191,255,126,242,243,243,157,14,67,108,16,23,23,199,210,165,75,121,248,225,135,45,239,43,33,33,129,247,223,127,159,231,159,127,222,
241,39,85,98,159,127,255,251,223,158,182,236,126,223,198,80,68,68,90,235,51,224,115,179,134,7,31,124,144,13,27,54,216,28,142,216,237,135,63,252,33,105,105,105,182,46,69,154,62,125,58,187,118,237,98,218,
180,105,182,245,41,206,121,253,245,215,89,178,100,137,187,230,231,129,221,54,134,163,100,141,136,175,203,207,207,231,215,191,254,53,13,13,13,78,135,34,54,232,210,165,11,79,60,241,4,25,25,25,60,253,244,
211,140,24,49,194,107,247,14,11,11,227,142,59,238,96,201,146,37,236,217,179,135,159,252,228,39,132,132,132,120,237,254,226,219,142,28,57,194,255,253,223,255,185,107,254,8,248,218,198,112,68,68,206,196,
31,221,53,220,116,211,77,228,228,228,216,25,139,56,96,240,224,193,188,251,238,187,236,216,177,131,167,159,126,154,49,99,198,120,189,143,148,148,20,254,242,151,191,176,123,247,110,222,120,227,13,146,147,
147,189,222,135,248,158,117,235,214,121,154,109,126,12,152,99,99,56,128,10,12,139,248,133,249,243,231,147,146,146,194,29,119,220,225,116,40,98,147,129,3,7,242,192,3,15,48,123,246,108,178,178,178,72,79,
79,231,187,239,190,35,43,43,139,236,236,108,142,28,57,66,78,78,14,213,213,213,167,124,174,115,231,206,244,238,221,155,238,221,187,211,171,87,47,250,247,239,79,66,66,2,131,6,13,34,49,49,145,176,176,48,
135,254,68,226,164,218,218,90,238,189,247,94,79,95,100,94,179,51,30,17,145,51,180,22,120,12,248,221,233,13,89,89,89,76,159,62,157,69,139,22,17,29,29,109,123,96,98,175,97,195,134,49,108,216,48,30,120,224,
1,242,243,243,201,200,200,32,51,51,147,156,156,28,114,114,114,56,114,228,8,249,249,249,228,230,230,82,91,91,123,202,103,131,131,131,73,72,72,160,71,143,30,196,197,197,209,183,111,95,250,245,235,199,128,
1,3,72,78,78,166,91,183,110,14,253,169,196,41,123,247,238,101,234,212,169,238,150,63,1,220,9,100,217,24,18,160,100,141,136,223,152,53,107,22,103,157,117,22,151,93,118,153,211,161,136,141,130,130,130,24,
48,96,0,3,6,12,112,58,20,241,99,79,62,249,36,239,188,243,142,187,230,143,128,229,54,134,35,34,210,22,143,2,87,1,23,156,222,176,114,229,74,110,191,253,118,22,44,88,64,84,84,148,253,145,137,35,226,227,227,
137,143,143,39,37,37,197,233,80,196,15,101,103,103,115,195,13,55,144,151,151,231,238,146,23,129,119,237,139,232,4,45,131,18,241,19,245,245,245,92,123,237,181,124,253,181,86,42,136,72,203,189,250,234,171,
158,118,53,168,199,228,9,181,136,136,143,187,211,93,195,226,197,139,249,217,207,126,70,105,105,169,157,241,136,136,31,202,206,206,230,234,171,175,38,45,45,205,221,37,199,48,146,53,142,80,178,70,196,143,
148,151,151,115,249,229,151,179,113,227,70,167,67,17,17,63,240,202,43,175,120,170,83,3,198,23,158,29,54,133,35,34,226,45,91,128,255,117,215,184,120,241,98,166,78,157,170,13,26,68,196,173,244,244,116,174,
184,226,10,79,137,26,112,104,249,83,35,37,107,68,124,215,49,179,147,37,37,37,140,31,63,158,207,63,55,221,16,65,68,132,218,218,90,30,123,236,49,126,249,203,95,122,186,236,5,84,171,70,68,252,215,63,128,
223,186,107,252,252,243,207,153,52,105,18,233,233,233,54,134,36,34,254,96,253,250,245,76,152,48,129,221,187,61,110,238,52,19,135,150,63,53,82,178,70,196,119,221,9,84,153,53,212,212,212,112,229,149,87,
242,218,107,175,105,151,40,17,57,69,97,97,33,51,103,206,228,209,71,31,245,116,217,82,224,62,155,66,18,17,177,202,159,92,135,169,173,91,183,114,193,5,23,240,217,103,159,217,24,146,136,248,170,134,134,6,
22,44,88,192,248,241,227,61,213,168,1,248,21,48,207,166,176,220,82,178,70,196,119,237,6,254,159,167,11,102,205,154,197,207,127,254,115,138,139,139,237,137,72,68,124,90,106,106,42,19,38,76,96,193,130,5,
158,46,219,0,76,182,41,36,17,17,171,253,22,15,9,155,146,146,18,174,186,234,42,30,127,252,113,42,43,43,109,12,75,68,124,201,177,99,199,184,251,238,187,153,62,125,186,167,93,159,192,72,212,252,197,166,176,
60,82,178,70,196,183,189,5,76,247,116,193,107,175,189,198,37,151,92,194,214,173,91,109,10,73,68,124,77,109,109,45,243,230,205,99,244,232,209,108,223,190,221,211,165,235,1,109,151,33,34,237,205,111,105,
166,88,250,35,143,60,194,85,87,93,213,220,178,7,17,105,135,190,254,250,107,38,76,152,192,95,254,210,108,14,230,23,248,72,162,6,148,172,17,241,7,11,128,105,64,153,187,11,210,210,210,56,255,252,243,121,
241,197,23,245,212,72,36,192,124,247,221,119,76,157,58,149,153,51,103,82,87,87,231,233,210,79,128,139,109,10,75,68,196,110,127,4,238,241,116,193,170,85,171,24,49,98,4,47,189,244,146,198,75,34,1,160,180,
180,148,63,252,225,15,140,25,51,134,212,212,212,230,46,159,9,252,213,134,176,90,76,201,26,17,255,240,14,112,14,198,238,7,166,234,235,235,185,247,222,123,185,236,178,203,216,188,121,179,125,145,137,136,
35,106,106,106,152,63,127,62,195,135,15,103,201,146,37,205,93,254,18,112,173,13,97,137,136,56,233,37,140,25,201,21,238,46,168,171,171,227,158,123,238,97,194,132,9,108,216,176,193,190,200,68,196,86,171,
87,175,230,226,139,47,230,247,191,255,125,115,151,30,3,110,198,7,106,212,156,78,201,26,17,255,241,29,112,62,240,55,79,23,109,216,176,129,11,46,184,128,223,253,238,119,28,57,114,196,158,200,68,196,86,27,
55,110,228,7,63,248,1,51,102,204,104,201,211,225,95,210,204,211,102,17,145,118,100,1,198,3,174,101,158,46,218,180,105,19,41,41,41,220,127,255,253,228,230,230,218,19,153,136,88,46,51,51,147,159,255,252,
231,76,152,48,161,185,109,185,1,230,99,252,190,112,116,215,39,119,148,172,17,241,63,119,98,20,190,242,232,143,127,252,35,67,135,14,101,222,188,121,154,234,43,210,78,28,60,120,144,95,254,242,151,140,29,
59,150,53,107,214,52,119,121,9,112,11,240,138,245,145,137,136,248,148,12,224,71,120,216,218,187,209,243,207,63,207,192,129,3,121,225,133,23,56,118,236,152,245,145,137,136,37,138,139,139,121,250,233,167,
73,74,74,226,239,127,255,123,75,62,114,15,48,3,200,178,52,176,54,80,178,70,196,63,253,5,99,186,158,199,81,69,65,65,1,51,103,206,36,37,37,133,101,203,150,53,87,207,66,68,124,84,97,97,33,207,60,243,12,73,
73,73,188,242,74,139,114,47,127,195,120,82,180,208,218,200,68,68,124,218,159,48,146,214,37,158,46,170,172,172,228,190,251,238,99,196,136,17,44,92,184,144,170,170,42,123,162,19,145,54,171,168,168,224,237,
183,223,102,228,200,145,60,244,208,67,45,249,190,147,138,177,217,194,75,214,71,215,54,74,214,136,248,175,119,49,190,140,61,213,220,133,91,183,110,229,71,63,250,17,227,199,143,103,201,146,37,212,212,212,
88,31,157,136,180,89,89,89,25,175,189,246,26,67,135,14,229,193,7,31,108,201,44,185,50,224,86,140,25,120,153,86,199,39,34,226,7,22,98,140,151,158,110,238,194,156,156,28,110,185,229,22,206,59,239,60,222,
126,251,109,42,42,220,150,190,17,17,135,149,149,149,49,127,254,124,206,57,231,28,110,189,245,86,114,114,114,90,242,177,95,3,163,0,191,40,88,165,100,141,136,127,203,194,248,165,51,1,248,182,185,139,55,
108,216,192,148,41,83,72,73,73,225,227,143,63,214,32,68,196,71,21,23,23,51,119,238,92,70,140,24,193,172,89,179,200,207,207,111,201,199,254,142,241,133,228,109,107,163,19,17,241,59,153,192,67,24,227,165,
175,155,187,120,231,206,157,220,122,235,173,12,30,60,152,121,243,230,81,82,226,113,98,142,136,216,168,184,184,152,121,243,230,49,116,232,80,102,204,152,65,70,70,70,75,62,182,22,24,75,11,30,114,251,18,
37,107,68,218,135,213,24,95,210,30,0,234,155,187,120,243,230,205,252,248,199,63,38,57,57,153,231,158,123,142,172,44,159,93,170,41,18,80,142,28,57,194,139,47,190,200,224,193,131,185,227,142,59,90,250,223,
230,22,224,114,224,231,24,133,200,69,68,196,220,106,96,12,112,23,112,188,185,139,115,114,114,152,57,115,38,201,201,201,60,253,244,211,100,102,102,90,29,159,136,184,177,119,239,94,30,123,236,49,250,247,
239,207,204,153,51,91,58,147,166,4,99,135,184,241,192,70,75,3,180,128,146,53,34,237,203,179,192,217,52,179,99,84,163,220,220,92,102,207,158,205,128,1,3,152,57,115,38,95,125,245,21,213,213,213,214,70,40,
34,223,179,115,231,78,126,247,187,223,209,175,95,63,238,189,247,222,150,206,164,41,7,238,192,216,37,110,133,165,1,138,136,180,47,115,48,198,75,207,182,228,226,252,252,124,30,122,232,33,18,19,19,185,237,
182,219,248,234,171,175,168,173,173,181,54,66,17,161,182,182,150,85,171,86,49,109,218,52,6,13,26,196,163,143,62,218,154,153,110,79,99,60,204,94,96,93,132,214,82,178,70,164,253,217,129,81,175,98,34,240,
105,75,63,52,111,222,60,38,78,156,200,192,129,3,121,252,241,199,217,182,109,27,13,13,13,86,197,40,18,240,42,43,43,249,228,147,79,184,230,154,107,24,62,124,56,127,252,227,31,91,179,115,91,99,98,118,174,
117,17,138,136,180,107,25,24,51,146,47,0,222,108,233,135,222,124,243,77,38,78,156,200,249,231,159,207,107,175,189,166,217,201,34,22,216,187,119,47,47,188,240,2,67,135,14,229,210,75,47,229,157,119,222,
105,205,199,231,99,60,200,122,8,63,175,223,215,193,233,0,68,196,50,95,185,142,43,48,150,71,92,223,146,15,229,228,228,240,200,35,143,240,200,35,143,48,122,244,104,102,206,156,201,196,137,19,25,50,100,8,
65,65,65,86,198,43,210,238,53,52,52,144,150,150,198,210,165,75,121,229,149,87,200,205,205,109,237,45,254,130,145,160,73,245,126,116,34,34,1,105,51,112,27,240,6,240,51,224,246,150,124,40,53,53,149,89,179,
102,1,48,101,202,20,166,77,155,198,101,151,93,70,76,76,140,69,97,138,180,111,5,5,5,124,241,197,23,188,241,198,27,124,246,217,103,103,114,139,191,99,212,237,91,235,221,200,156,163,100,141,72,251,247,185,
235,248,1,70,210,102,106,75,63,184,101,203,22,182,108,217,2,192,136,17,35,152,54,109,26,151,94,122,41,163,70,141,162,99,199,142,150,4,43,210,30,101,101,101,177,124,249,114,230,206,157,203,166,77,155,206,
228,22,127,197,24,128,248,197,238,5,34,34,126,104,149,235,152,7,220,140,49,75,185,69,22,47,94,204,226,197,139,9,9,9,97,214,172,89,76,158,60,153,139,46,186,136,232,232,104,75,2,21,105,47,242,243,243,89,
189,122,53,159,124,242,9,111,191,253,54,245,245,205,150,222,52,51,7,99,169,211,127,189,27,157,243,148,172,17,9,28,43,92,199,56,224,127,128,105,64,139,31,255,108,223,190,157,95,255,250,215,0,196,199,199,
51,125,250,116,38,76,152,192,232,209,163,233,217,179,167,21,241,138,248,181,189,123,247,178,102,205,26,22,45,90,196,167,159,182,120,69,226,201,234,49,102,210,188,133,81,68,88,68,68,172,183,214,117,204,
7,110,1,110,5,186,181,228,131,117,117,117,252,245,175,127,229,175,127,253,43,33,33,33,220,114,203,45,76,158,60,153,113,227,198,209,171,87,47,11,67,22,241,31,7,14,28,96,221,186,117,44,90,180,136,69,139,
22,157,233,109,14,1,239,2,11,105,199,99,36,37,107,68,2,79,227,32,228,105,224,18,140,39,71,227,90,115,131,252,252,124,158,121,230,25,158,121,230,25,0,198,141,27,199,53,215,92,195,69,23,93,196,168,81,163,
136,138,138,242,118,204,34,62,175,182,182,150,29,59,118,240,229,151,95,242,238,187,239,158,233,12,26,128,253,192,34,140,41,249,59,188,21,159,136,136,180,202,215,174,227,25,140,113,210,47,48,118,148,105,
145,186,186,58,22,44,88,192,130,5,70,109,211,171,175,190,154,201,147,39,51,102,204,24,70,140,24,161,25,202,18,48,42,42,42,216,182,109,27,107,215,174,229,163,143,62,98,227,198,54,109,202,180,26,227,65,
214,42,224,168,55,226,243,101,74,214,136,4,174,28,140,108,244,66,224,82,224,199,192,20,32,161,181,55,90,187,118,45,107,215,26,203,67,131,131,131,185,230,154,107,152,48,97,2,231,157,119,30,195,134,13,163,
123,247,238,222,139,90,196,135,100,103,103,179,101,203,22,86,173,90,197,123,239,189,71,94,94,94,91,110,247,47,224,53,140,90,83,101,94,9,80,68,68,218,234,32,240,158,235,248,33,112,35,70,109,155,86,125,
143,90,186,116,41,75,151,46,5,32,38,38,134,27,111,188,145,75,47,189,148,243,207,63,159,1,3,6,168,46,160,180,27,181,181,181,164,167,167,243,205,55,223,176,114,229,74,222,126,251,109,106,106,106,218,114,
203,98,224,159,192,71,192,127,188,17,163,191,80,178,70,68,0,190,116,29,247,98,12,68,254,7,99,48,18,214,218,27,213,215,215,179,100,201,18,150,44,89,210,116,238,194,11,47,228,202,43,175,228,130,11,46,96,
232,208,161,244,239,223,159,14,29,244,235,71,252,207,161,67,135,216,177,99,7,95,127,253,53,139,23,47,110,203,236,153,166,91,2,31,186,142,213,109,14,80,68,68,172,244,31,215,241,40,198,108,155,105,192,213,
173,189,73,81,81,17,115,230,204,97,206,156,57,0,12,26,52,136,171,175,190,154,177,99,199,114,246,217,103,147,148,148,164,113,146,248,141,170,170,42,118,239,222,205,182,109,219,88,189,122,53,75,150,44,161,
160,160,192,27,183,254,24,163,22,205,90,2,96,22,141,25,253,22,16,145,147,213,2,203,92,199,221,24,3,145,235,129,107,128,30,103,122,211,141,27,55,158,50,229,49,34,34,162,105,80,50,124,248,112,146,147,147,
233,215,175,31,33,33,33,109,139,94,196,139,234,234,234,200,204,204,100,251,246,237,108,218,180,137,165,75,151,146,154,234,149,77,152,106,49,102,180,253,19,88,9,84,121,227,166,34,34,98,155,28,140,223,225,
255,4,206,197,120,192,117,41,198,54,224,173,150,158,158,78,122,122,122,211,207,49,49,49,76,153,50,133,139,47,190,152,161,67,135,50,104,208,32,226,226,226,188,16,182,72,219,52,52,52,112,240,224,65,246,
236,217,67,90,90,26,155,54,109,226,227,143,63,166,178,178,210,91,93,172,195,72,206,44,4,190,245,214,77,253,149,146,53,34,226,78,9,240,169,235,0,184,12,184,22,99,230,205,176,182,220,184,188,188,156,247,
223,127,159,247,223,127,191,233,92,76,76,12,87,93,117,21,163,70,141,98,208,160,65,12,28,56,144,254,253,251,171,254,141,216,162,161,161,129,156,156,28,246,236,217,195,206,157,59,217,178,101,11,159,124,
242,9,69,69,69,222,236,102,49,198,52,250,181,24,3,125,17,17,241,127,91,93,7,192,121,192,79,49,30,118,165,156,233,13,139,138,138,152,63,127,62,243,231,207,111,58,55,114,228,72,46,187,236,50,70,141,26,197,
208,161,67,73,74,74,34,54,54,182,45,113,139,120,212,208,208,192,161,67,135,200,204,204,100,215,174,93,108,222,188,153,101,203,150,145,149,149,229,237,174,150,97,204,162,89,11,236,242,246,205,253,153,146,
53,34,210,82,43,93,7,192,32,140,129,200,15,128,203,129,248,182,222,188,168,168,136,133,11,23,178,112,225,194,83,206,143,30,61,154,148,148,20,134,15,31,78,82,82,18,125,251,246,165,79,159,62,68,68,68,180,
181,75,9,80,229,229,229,100,101,101,177,127,255,126,210,211,211,73,75,75,99,249,242,229,228,230,230,122,187,171,99,192,18,78,12,64,2,114,10,175,136,72,0,249,198,117,0,36,99,140,149,174,3,38,1,157,218,
114,227,180,180,52,210,210,210,78,57,151,148,148,196,248,241,227,25,57,114,36,73,73,73,156,117,214,89,244,235,215,79,15,186,164,213,202,202,202,56,112,224,0,25,25,25,236,217,179,135,173,91,183,178,98,
197,10,242,243,243,173,232,46,31,88,14,124,134,49,62,58,96,69,39,237,129,146,53,34,114,38,210,93,199,60,215,207,23,98,44,151,26,7,92,236,205,142,182,108,217,194,150,45,223,223,145,111,216,176,97,140,29,
59,150,193,131,7,55,13,78,122,247,238,77,207,158,61,181,206,91,0,40,44,44,228,208,161,67,100,103,103,147,153,153,201,206,157,59,217,184,113,163,55,234,204,120,178,13,163,246,204,39,24,133,130,219,84,81,
79,68,68,252,214,94,215,49,31,8,6,38,2,147,49,198,74,103,180,92,234,116,25,25,25,100,100,100,124,239,252,200,145,35,185,248,226,139,73,74,74,98,192,128,1,36,36,36,208,167,79,31,141,145,2,92,125,125,61,
249,249,249,100,103,103,147,157,157,205,190,125,251,216,179,103,15,27,55,110,100,251,246,237,86,119,191,1,35,49,179,4,99,169,147,180,64,80,67,67,131,119,110,20,20,212,1,147,65,105,104,104,40,253,251,247,
247,74,31,34,237,81,86,86,150,187,10,233,161,24,181,45,252,77,48,48,28,99,48,210,152,188,177,245,151,192,152,49,99,24,54,108,24,137,137,137,244,237,219,151,132,132,4,122,246,236,73,124,124,60,113,113,
113,170,141,211,78,212,214,214,114,244,232,81,14,31,62,76,126,126,62,7,15,30,100,223,190,125,236,218,181,139,13,27,54,88,49,83,198,204,33,140,98,147,159,99,12,66,188,62,55,216,79,132,96,242,251,42,36,
36,132,196,196,68,7,194,17,167,20,20,20,184,91,62,120,1,176,217,230,112,68,124,85,111,140,49,210,101,174,215,225,118,116,26,18,18,66,74,74,10,35,71,142,100,224,192,129,77,9,156,30,61,122,16,31,31,79,76,
76,140,118,165,242,115,165,165,165,228,231,231,115,248,240,97,114,115,115,201,206,206,38,43,43,139,93,187,118,177,110,221,58,202,203,203,237,10,101,51,198,184,104,185,235,181,93,239,114,233,173,156,202,
233,188,153,172,1,88,15,92,228,149,27,138,4,182,13,180,97,173,179,15,74,224,68,242,102,44,48,218,169,64,130,131,131,57,231,156,115,72,78,78,38,33,33,129,132,132,4,226,227,227,233,222,189,59,221,187,119,
39,54,54,150,232,232,104,162,162,162,52,96,113,80,117,117,53,133,133,133,20,21,21,81,80,80,192,209,163,71,57,122,244,104,211,44,153,29,59,118,152,206,184,178,193,94,140,65,199,26,215,235,94,39,130,240,
81,235,104,95,191,183,196,123,42,129,206,78,7,33,226,195,186,97,124,135,250,1,94,156,121,211,90,17,17,17,92,120,225,133,77,99,164,94,189,122,17,23,23,71,92,92,28,177,177,177,196,196,196,208,173,91,55,
66,67,67,157,8,47,160,53,52,52,112,236,216,49,10,10,10,40,44,44,252,222,195,170,204,204,76,190,249,230,27,114,114,28,43,137,247,95,140,113,209,50,140,239,49,199,157,10,196,9,254,146,172,121,26,120,192,
43,55,20,9,108,207,0,15,58,29,132,133,58,2,35,49,6,35,231,99,36,111,70,57,25,208,233,66,66,66,24,54,108,24,253,250,245,163,87,175,94,244,234,213,139,110,221,186,17,19,19,67,92,92,28,93,187,118,37,42,42,
138,240,240,112,34,34,34,154,14,77,47,54,87,81,81,65,89,89,25,165,165,165,148,150,150,82,82,82,66,81,81,209,41,71,227,212,220,93,187,118,177,127,255,126,167,67,110,180,133,83,147,51,135,157,13,199,167,
61,69,251,254,189,37,103,238,5,224,62,167,131,16,241,35,97,24,227,163,113,174,215,49,64,95,71,35,58,73,82,82,18,131,7,15,166,71,143,30,244,234,213,171,233,65,87,183,110,221,136,142,142,110,26,35,53,142,
147,194,195,195,9,14,14,118,58,108,159,82,81,81,65,69,69,5,229,229,229,148,149,149,113,236,216,49,142,29,59,70,81,81,17,71,143,30,165,160,160,128,130,130,2,114,114,114,200,201,201,97,199,142,29,222,220,
113,169,173,118,1,155,92,199,215,24,75,192,3,122,217,183,191,36,107,18,129,84,64,85,173,68,206,92,41,112,14,224,51,223,86,109,18,6,156,237,58,70,158,244,234,87,91,29,196,198,198,210,175,95,63,122,244,
232,65,92,92,28,209,209,209,68,70,70,18,21,21,69,88,88,24,157,59,119,38,42,42,138,142,29,59,210,169,83,39,162,162,162,232,212,169,19,161,161,161,77,175,65,65,65,132,135,135,3,208,177,99,199,166,39,88,
141,231,26,117,234,212,169,77,75,186,106,107,107,169,174,174,110,250,185,162,162,162,233,124,85,149,177,155,116,101,101,37,117,117,117,212,212,212,80,85,85,69,69,69,5,85,85,85,84,86,86,82,81,81,65,117,
117,117,211,128,163,180,180,180,105,176,81,88,88,200,145,35,71,56,116,232,144,47,37,94,154,179,3,99,208,177,197,117,108,197,152,17,32,45,51,0,99,12,208,197,225,56,196,183,28,199,248,55,77,179,208,68,218,
38,30,227,225,86,227,113,62,198,204,101,191,208,88,55,167,123,247,238,77,227,163,174,93,187,18,30,30,222,52,70,234,212,169,19,17,17,17,77,239,195,195,195,155,198,70,193,193,193,116,238,108,76,208,235,
212,169,19,29,58,116,32,40,40,168,233,28,24,99,166,182,60,52,171,170,170,162,174,174,174,233,231,202,202,74,234,235,235,169,171,171,107,74,148,52,142,139,234,234,234,56,126,252,56,85,85,85,84,85,85,81,
94,94,78,101,101,101,211,251,242,242,114,142,31,63,78,121,121,57,69,69,69,77,227,162,131,7,15,154,214,25,242,97,233,24,91,104,111,193,72,206,108,198,216,49,86,78,226,47,201,26,128,91,128,57,40,97,35,114,
38,74,129,59,129,119,156,14,196,135,244,226,68,226,102,4,48,24,24,2,68,59,24,147,248,183,98,140,129,199,14,32,205,117,108,163,157,175,167,182,201,205,24,99,0,37,108,4,140,68,205,157,192,155,78,7,34,210,
78,197,96,212,187,25,9,12,227,196,88,201,175,30,116,137,79,40,194,24,15,165,98,140,145,82,49,198,73,21,78,6,229,47,252,41,89,3,198,12,155,59,49,166,238,169,134,141,72,243,26,43,164,207,33,240,102,212,
156,169,56,96,40,198,54,226,131,93,71,50,48,16,99,153,149,200,65,32,3,216,131,241,100,104,187,235,56,232,100,80,1,96,0,39,198,0,170,97,19,152,190,230,196,191,105,154,81,35,98,191,120,140,164,77,18,198,
56,41,9,99,140,52,204,201,160,196,39,100,0,187,49,198,69,187,78,122,61,226,100,80,254,206,223,146,53,39,235,128,102,217,136,120,82,138,127,238,250,228,203,250,96,124,97,108,60,206,58,237,189,180,15,53,
24,187,47,101,2,7,48,18,157,123,48,190,28,102,160,153,50,190,32,4,205,178,9,52,199,209,242,65,17,95,21,130,81,251,38,9,227,225,214,0,160,159,235,181,63,198,248,73,252,91,1,198,120,40,211,245,218,248,126,
159,235,181,218,205,231,164,13,252,57,89,35,34,226,75,130,49,158,56,245,193,88,98,149,128,177,133,102,239,147,222,247,194,152,185,35,206,169,4,114,128,60,32,215,117,228,97,36,103,26,19,52,135,156,10,78,
68,68,164,29,234,136,145,204,233,135,49,38,106,28,43,245,114,189,239,233,122,237,228,84,128,1,238,48,198,216,232,16,198,44,225,211,223,103,97,60,4,22,155,41,89,35,34,98,175,16,140,53,223,113,64,119,160,
135,235,125,44,198,96,37,14,99,173,120,87,140,250,57,141,135,150,96,153,43,1,10,49,158,248,52,30,71,92,199,201,63,55,38,103,142,57,19,166,136,136,136,52,35,6,35,129,19,203,169,227,163,238,174,163,241,
231,110,24,99,163,174,24,15,203,228,132,18,140,26,122,197,24,245,98,142,186,142,35,24,73,153,163,24,99,163,188,147,218,2,122,199,37,95,166,100,141,136,136,127,232,204,169,9,156,46,64,132,235,53,12,8,119,
181,135,185,174,141,118,189,118,114,181,117,196,24,208,116,57,233,126,157,128,32,215,231,78,22,129,145,84,58,83,213,156,186,92,161,248,164,243,141,5,229,202,49,6,7,85,24,203,27,74,92,159,169,192,120,122,
83,133,177,220,168,220,245,243,49,215,53,199,78,58,74,0,107,254,21,19,17,17,17,127,16,133,49,142,105,28,35,53,190,15,199,24,243,132,99,140,141,204,222,119,196,40,173,17,233,186,87,4,16,202,169,227,37,
92,159,105,203,67,179,10,78,45,77,80,230,250,185,150,19,75,171,27,199,69,117,24,227,158,10,140,113,81,137,171,173,210,117,190,12,99,220,84,134,49,190,58,198,137,228,76,49,26,23,181,43,74,214,136,136,136,
153,182,252,242,213,64,65,68,68,68,218,19,141,139,196,118,62,159,172,17,17,17,17,17,17,17,17,145,182,211,218,65,17,17,17,17,17,17,17,17,31,162,100,141,136,136,136,136,136,136,136,136,15,81,178,70,68,68,
68,68,68,68,68,196,135,40,89,35,34,34,34,34,34,34,34,226,67,148,172,17,17,17,17,17,17,17,17,241,33,74,214,136,136,136,136,136,136,136,136,248,16,37,107,68,68,68,68,68,68,68,68,124,136,146,53,34,34,34,
34,34,34,34,34,62,68,201,26,17,17,17,17,17,17,17,17,31,242,255,1,156,218,214,33,190,190,113,253,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* MainUI::logo_png = (const char*) resource_MainUI_logo_png;
const int MainUI::logo_pngSize = 19035;


//[EndFile] You can add extra defines here...
//[/EndFile]

