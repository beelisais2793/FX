/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MainUI.h"
#include "TrioLookAndFeel.h"
#include "AttachmentFactory.h"

//==============================================================================
LupoAudioProcessorEditor::LupoAudioProcessorEditor (LupoAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setLookAndFeel(&tlf);
    setSize (1000, 900);	
	setResizable(true, true);
	mainUI = new MainUI(&p, p.getFactory());
	addAndMakeVisible(mainUI);
	mainUI->updatePresetList();
	loadSettings();
}

LupoAudioProcessorEditor::~LupoAudioProcessorEditor()
{
	saveSettings();
	setLookAndFeel(nullptr);
	if (JUCEApplication::isStandaloneApp()) {
		delete mainUI;
	}
}

//==============================================================================
void LupoAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
	g.fillAll(Colours::lightgrey);
    g.setColour (Colours::white);

}

void LupoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


void LupoAudioProcessorEditor::saveSettings()
{
	String userHome = File::getSpecialLocation(File::userHomeDirectory).getFullPathName();
	File appDir = File(userHome + "/.Lupo");

	if (!appDir.exists()) {
		appDir.createDirectory();
	}

	File configFile = File(userHome + "/.Lupo/settings.xml");

	if (!configFile.exists()) {
		configFile.create();
	}
	else {
		configFile.deleteFile();
		configFile = File(userHome + "/.Lupo/settings.xml");
		configFile.create();
	}

	ValueTree* v = new ValueTree("Settings");

	

	std::unique_ptr<XmlElement> xml = v->createXml();
	xml->writeToFile(configFile, "");

	xml = nullptr;
	delete v;
}

void LupoAudioProcessorEditor::loadSettings()
{
	String userHome = File::getSpecialLocation(File::userHomeDirectory).getFullPathName();

	File appDir = File(userHome + "/.Lupo");

	if (!appDir.exists()) {
		appDir.createDirectory();
	}

	File configFile = File(userHome + "/.Lupo/settings.xml");

	if (configFile.exists()) {
		std::unique_ptr<XmlElement> xml = XmlDocument(configFile).getDocumentElement();
		ValueTree v = ValueTree::fromXml(*xml.get());
		
		xml = nullptr;
	}
}