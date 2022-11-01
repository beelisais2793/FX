#pragma once
#include "Data/StringModel.h"
#include "Data/ADSR.h"
#include "Data/Oscillators.h"

class MySynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote      (int) override      { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override      { return true; }
};


class MySynthVoice : public juce::SynthesiserVoice
{
public:
    MySynthVoice() {}

    // ====== INITIALIZATE PARAMETER POINTERS =======
    void setParameterPointers(
                              float attackParam,
                              float decayParam,
                              float sustainParam,
                              float releaseParam,
                              
                              float oscWaveParam,
                              
                              float loPassParam,
                          
                              float dampStringParam,
                              float feedbackParam,
                              
                              float velToLoPassParam,
                              float velToDampStringParam,
                              float velToFeedbackParam,
  
                              float volumeParam
    )
    {
        attack = attackParam;
        decay = decayParam;
        sustain = sustainParam;
        release = releaseParam;
       
        oscType = oscWaveParam;
       
        loPass = loPassParam;
       
        dampening = dampStringParam;
        feedback = feedbackParam;
       
        velToLoPass = velToLoPassParam;
        velToDampening = velToDampStringParam;
        velToFeedback = velToFeedbackParam;

        volume = volumeParam;
    }

    // ====== SAMPLERATE SETUP FOR PREPARE TO PLAY =======
    void prepareToPlay(int sampleRate, int samplesPerBlock, int outputChannels)
    {
        // SET SAMPLERATE
        karplusStrong.setSamplerate (sampleRate);
        osc.setSampleRate (sampleRate);
        
        sr = sampleRate;
        
        karplusStrong.setSize (sampleRate * 1); // Delay size of 1000ms
        
        isPrepared = true;
    }
    
    // ====== PRODUCES PARAMETER VALUES RELATIVE TO INPUT VELOCITY =======
    float velToParam (float parameter, float velocity, float amount)
    {
        float velToParam = parameter;
        velToParam *= amount;
        velToParam *= velocity;
        velToParam += parameter;
        velToParam -= parameter * amount;
        
        return velToParam;
    }
    

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false;
        
        // ====== RElATIVE VELOCITY VALUES =======
        velToLoPass = velToParam (loPass, velocity, velToLoPass);
        velToDampening = velToParam (dampening, velocity, velToDampening);
        velToFeedback = velToParam (feedback, velocity, velToFeedback);

        float velToVol = velToParam (volume, velocity, 1.0f);

        // ====== SET NOTE PARAMETERS =======
        osc.setWaveType (oscType);
        //excitation.setDampening (velToLoPass);
        
        karplusStrong.setDampening (velToDampening);
        karplusStrong.setFeedback (velToFeedback);
        
        freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        karplusStrong.setPitch (freq);
        osc.setFrequency (freq);
        dcBlock.setCoefficients (juce::IIRCoefficients::makeHighPass (sr, freq));

        vol = velToVol;
        
        
        // ====== CALCULATE RELEASE TIME OF ADSR BASED ON FEEDBACK AND FREQUENCY =======
        float hzToMs = 1 / freq;
        relativeSustainTime = velToFeedback * hzToMs * 10000;
        
        // ====== TRIGGER ENVELOPES =======
        generalADSR.reset(); // clear out envelope before re-triggering it
        generalADSR.noteOn(); // start envelope

        impulseADSR.reset();
        impulseADSR.noteOn();

    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        // ====== TRIGGER OFF ENVELOPES =======
        generalADSR.noteOff();
        impulseADSR.noteOff();

        ending = true;
    }

     // ====== DSP BLOCK =======
    void renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override
    {
        jassert (isPrepared);
        
        if (! isVoiceActive())
            return;

        // ====== ADSR =======
        impulseADSR.updateADSR (attack, decay, sustain, release);
        generalADSR.updateADSR (0.1, relativeSustainTime, 1.0f, relativeSustainTime);

        // ====== DSP LOOP =======
        for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
        {
            // ====== ADSR =======
            float globalEnv = generalADSR.getNextSample(); // Global envelope
            float impulseEnv = impulseADSR.getNextSample(); // White Noise envelope
                            
            // ====== CHANNEL ASSIGNMENT =======
            for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
            {
                // ====== STEREO DSP =======
                float currentSample;
                currentSample = osc.process();
                currentSample = dcBlock.processSingleSampleRaw (currentSample);
                currentSample *= impulseEnv;
                
                currentSample = karplusStrong.process (currentSample);
                
                currentSample *= globalEnv; // ADSR
                currentSample *= vol; // Volume
                
                outputBuffer.addSample (chan, sampleIndex, currentSample);
                
                if (! generalADSR.isActive())
                    clearCurrentNote();
            }
                
    }
    }

    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<MySynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
    
private:
    // ====== NOTE ON/OFF =======   
    bool playing = false;
    bool ending = false;
    
    // ====== JASSERTS =======
    bool isPrepared { false };

    // ====== PARAMETER VALUES ======= 
    float attack;
    float decay;
    float sustain;
    float release;
    
    float oscType;
    
    float loPass;
    
    float dampening;
    float feedback;
    
    float velToLoPass;
    
    float velToDampening;
    float velToFeedback;
    
    float volume;
    
    // ====== ENVELOPES =======
    ADSRData generalADSR, impulseADSR;
    float relativeSustainTime;

    juce::Random random;
    juce::IIRFilter dcBlock;
    
    KarplusStrong karplusStrong;
    
    Oscillator osc;
    

    float freq; // Frequency of Synth
    float sr; // Samplerate
    float vol;
    juce::SmoothedValue<float> globalVol;
};

