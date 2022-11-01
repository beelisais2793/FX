/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AudioEngine/MultimodeOscillator.h"
#include "AttachmentFactory.h"

//==============================================================================
LupoAudioProcessor::LupoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	String appDataPath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName();


	model.reset(new Model());
	matrix = new ModMatrix();

	lupo.reset(new LupoSynth(model.get(),matrix));
	parameters = new AudioProcessorValueTreeState(*this, nullptr);

	factory = new AttachmentFactory(this, lupo.get());

	factory->createParam("cutoff1", "Cutoff", 0.01f, 15000.0, 12000.0);
	factory->createParam("resonance1", "Resonance", 0.01f, 5.0f, 1.0);
	factory->createParam("envAmt1", "Filter amount", 0.01f, 1.0f, 1.0);
	factory->createParam("filterMode1", "Filter Mode", 0.0f, 1.0f, 0.0);

	factory->createParam("cutoff2", "Cutoff", 0.01f, 15000.0, 12000.0);
	factory->createParam("resonance2", "Resonance", 0.01f, 5.0f, 1.0);
	factory->createParam("envAmt2", "Filter amount", 0.01f, 1.0f, 1.0);
	factory->createParam("filterMode2", "Filter Mode", 0.0f, 1.0f, 0.0);

	factory->createParam("mainVolume", "Main Volume", 0.01f, 2.0f, 1.0);

	factory->createParam("ampAttack", "Attack", 0.0f, 10.0, 0.0);
	factory->createParam("ampDecay", "Decay", 0.0f, 10.0f, 1.0);
	factory->createParam("ampSustain", "Sustain", 0.0f, 1.0f, 0.0);
	factory->createParam("ampRelease", "Release", 0.0f, 10.0f, 1.0);

	factory->createParam("auxAttack1", "ENV 1 Attack", 0.0f, 10.0, 0.0);
	factory->createParam("auxDecay1", "ENV 1 Decay", 0.0f, 10.0f, 1.0);
	factory->createParam("auxSustain1", "ENV 1 Sustain", 0.0f, 1.0f, 0.0);
	factory->createParam("auxRelease1", "ENV 1Release", 0.0f, 10.0f, 1.0);

	factory->createParam("auxAttack2", "ENV 2 Attack", 0.0f, 10.0, 0.0);
	factory->createParam("auxDecay2", "ENV 2 Decay", 0.0f, 10.0f, 1.0);
	factory->createParam("auxSustain2", "ENV 2 Sustain", 0.0f, 1.0f, 0.0);
	factory->createParam("auxRelease2", "ENV 2 Release", 0.0f, 10.0f, 1.0);

	factory->createParam("dlyTimeLeft", "DelayL", 0.0f, 1000.0, 0.0);
	factory->createParam("dlyTimeRight", "DelayR", 0.0f, 1000.0f, 1.0);
	factory->createParam("dlyFeedback", "Feedback", 0.0f, 1.0f, 0.0);
	factory->createParam("dlyMix", "Mix", 0.0f, 1.0f, 0.0);

	factory->createParam("rvbRoomSize", "RoomSize", 0.0f, 1.0, 0.0);
	factory->createParam("rvbDamping", "Damping", 0.0f, 1.0f, 1.0);
	factory->createParam("rvbWetLevel", "WetLevel", 0.0f, 1.0f, 0.0);
	factory->createParam("rvbDryLevel", "DryLevel", 0.0f, 1.0f, 0.0);
	factory->createParam("rvbWidth", "Width", 0.0f, 1.0f, 0.0);
	factory->createParam("rvbFreezeMode", "Freeze", 0.0f, 1.0f, 0.0);
	factory->createParam("rvbEnabled", "Reverb enabled", 0, 1.0, 0);

	factory->createParam("chrDelay", "Delay", 0.0f, 1.0, 0.0);
	factory->createParam("chrModulation", "MOdulation", 0.0f, 1.0f, 1.0);
	factory->createParam("chrFeedback", "Feedback", 0.0f, 1.0f, 0.0);
	factory->createParam("chrMix", "Mix", 0.0f, 1.0f, 0.0);

	factory->createParam("osc1Pitch", "Osc1 pitch", -36, 36, 0);
	factory->createParam("osc1Fine", "Osc1 fine", -1.0, 1.0, 0);
	factory->createParam("osc1Volume", "Osc1 volume", 0, 1.0, 1);
	factory->createParam("osc1Pan", "Osc1 pan", -1.0, 1.0, 0);
	factory->createParam("osc1Shape", "Osc1 shape", 0, 3.0, 0);
	factory->createParam("osc1Spread", "Osc1 spread", 0, 1.0, 0);
	factory->createParam("osc1Enabled", "Osc1 enabled", 0, 1.0, 0);
	factory->createParam("osc1Sync", "Osc1 sync", 0, 1.0, 1);

	factory->createParam("osc2Pitch", "Osc2 pitch", -36, 36, 0);
	factory->createParam("osc2Fine", "Osc2 fine", -1.0, 1.0, 0);
	factory->createParam("osc2Volume", "Osc2 volume", 0, 1.0, 1);
	factory->createParam("osc2Pan", "Osc2 pan", -1.0, 1.0, 0);
	factory->createParam("osc2Shape", "Osc2 shape", 0, 3.0, 0);
	factory->createParam("osc2Spread", "Osc2 spread", 0, 1.0, 0);
	factory->createParam("osc2Enabled", "Osc2 enabled", 0, 1.0, 0);

	factory->createParam("osc3Pitch", "Osc3 pitch", -36, 36, 0);
	factory->createParam("osc3Fine", "Osc3 fine", -1.0, 1.0, 0);
	factory->createParam("osc3Volume", "Osc3 volume", 0, 1.0, 1);
	factory->createParam("osc3Pan", "Osc3 pan", -1.0, 1.0, 0);
	factory->createParam("osc3Shape", "Osc3 shape", 0, 3.0, 0);
	factory->createParam("osc3Spread", "Osc3 spread", 0, 1.0, 0);
	factory->createParam("osc3Enabled", "Osc3 enabled", 0, 1.0, 0);

	factory->createParam("osc4Pitch", "Osc4 pitch", -36, 36, 0);
	factory->createParam("osc4Fine", "Osc4 fine", -1.0, 1.0, 0);
	factory->createParam("osc4Volume", "Osc4 volume", 0, 1.0, 1);
	factory->createParam("osc4Pan", "Osc4 pan", -1.0, 1.0, 0);
	factory->createParam("osc4Shape", "Osc4 shape", 0, 3.0, 0);
	factory->createParam("osc4Spread", "Osc4 spread", 0, 1.0, 0);
	factory->createParam("osc4Enabled", "Osc4 enabled", 0, 1.0, 0);

	factory->createParam("lfo1Shape", "Lfo1 shape", 0, 4.0, 0);
	factory->createParam("lfo2Shape", "Lfo2 shape", 0, 4.0, 0);
	factory->createParam("lfo3Shape", "Lfo3 shape", 0, 4.0, 0);

	factory->createParam("lfo1Speed", "Lfo1 speed", 0, 10.0, 0);
	factory->createParam("lfo2Speed", "Lfo2 speed", 0, 10.0, 0);
	factory->createParam("lfo3Speed", "Lfo3 speed", 0, 10.0, 0);

	factory->createParam("lfo1Amount", "Lfo1 amount", 0, 1.0, 0);
	factory->createParam("lfo2Amount", "Lfo2 amount", 0, 1.0, 0);
	factory->createParam("lfo3Amount", "Lfo3 amount", 0, 1.0, 0);

	factory->createParam("fmAmount", "FM amount", 0, 1.0, 0);

	factory->createParam("distDrive", "Drive", 0, 5.0, 0);
	factory->createParam("distMix", "Distortion Mix", 0, 1.0, 0);
	factory->createParam("distMode", "Distortion MOde", 0, 3.0, 0);

	factory->createParam("arpEnabled", "Arp enabled", 0, 1.0, 0);
	factory->createParam("arpSpeed", "Arp speed", 0, 1.0, 0);
	factory->createParam("arpOctaves", "Arp Octaves", 0, 3.0, 0);
	factory->createParam("arpMode", "Arp mode", 0, 1.0, 0);
	
	factory->createParam("filterMode", "Filter mode", 0, 1.0, 0);
	factory->createParam("cutoffLink", "Cutoff link", 0, 1.0, 0);

	for (int i = 0; i < 6; i++) {
		factory->createParam("Source_" + String(i), "Matrix source " + String(i), 0.0f, 4.0f, 0.0);
		factory->createParam("Target_" + String(i), "Matrix target " + String(i), 0.0f, 5.0f, 0.0);
		factory->createParam("Amount_" + String(i), "Matrix amount " + String(i), 0.0f, 20.0f, 0.0);
	}

	bypass = parameters->createAndAddParameter("bypass", "bypass", "Bypass", NormalisableRange<float>(0, 1),0, nullptr, nullptr);
	
	Logger::getCurrentLogger()->writeToLog("Building preset list");

	String presetPath = appDataPath + "/Audio/Presets/pueski/Lupo/";
	
	File presets = File(presetPath);

	if (presets.exists() && presets.isDirectory()) {
		ScopedPointer<DirectoryIterator> iter = new DirectoryIterator(presets, false);
		while (iter->next()) {
			if (iter->getFile().exists() && !iter->getFile().isDirectory()) {
				String name = iter->getFile().getFileNameWithoutExtension();
				Logger::getCurrentLogger()->writeToLog("Found preset : " + name);
				programNames.push_back(name);
			}

		}
		iter = nullptr;

	}

	Logger::getCurrentLogger()->writeToLog("Found"+String(programNames.size())+" presets");
	
}

LupoAudioProcessor::~LupoAudioProcessor()
{
	if (JUCEApplication::isStandaloneApp()) {		
		// delete factory;
		lupo.reset();
		model.reset();
		delete matrix;
	}
// delete messageBus;
	Logger::getCurrentLogger()->writeToLog("Lupo is dead.");
}

//==============================================================================
const String LupoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LupoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LupoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LupoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LupoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LupoAudioProcessor::getNumPrograms()
{
	return std::max(1,(int)programNames.size());
}

int LupoAudioProcessor::getCurrentProgram()
{
	return std::max(0, (int)currentProgramNumber);
}

void LupoAudioProcessor::setCurrentProgram (int index)
{
	Logger::getCurrentLogger()->writeToLog("Setting program index "+String(index));

	if (index < 0 || index > programNames.size()) {
		index = 0;
	}

	if (programNames.size() > 0) {
		String name = programNames.at(index);
		this->currentProgramNumber = index;
		this->setSelectedProgram(name);
	}

}

void LupoAudioProcessor::setSelectedProgram(juce::String name) {

	Logger::getCurrentLogger()->writeToLog("Loading preset "+name);

	lupo->running = false;

	if (!prepared) {
		return;
	}

	String appDataPath = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName();
	String presetPath = appDataPath + "/Audio/Presets/pueski/Lupo/";

	String filename = name + ".xml";
	File preset = File(presetPath + filename);

	if (preset.exists()) {

		std::unique_ptr<XmlElement> xml = XmlDocument(preset).getDocumentElement();
		ValueTree state = ValueTree::fromXml(*xml.get());
		
		getValueTreeState()->state = state;

		xml = nullptr;
		this->selectedProgram = name;

		lupo->updateState(state);
		
		Logger::getCurrentLogger()->writeToLog("Updating synth state");
	}

	lupo->running = true;
}

const String LupoAudioProcessor::getProgramName (int index)
{
	if (programNames.size() < 1) {
		return "init";
	}

	if (getNumPrograms() > 0 && index < getNumPrograms() && index >= 0) {
		return programNames.at(index);
	}

	return "init";
}

void LupoAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LupoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	Logger::getCurrentLogger()->writeToLog("Preparing playback");
	if (!prepared) {
		prepared = true;
		// setSelectedProgram("init");
	}
	lupo->prepareToPlay(sampleRate, samplesPerBlock);

}

void LupoAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LupoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void LupoAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{

	lupo->processBlock(buffer,midiMessages);
}

AudioProcessorParameter * LupoAudioProcessor::getBypassParameter() const
{
	return bypass;
}



//==============================================================================
bool LupoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LupoAudioProcessor::createEditor()
{
    return new LupoAudioProcessorEditor (*this);
}

//==============================================================================
void LupoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	int program = getCurrentProgram();
	String name = getProgramName(program);
	destData.append(name.getCharPointer(),name.length());	
}

void LupoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	char* _name = (char*)data;	
	String name = juce::String(_name,sizeInBytes);
	setSelectedProgram(name);
	//lupo->updateState(getValueTreeState()->state);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LupoAudioProcessor();
}

LupoSynth	* LupoAudioProcessor::getSynth() {
	return lupo.get();
}

Model* LupoAudioProcessor::getModel() {
	return model.get();
}

AudioProcessorValueTreeState* LupoAudioProcessor::getValueTreeState() {
	return this->parameters;
}

