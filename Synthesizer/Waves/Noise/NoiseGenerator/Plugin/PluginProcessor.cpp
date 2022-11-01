/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Constructor
NoiseGeneratorPluginAudioProcessor::NoiseGeneratorPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

NoiseGeneratorPluginAudioProcessor::~NoiseGeneratorPluginAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout NoiseGeneratorPluginAudioProcessor::createParameterLayout()
{
    //std::vector<std::unique_ptr<RangedAudioParameter>> params;
    AudioProcessorValueTreeState::ParameterLayout layout;

    // add the respective parameters
    // BUTTONS
    layout.add(std::make_unique<AudioParameterBool>(WHITE_ID, WHITE_NAME, false));
    layout.add(std::make_unique<AudioParameterBool>(PINK_ID, PINK_NAME, false));
    layout.add(std::make_unique<AudioParameterBool>(BROWN_ID, BROWN_NAME, false));
    layout.add(std::make_unique<AudioParameterBool>(STATE_ID, STATE_NAME, true));
    layout.add(std::make_unique<AudioParameterBool>(DC_ID, DC_NAME, true));
    layout.add(std::make_unique<AudioParameterBool>(AVG_ID, AVG_NAME, true));
    // SLIDERS
    layout.add(std::make_unique<AudioParameterFloat>(LEVEL_ID, LEVEL_NAME, 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<AudioParameterFloat>(DC_SLIDER_ID, DC_SLIDER_NAME, 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<AudioParameterFloat>(AVG_SLIDER_ID, AVG_SLIDER_NAME, 1.0f, 2.0f, 1.0f)); // CHECK - min, max, default?

    return layout;
}

//==============================================================================
const juce::String NoiseGeneratorPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NoiseGeneratorPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool NoiseGeneratorPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool NoiseGeneratorPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double NoiseGeneratorPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NoiseGeneratorPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NoiseGeneratorPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NoiseGeneratorPluginAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String NoiseGeneratorPluginAudioProcessor::getProgramName(int index)
{
    return {};
}

void NoiseGeneratorPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void NoiseGeneratorPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void NoiseGeneratorPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NoiseGeneratorPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void NoiseGeneratorPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());



    //check noise type
    bool noiseIsWhite = treeState.getRawParameterValue(WHITE_ID)->load();
    bool noiseIsPink  = treeState.getRawParameterValue(PINK_ID)->load();
    bool noiseIsBrown = treeState.getRawParameterValue(BROWN_ID)->load();
    bool dc_filter    = treeState.getRawParameterValue(DC_ID)->load();
    bool smoothing    = treeState.getRawParameterValue(AVG_ID)->load();
    
    // TO DO
    // check filter slider state changes
    
    // get slider values
    float levelSliderValue = treeState.getRawParameterValue(LEVEL_ID)->load();
//    float dcSliderValue    = treeState.getRawParameterValue(DC_SLIDER_ID)->load();
//    int   avgSliderValue   = treeState.getRawParameterValue(AVG_SLIDER_ID)->load();
    
    // check slider values and update filters if changed
//    filterWhite.setDCfiltConst(dcSliderValue);
//    filterPink.setDCfiltConst(dcSliderValue);
//    filterBrown.setDCfiltConst(dcSliderValue);
//    if (avgSliderValue != smoothLength)
//    {
//        filterWhite.setSmoothLength(avgSliderValue);
//        filterPink.setSmoothLength(avgSliderValue);
//        filterBrown.setSmoothLength(avgSliderValue);
//        smoothLength = avgSliderValue;
//    }

    // check if noise is on
    if (treeState.getRawParameterValue(STATE_ID)->load())
    {
        // create a temp varaible to hold the dry signal
        float drySig;
        float wetSig;
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);

            // add noise to every sample in the buffer
            for (int sample = 0; sample < buffer.getNumSamples(); sample++)
            {
                if (noiseIsWhite)
                {
                    // fill dry signal with current sample
                    drySig = buffer.getSample(channel, sample);
                    // multiply by 1 - the slider value
                    drySig *= (1.0 - levelSliderValue);
                    // noise source
                    wetSig = random.nextFloat();
                    //smoothing
                    if (smoothing)
                        wetSig = filterWhite.smoothing_filter(wetSig);
                    // dc block
                    if (dc_filter)
                        wetSig = filterWhite.dc_blocking_filter(wetSig);
                    // level adjust
                    wetSig = wetSig * levelSliderValue;
                    // add the dry and the wet to mix
                    channelData[sample] = drySig + wetSig;

                }
                else if (noiseIsPink)
                {
                    // fill dry signal with current sample
                    drySig = buffer.getSample(channel, sample);
                    // multiply by 1 - the slider value
                    drySig *= (1.0 - levelSliderValue);
                    // noise source
                    wetSig = nP.generate();
                    // smoothing
                    if (smoothing)
                        wetSig = filterPink.smoothing_filter(wetSig);
                    // dc filter
                    if (dc_filter)
                        wetSig = filterPink.dc_blocking_filter(wetSig);
                    // level adjust
                    wetSig = wetSig * levelSliderValue;
                    // add the dry and the wet to mix
                    channelData[sample] = drySig + wetSig;
                }
                else if (noiseIsBrown)
                {
                    // fill dry signal with current sample
                    drySig = buffer.getSample(channel, sample);
                    // multiply by 1 - the slider value
                    drySig *= (1.0 - levelSliderValue);
                    // noise source
                    wetSig = nB.generate();
                    // smoothing
                    if (smoothing)
                        wetSig = filterBrown.smoothing_filter(wetSig);
                    // dc filter
                    if (dc_filter)
                        wetSig = filterBrown.dc_blocking_filter(wetSig);
                    // level adjust
                    wetSig = wetSig * levelSliderValue;
                    // add the dry and the wet to mix
                    channelData[sample] = drySig + wetSig;
                }
                else
                    channelData[sample] = buffer.getSample(channel, sample); // should never happen, but here as a catch
            }
        }
    }
    // if noise is off, use the slider as a level adjust
    else
    {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);

            // modify the volume by multiplying each sample value
            for (int sample = 0; sample < buffer.getNumSamples(); sample++)
            {
                channelData[sample] = buffer.getSample(channel, sample) * levelSliderValue;
            }
        }
    }
}

//==============================================================================
bool NoiseGeneratorPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NoiseGeneratorPluginAudioProcessor::createEditor()
{
    return new NoiseGeneratorPluginAudioProcessorEditor(*this);
}

//==============================================================================
void NoiseGeneratorPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    std::unique_ptr<juce::XmlElement> xml(treeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NoiseGeneratorPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // error checking
    if (xmlState != nullptr)
        if (xmlState->hasTagName(treeState.state.getType()))
            treeState.state = juce::ValueTree::fromXml(*xmlState);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NoiseGeneratorPluginAudioProcessor();
}
