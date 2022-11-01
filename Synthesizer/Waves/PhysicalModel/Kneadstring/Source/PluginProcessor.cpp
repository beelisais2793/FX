/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//static variables
float SynthVoice::lowpassValue = 2000;
float SynthVoice::feedback = 0.98;

//==============================================================================
StringSynthesisKarplusAudioProcessor::StringSynthesisKarplusAudioProcessor()
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
    for (int i = 0; i < 5; i++){
    m_synth.addVoice(new SynthVoice);
    }
    
	m_synth.clearSounds();
	m_synth.addSound(new SynthSound);
    
}

StringSynthesisKarplusAudioProcessor::~StringSynthesisKarplusAudioProcessor()
{
}

//==============================================================================
const String StringSynthesisKarplusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StringSynthesisKarplusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StringSynthesisKarplusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StringSynthesisKarplusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StringSynthesisKarplusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StringSynthesisKarplusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StringSynthesisKarplusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StringSynthesisKarplusAudioProcessor::setCurrentProgram (int index)
{
}

const String StringSynthesisKarplusAudioProcessor::getProgramName (int index)
{
    return {};
}

void StringSynthesisKarplusAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void StringSynthesisKarplusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	m_synth.setCurrentPlaybackSampleRate(sampleRate);
}

void StringSynthesisKarplusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StringSynthesisKarplusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void StringSynthesisKarplusAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	auto numSamples = buffer.getNumSamples();

	m_synth.renderNextBlock(buffer, midiMessages, 0, numSamples);
}

//==============================================================================
bool StringSynthesisKarplusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StringSynthesisKarplusAudioProcessor::createEditor()
{
    return new StringSynthesisKarplusAudioProcessorEditor (*this);
}

//==============================================================================
void StringSynthesisKarplusAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StringSynthesisKarplusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StringSynthesisKarplusAudioProcessor();
}

