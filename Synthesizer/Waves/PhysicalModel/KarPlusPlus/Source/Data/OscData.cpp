/*
  ==============================================================================

    OscData.cpp
    Created: 26 Jul 2022 11:01:58am
    Author:  Simon Weins

  ==============================================================================
*/

#include "OscData.h"

void OscData::setWaveType (const int choice)
{
    
    switch (choice)
    {
        case 0:
            // SINE WAVE
            initialise ([](float x) { return std::sin (x); });
            break;
            
        case 1:
            // SAW WAVE
            initialise ([](float x) { return x / juce::MathConstants<float>::pi; });
            break;
            
        case 2:
            // SQUARE
            initialise ([](float x) { return x < 0.0f ? -1.0f : 1.0f; });
            break;
            
        default:
            jassertfalse; /// Pause plugin execution if none of the above is selected
            break;
    }
}

void OscData::prepareToPlay (juce::dsp::ProcessSpec& spec)
{
    fmOsc.prepare (spec);
    prepare (spec);
};

void OscData::setWaveFrequency (const int midiNoteNumber)
{
    setFrequency (juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) + fmMod); /// Midi Note Number to Freq with frequency modulation
    lastMidiNote = midiNoteNumber; /// make midiNote available elsewhere
};

void OscData::getNextAudioBlock (juce::dsp::AudioBlock<float>& block)
{
    /// get the value of the wave at a given point in time
    for (int ch = 0; ch < block.getNumChannels(); ++ch)
    {
        for (int s = 0; s < block.getNumSamples(); ++s)
        {
            fmMod = fmOsc.processSample (block.getSample (ch, s)) * fmDepth;
        }
        
    }
    
    process(juce::dsp::ProcessContextReplacing<float> (block));  /// Process oscillator through buffer - block will contain all sinewave information
};

void OscData::setFmParams (const float depth, const float freq)
{
    fmOsc.setFrequency (freq);
    fmDepth = depth;
    setFrequency (juce::MidiMessage::getMidiNoteInHertz(lastMidiNote) + fmMod); /// call this function again to update new FM Values
}
