/*
  ==============================================================================

    LupoSynth.h
    Created: 14 Mar 2020 12:33:42pm
    Author:  mpue

  ==============================================================================
*/

#pragma once

#include "OscGroup.h"
#include "ADSR.h"
#include "MultimodeFilter.h"
#include "Voice.h"
#include "Oszillator.h"
#include "StereoDelay.h"
#include "StereoReverb.h"
#include "StereoChorus.h"
#include "Distortion.h"
#include "Arpeggiator.h"
#include "OscGroup.h"
#include "../Model.h"
#include "../ModMatrix.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "../DummyModulator.h"

class LupoSynth : public ChangeListener, 
			      public AudioProcessorValueTreeState::Listener, 
				  public AudioProcessorParameter::Listener,
				  public ChangeBroadcaster
{
	
public:

	LupoSynth(Model* model, ModMatrix* modMatrix);
	~LupoSynth();

	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages);	
	void processMidi(MidiBuffer& midiMessages);
	void prepareToPlay(double sampleRate, int bufferSize);	
	void updateState(ValueTree state);
	Arpeggiator* getArpeggiator();

	MultimodeOscillator* createOscillator(Oszillator::OscMode mode);
	void configureOscillators(Oszillator::OscMode mode1, Oszillator::OscMode mode2, Oszillator::OscMode mode3, Oszillator::OscMode mode4);
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void parameterChanged(const String& parameterID, float newValue) override;
	void parameterValueChanged(int parameterIndex, float newValue) override;
	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
	ModMatrix* getModMatrix();
	bool cutoffLink = false;
	bool running = true;
	void updateMatrix();
private:
	Model* model;
	Voice* voices[128];
	vector<SynthLab::ADSR*>* modEnvelopes;
	float sampleRate;
	int bufferSize;
	MultimodeFilter* filter1;
	MultimodeFilter* filter2;

	float filterMode = 1.0f;

	float cutoff = 15000.0f;
	float resonance = 1.0f;
	int currentSample = 0;
	int numVoices = 0;

	OscGroup* oscGroup1;
	OscGroup* oscGroup2;
	OscGroup* oscGroup3;
	OscGroup* oscGroup4;

	MultimodeOscillator* lfo1;
	MultimodeOscillator* lfo2;
	MultimodeOscillator* lfo3;

	DummyModulator* dummyModulator;

	StereoDelay* delay;
	StereoReverb* reverb;
	StereoChorus* chorus;
	Distortion* distortion;
	Arpeggiator* arp;
	ModMatrix* matrix;

	int highestNote = 0;

	float mainVolume = 1.0f;
	float* leftOut;
	float* rightOut;

	float elapsed;
	float lastTime;
	float currentTime;
	float deltaTime;

	float volume[4];
	float pan[4];
};