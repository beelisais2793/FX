/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XylophoneModelAudioProcessor::XylophoneModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

XylophoneModelAudioProcessor::~XylophoneModelAudioProcessor()
{
}

//==============================================================================
const juce::String XylophoneModelAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool XylophoneModelAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool XylophoneModelAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool XylophoneModelAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double XylophoneModelAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int XylophoneModelAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int XylophoneModelAudioProcessor::getCurrentProgram()
{
    return 0;
}

void XylophoneModelAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String XylophoneModelAudioProcessor::getProgramName (int index)
{
    return {};
}

void XylophoneModelAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void XylophoneModelAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  for(int i =0; i< 12; ++i){
      bars.push_back(std::make_unique<idealBar>(octave4Lengths[i], 0.037, 0.019, sampleRate, (9.54 * pow(10, 9)), 796, 0.8, 0.05));
    }
}

void XylophoneModelAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool XylophoneModelAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void XylophoneModelAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    float* outputL = buffer.getWritePointer(0);
    float* outputR = buffer.getWritePointer(1);

    float output = 0.0;

    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        for(int j = 0; j<2; ++j){
            if(bars[j]->isInitialized){
                    output += (0.083)*(bars[j]->getSchemeOutput(bars[j]->excitationLocation));
                    bars[j]->updateOperation();
                    bars[j]->stateChange();
            }
        
        }
        /*output += (bars[1]->getSchemeOutput(bars[1]->excitationLocation));
        bars[1]->updateOperation();
        bars[1]->stateChange();*/
        
        
        output = limit(output);
        outputL[i] = output;
        outputR[i] = outputL[i];
    }
    
}

//==============================================================================
bool XylophoneModelAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* XylophoneModelAudioProcessor::createEditor()
{
    return new XylophoneModelAudioProcessorEditor (*this);
}

//==============================================================================
void XylophoneModelAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void XylophoneModelAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XylophoneModelAudioProcessor();
}

float XylophoneModelAudioProcessor::limit(float input){
    if (input>1)
        return 1;
    else if (input<-1)
        return -1;
    else
        return input;
}
