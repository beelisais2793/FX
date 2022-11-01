/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <iostream>
#include <cmath>

//==============================================================================
/**
*/
class MoogVcftrapAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    MoogVcftrapAudioProcessor();
    ~MoogVcftrapAudioProcessor();
    
    float x[4] = {0,0,0,0}; //  state vector
    
    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

    //==============================================================================
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (! isUsingDoublePrecision());
        process (buffer, midiMessages);
    }

    void processBlock (AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (isUsingDoublePrecision());
        process (buffer, midiMessages);
    }

    //==============================================================================
    bool hasEditor() const override                                             { return true; }
    AudioProcessorEditor* createEditor() override;
    
    //==============================================================================
    const String getName() const override                                       { return JucePlugin_Name; }

    bool acceptsMidi() const override                                           { return true; }
    bool producesMidi() const override                                          { return true; }
    double getTailLengthSeconds() const override                                { return 0.0; }
    
    //==============================================================================
    int getNumPrograms() override                                               { return 0; }
    int getCurrentProgram() override                                            { return 0; }
    void setCurrentProgram (int /*index*/) override                             {}
    const String getProgramName (int /*index*/) override                        { return String(); }
    void changeProgramName (int /*index*/, const String& /*name*/) override     {}
    
    //==============================================================================
    void getStateInformation (MemoryBlock&) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void updateTrackProperties (const TrackProperties& properties) override;
    
    // this is kept up to date with the midi messages that arrive, and the UI component
    // registers with it so it can represent the incoming messages
    MidiKeyboardState keyboardState;
    
    // this keeps a copy of the last set of time info that was acquired during an audio
    // callback - the UI component will read this and display it.
    AudioPlayHead::CurrentPositionInfo lastPosInfo;
    
    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth = 400, lastUIHeight = 200;
    
    // Gain parameter
    AudioParameterFloat* gainParam = nullptr; // gain
    AudioParameterFloat* cutParam = nullptr; // cutoff frequency [Hz]
    AudioParameterFloat* resParam = nullptr; // resonance (a number between 0 and 1)
    
    // Current track colour and name
    TrackProperties trackProperties;

private:
    //==============================================================================
    template <typename FloatType>
    void process (AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages);
    template <typename FloatType>
    void applyGain (AudioBuffer<FloatType>&);
    template <typename FloatType>
    void applyMoog (AudioBuffer<FloatType>&);
    
    Synthesiser synth;
    
    void initialiseSynth();
    void updateCurrentTimeInfoFromHost();
    static BusesProperties getBusesProperties();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoogVcftrapAudioProcessor)
};
