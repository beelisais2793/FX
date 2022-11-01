/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SquarewaveSynth.h"
#include <iostream>
#include <math.h>
#include </usr/local/Eigen/Dense>

using namespace std;
using namespace Eigen;

AudioProcessor* JUCE_CALLTYPE createPluginFilter();


//==============================================================================
MoogVcftrapAudioProcessor::MoogVcftrapAudioProcessor()
     : AudioProcessor (getBusesProperties())
{
    lastPosInfo.resetToDefault();
    
    // This creates our parameters. We'll keep some raw pointers to them in this class,
    // so that we can easily access them later, but the base class will take care of
    // deleting them for us.
    addParameter (gainParam  = new AudioParameterFloat ("gain",  "Overall Gain", 0.0f, 2.0f, 1.4f));
    addParameter (cutParam  = new AudioParameterFloat ("cutoff",  "Cutoff", 0.0f, 10000.0f, 10000.0f));
    addParameter (resParam  = new AudioParameterFloat ("resonance",  "Resonance", 0.0f, 1.0f, 0.0f));
    addParameter (rateParam  = new AudioParameterFloat ("rate",  "Rate", 0.0f, 10.0f, 0.0f));

    initialiseSynth();
}

MoogVcftrapAudioProcessor::~MoogVcftrapAudioProcessor()
{
}

void MoogVcftrapAudioProcessor::initialiseSynth()
{
    const int numVoices = 8;
    
    // Add some voices...
    for (int i = numVoices; --i >= 0;)
        synth.addVoice (new SineWaveVoice());
    
    // ..and give the synth a sound to play
    synth.addSound (new SineWaveSound());
}

//==============================================================================
bool MoogVcftrapAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Only mono/stereo and input/output must have same layout
    const AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();
    const AudioChannelSet& mainInput  = layouts.getMainInputChannelSet();
    
    // input and output layout must either be the same or the input must be disabled altogether
    if (! mainInput.isDisabled() && mainInput != mainOutput)
        return false;
    
    // do not allow disabling the main buses
    if (mainOutput.isDisabled())
        return false;
    
    // only allow stereo and mono
    if (mainOutput.size() > 2)
        return false;
    
    return true;
}

AudioProcessor::BusesProperties MoogVcftrapAudioProcessor::getBusesProperties()
{
    return BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
    .withOutput ("Output", AudioChannelSet::stereo(), true);
}

//==============================================================================
void MoogVcftrapAudioProcessor::prepareToPlay (double newSampleRate, int /*samplesPerBlock*/)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (newSampleRate);
    keyboardState.reset();
    previousGain = *gainParam;
    
    reset();
}

void MoogVcftrapAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void MoogVcftrapAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
}

template <typename FloatType>
void MoogVcftrapAudioProcessor::process (AudioBuffer<FloatType>& buffer,
                                           MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    
    // Now pass any incoming midi messages to our keyboard state object, and let it
    // add messages to the buffer if the user is clicking on the on-screen keys
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    
    // and now get our synth to process these midi events and generate its output.
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);
    
    // Apply the Moog effect to the new output
    applyMoog (buffer);
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamples);
    
    // apply the gain-change to the outgoing data
    applyGain (buffer);
    
    // Now ask the host for the current time so we can store it to be displayed later...
    updateCurrentTimeInfoFromHost();
}

template <typename FloatType>
void MoogVcftrapAudioProcessor::applyGain (AudioBuffer<FloatType>& buffer)
{
    // Use linear interpolation to determine gainLevel. Note that if you don't do interpolation you could produce some unwanted audio clips in your output signal (i.e. discontinuities) because your gain level parameter updates at a rate much faster than your sampling rate.

    auto currentGain = gainParam->get();
    
    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel)
        
        if (currentGain == previousGain)
        {
            buffer.applyGain (channel, 0, buffer.getNumSamples(), currentGain);
        }
        else
        {
            buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), previousGain, currentGain);
            previousGain = currentGain;
        }
}

template <typename FloatType>
void MoogVcftrapAudioProcessor::applyMoog (AudioBuffer<FloatType>& buffer)
{
    // Set constant parameters
    const float pi = MathConstants<double>::pi; // constant value of pi
    float SR = getSampleRate(); // sample rate [samples/sec]
    float k = 1/SR; // time step [sec]
    
    const int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        // Define signals
        auto channelData = buffer.getReadPointer (0); // input
        auto leftBuffer  = buffer.getWritePointer (0); // left output
        auto rightBuffer = buffer.getWritePointer (1); // right output
        auto in = channelData[i];
        
        // Set variable parameters
        float rate = *rateParam; // LFO rate [Hz]
        float fc = *cutParam + 1000*(0.5 + 0.5*cos(2*pi*rate*(count + i)/SR)); // cut off frequency [Hz]
        float wc = 2*pi*fc; // angular cut off frequency [rad/sec]
        float r = *resParam;// + sidechain[i]; // resonance (with sidechain modulation)
        
        float A[4][4] = { { -wc,  0,   0, -4*r*wc },
            {  wc, -wc,  0,       0 },
            {   0,  wc, -wc,      0 },
            {   0,   0,  wc,    -wc } }; // system matrix
        
        float b[4] = {wc,0,0,0}; // forcing vector
        float eye[4][4] = { { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 } }; // identity matrix
        
        // Compute matrix terms
        Matrix4f alpha;
        Matrix4f beta;
        
        for (int n = 0; n < 4; n++)
        {
            for (int m = 0; m < 4; m++)
            {
                alpha(n,m) = eye[n][m] + k*A[n][m]/2;
                beta(n,m) = eye[n][m] - k*A[n][m]/2;
            }
        }
        
        // Compute beta inverse
        Matrix4f invBeta = beta.inverse();
        
        // Calculate gamma
        float gamma1[4];
        float gamma2[4];
        float gamma[4];
        
        for (int j = 0; j < 4; j++)
        {
            gamma1[j] = alpha(j,0)*x[0] + alpha(j,1)*x[1] + alpha(j,2)*x[2] + alpha(j,3)*x[3];
            gamma2[j] = k*in*(alpha(j,0)*b[0] + alpha(j,1)*b[1] + alpha(j,2)*b[2] + alpha(j,3)*b[3]);
            gamma[j] = gamma1[j] + gamma2[j];
        }
        
        // Compute state update
        for (int j = 0; j < 4; j++)
        {
            x[j] = invBeta(j,0)*gamma[0] + invBeta(j,1)*gamma[1] + invBeta(j,2)*gamma[2] + invBeta(j,3)*gamma[3];
        }
        
        // Write output signal to left and right channels
        leftBuffer[i]  = x[3];
        rightBuffer[i] = x[3];
    }
    count += numSamples;
}

void MoogVcftrapAudioProcessor::updateCurrentTimeInfoFromHost()
{
    if (AudioPlayHead* ph = getPlayHead())
    {
        AudioPlayHead::CurrentPositionInfo newTime;
            
        if (ph->getCurrentPosition (newTime))
        {
            lastPosInfo = newTime;  // Successfully got the current time from the host..
            return;
        }
    }
        
    // If the host fails to provide the current time, we'll just reset our copy to a default..
    lastPosInfo.resetToDefault();
}
    
//==============================================================================
AudioProcessorEditor* MoogVcftrapAudioProcessor::createEditor()
{
    return new MoogVcftrapAudioProcessorEditor (*this);
}
    
//==============================================================================
void MoogVcftrapAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:
        
    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");
        
    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
        
    // Store the values of all our parameters, using their param ID as the XML attribute
    for (auto* param : getParameters())
        if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
            xml.setAttribute (p->paramID, p->getValue());
        
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}
    
void MoogVcftrapAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
        
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        
    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our last window size..
            lastUIWidth  = jmax (xmlState->getIntAttribute ("uiWidth", lastUIWidth), 400);
            lastUIHeight = jmax (xmlState->getIntAttribute ("uiHeight", lastUIHeight), 200);
                
            // Now reload our parameters..
            for (auto* param : getParameters())
                if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
                    p->setValue ((float) xmlState->getDoubleAttribute (p->paramID, p->getValue()));
        }
    }
}
    
void MoogVcftrapAudioProcessor::updateTrackProperties (const TrackProperties& properties)
{
    trackProperties = properties;
        
    if (auto* editor = dynamic_cast<MoogVcftrapAudioProcessorEditor*> (getActiveEditor()))
        editor->updateTrackProperties ();
}
    
//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MoogVcftrapAudioProcessor();
}
    
    

