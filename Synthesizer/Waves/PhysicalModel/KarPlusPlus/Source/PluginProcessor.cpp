/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KarPlusPlus2AudioProcessor::KarPlusPlus2AudioProcessor()
: foleys::MagicProcessor  (juce::AudioProcessor::BusesProperties()
                           .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts (*this, nullptr, "ParamTreeID", createParams())
{
    // ====== GUI MAGIC =======
    
    //    FOLEYS_SET_SOURCE_PATH (__FILE__);
    magicState.setGuiValueTree (BinaryData::GUImagic_xml, BinaryData::GUImagic_xmlSize); // Load custom GUI
    analyser = magicState.createAndAddObject<foleys::MagicAnalyser>("input");
    
    // ====== CONSTRUCTOR TO SET UP POLYPHONY =======
    for (int i = 0; i < voiceCount; i++)
    {
        synth.addVoice (new MySynthVoice()); //Synth Voice makes the sound
    }

    synth.addSound (new MySynthSound()); // Synth Sound allocates
}

KarPlusPlus2AudioProcessor::~KarPlusPlus2AudioProcessor()
{
}

//==============================================================================
const juce::String KarPlusPlus2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KarPlusPlus2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool KarPlusPlus2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool KarPlusPlus2AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double KarPlusPlus2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KarPlusPlus2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KarPlusPlus2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void KarPlusPlus2AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String KarPlusPlus2AudioProcessor::getProgramName(int index)
{
    return {};
}

void KarPlusPlus2AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}
 
// =============== PREPARE TO PLAY - SAMPLERATE SETUP ====================
void KarPlusPlus2AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    analyser->prepareToPlay (sampleRate, samplesPerBlock);

    for (int i = 0; i < voiceCount; i++)
    {
        MySynthVoice* v = dynamic_cast<MySynthVoice*>(synth.getVoice(i)); //returns a pointer to synthesiser voice
        v->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    }
}

void KarPlusPlus2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KarPlusPlus2AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
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

// =============== PROCESS BLOCK ====================
void KarPlusPlus2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    magicState.processMidiBuffer (midiMessages, buffer.getNumSamples());
    
    // ====== UPDATE PARAMETERS =======
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        // ====== SYNTH VOICE CAST =======
        if (auto voice = dynamic_cast<MySynthVoice*>(synth.getVoice(i)))
        {
            // ====== PARAMETERS =======
            auto& attackParam = *apvts.getRawParameterValue("ATTACK");
            auto& decayParam = *apvts.getRawParameterValue("DECAY");
            auto& sustainParam = *apvts.getRawParameterValue("SUSTAIN");
            auto& releaseParam = *apvts.getRawParameterValue("RELEASE");
            
            auto& oscWaveChoice = *apvts.getRawParameterValue("OSC");
            
            auto& loPassParam = *apvts.getRawParameterValue("LOPASS");
            
            auto& dampStringParam = *apvts.getRawParameterValue("DAMPSTRING");
            auto& feedbackParam = *apvts.getRawParameterValue("FEEDBACK");
   
            auto& VelToLoPassParam = *apvts.getRawParameterValue("VELTOLOPASS");
            auto& velToDampStringParam = *apvts.getRawParameterValue("VELTODAMPENSTRING");
            auto& velToFeedbackParam = *apvts.getRawParameterValue("VELTOFEEDBACK");
            
            auto& volumeParam = *apvts.getRawParameterValue("VOLUME");
            
            // ====== CONVERT ATOMIC PARAMETERS TO FLOATS =======
            voice->setParameterPointers(
                                        attackParam.load(),
                                        decayParam.load(),
                                        sustainParam.load(),
                                        releaseParam.load(),
                                        
                                        oscWaveChoice.load(),
                                        
                                        loPassParam.load(),
                                    
                                        dampStringParam.load(),
                                        feedbackParam.load(),
                                        
                                        VelToLoPassParam.load(),
                                        velToDampStringParam.load(),
                                        velToFeedbackParam.load(),
            
                                        volumeParam.load()
                );
        }
    }

    // ====== DSP PROCESSING =======
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    analyser->pushSamples (buffer);
}

//==============================================================================
//bool KarPlusPlus2AudioProcessor::hasEditor() const
//{
//    return false; // (change this to false if you choose to not supply an editor)
//}
//

//juce::AudioProcessorEditor* KarPlusPlus2AudioProcessor::createEditor()
//{
////    return new juce::GenericAudioProcessorEditor(*this);
////  return new foleys::MagicPluginEditor (magicState);
//    return new foleys::MagicPluginEditor (magicState); // , BinaryData::magic1_xml , BinaryData::magic1_xmlSize THIS SHOULD POINT TO THE CORRECT SIZE
//}

//==============================================================================
//void KarPlusPlus2AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
//{
////    auto state = apvts.copyState();
////    std::unique_ptr<juce::XmlElement> xml(state.createXml());
////    copyXmlToBinary(*xml, destData);
//
//    magicState.getStateInformation (destData);
//}
//
//void KarPlusPlus2AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
//{
//    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
//    if (xmlState.get() != nullptr)
//    {
//        if (xmlState->hasTagName(apvts.state.getType()))
//        {
//            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
//        }
//    }
//
////    magicState.setStateInformation (data, sizeInBytes, getActiveEditor());
//}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KarPlusPlus2AudioProcessor();

}

// Parameter Layout - DATA Side of the Plugin
juce::AudioProcessorValueTreeState::ParameterLayout KarPlusPlus2AudioProcessor::createParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"ATTACK", 1}, "Attack", 10.0f, 1000.0f, 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DECAY", 1}, "Decay", 0.0f, 1000.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"SUSTAIN", 1}, "Sustain", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"RELEASE", 1}, "Release", 10.0f, 1000.0f, 10.0f));

    // OSC
    params.push_back (std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { "OSC", 1}, "Oscillator", juce::StringArray { "Sine", "Triangle", "Square", "Saw", "Noise"}, 0));

    // LOPASS
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"LOPASS", 1}, "Lo Pass", 0.0f, 1.0f, 0.5f));
    
    // STRING
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DAMPSTRING", 1}, "Dampen String", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"FEEDBACK", 1}, "Feedback", 0.0f, 1.0f, 0.9f));

    
    // OUTPUT VOLUME
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"VOLUME", 1}, "Volume", 0.0f, 1.0f, 0.7f));

    
    // VEL TO PARAMS
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"VELTOLOPASS", 1}, "Vel -> Damp Ex", 0.0f, 1.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"VELTODAMPENSTRING", 1}, "Vel -> Dampen St", 0.0f, 1.0f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"VELTOFEEDBACK", 1}, "Vel -> Feedback", 0.0f, 1.0f, 0.1f));

    
     return { params.begin(), params.end() };
}
