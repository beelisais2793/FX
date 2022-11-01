
#ifndef SYNTH_H_INCLUDED
#define SYNTH_H_INCLUDED

#include "PhysicalModel.h"

const int MAX_POLYPHONY = 1;

// ================================================================================================
struct PercussionRendererSound : public SynthesiserSound
{
	PercussionRendererSound() {}
	bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
	bool appliesToChannel(int /*midiChannel*/) override { return true; }
};
// ================================================================================================

class PercussionRendererVoice : public SynthesiserVoice
{
public:
    
    PercussionRendererVoice(float ten,float damp,float thik,int mtrl,float vol): Tens(ten),Damp(damp),Thik(thik),Mtrl(mtrl), Vol(vol)     {}
    
	bool canPlaySound(SynthesiserSound* sound) override { return sound != nullptr; }
	
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        note = new PercussionRenderer(midiNoteNumber, velocity, (float)getSampleRate(), Tens, Damp, Thik, Mtrl, Vol);
    }
	
    void stopNote(float /*velocity*/, bool /*allowTailOff*/) override
    {
        
    }
    
    void pitchWheelMoved(int /*newValue*/) override
    {
        
    }

    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override
    {
        
    }
  
	void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        const int leftChanIndex = 0;
        const int rightChanIndex = 1;
        
		float* leftChanBuffer = outputBuffer.getWritePointer(leftChanIndex);
        
		if (note != nullptr)
        {
			note->renderToBuffer(&leftChanBuffer[startSample], numSamples, isKeyDown());
			
            FloatVectorOperations::copy(outputBuffer.getWritePointer(rightChanIndex), leftChanBuffer, numSamples);
            
            if (!note->isAlive())
            {
				clearCurrentNote();
                note = nullptr;
            }
		}
	}
    
private:
    
    ScopedPointer<PercussionRenderer> note;
    float Tens,Damp,Thik,Vol; int Mtrl;
};



class PercSynthPlayer
:
public AudioSource,
public Slider::Listener,
public ComboBox::Listener
{
private:
	AudioDeviceManager& inputDeviceManager;
	AudioDeviceManager& outputDeviceManager;
	MidiKeyboardState& keyboardState;
	Synthesiser synth;
	MidiMessageCollector midiCollector;
	AudioSourcePlayer audioSourcePlayer;

public:
	PercSynthPlayer(MidiKeyboardState& _state, AudioDeviceManager& _in, AudioDeviceManager& _out)
    :
    keyboardState(_state),
    inputDeviceManager(_in),
    outputDeviceManager(_out)
	{
		inputDeviceManager.addMidiInputCallback(String::empty, &midiCollector);
		inputDeviceManager.setMidiInputEnabled(MidiInput::getDevices()[0], true);
        
		outputDeviceManager.addAudioCallback(&audioSourcePlayer);

		audioSourcePlayer.setSource(this);
        
        for (int i = 0; i < MAX_POLYPHONY; ++i)
        {
            synth.addVoice(new PercussionRendererVoice(m_stiffness, m_damping, m_thickness, m_material, m_volume));
        }
        
        synth.clearSounds();
        
		synth.addSound(new PercussionRendererSound());
	}
    
	~PercSynthPlayer()
    {
		audioSourcePlayer.setSource(nullptr);
		inputDeviceManager.removeMidiInputCallback(String::empty, &midiCollector);
		outputDeviceManager.removeAudioCallback(&audioSourcePlayer);
		outputDeviceManager.closeAudioDevice();
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
		midiCollector.reset(sampleRate);
		synth.setCurrentPlaybackSampleRate(sampleRate);
	}
    
    void sliderValueChanged(Slider *slider) override
    {
        
        float sliderValue = slider->getValue();
        
        if (slider->getComponentID() == "stiffness")
        {
            m_stiffness = sliderValue;
        }
        else if (slider->getComponentID() == "damping")
        {
            m_damping = sliderValue;
        }
        else if (slider->getComponentID() == "thickness")
        {
            m_thickness = sliderValue;
        }
        else if (slider->getComponentID() == "volume")
        {
            m_volume = sliderValue;
        }

        synth.clearVoices();
        
        for (int i = 0; i < MAX_POLYPHONY; ++i)
        {
            synth.addVoice(new PercussionRendererVoice(m_stiffness, m_damping, m_thickness, m_material, m_volume));
        }
    }
    
    void comboBoxChanged(ComboBox *box) override
    {
        if (box->getComponentID() == "materials")
        {
            m_material = box->getSelectedId();
        }
        
        synth.clearVoices();
        
        for (int i = 0; i < MAX_POLYPHONY; ++i)
        {
            synth.addVoice(new PercussionRendererVoice(m_stiffness, m_damping, m_thickness, m_material, m_volume));
        }
    }

	void releaseResources() override {}
 
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

		// Pass these messages to the keyboard state so that it can update the component
		// to show on-screen which keys are being pressed on the physical midi keyboard.	
		const int startSample = 0;
		const bool injectIndirectEvents = true;  // add midi messages generated by clicking on the on-screen keyboard.
        keyboardState.processNextMidiBuffer(incomingMidi, startSample, bufferToFill.numSamples, injectIndirectEvents);

		synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, startSample, bufferToFill.numSamples);
	}
    
private:
    float m_stiffness { 0.f };
    float m_damping { 0.1f };
    float m_thickness { 0.05f };
    float m_volume { 50.f};
    int m_material { 1 };
};

#endif  // SYNTH_H_INCLUDED
