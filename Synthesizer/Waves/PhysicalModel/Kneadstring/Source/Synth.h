//
//  Synth.h
//  String Synthesis (Karplus)
//
//  Created by asmprog on 03/04/2018.
//

#ifndef Synth_h
#define Synth_h

#define SAMPLE_RATE 44100

#include "../JuceLibraryCode/JuceHeader.h"
#include "Ramper.h"
#include "Delay.h"

//Class that says whether it can play midi notes
class SynthSound : public SynthesiserSound {
    
    public:
    
    SynthSound() {
        
    }
    
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
    
};

//Class that is the voice of my synth and contains the audio code
class SynthVoice : public SynthesiserVoice {
    Ramp m_ramp;
    DelayUnit m_leftDelay, m_rightDelay;
    IIRFilter m_leftFilter, m_rightFilter;
    IIRCoefficients m_filterCoeffs;
    Random random;
    
    public:
    
    //variables that are being changed in GUI
    static float lowpassValue, feedback;
    
    float sr, noteInDelay, tempOut, leftTemp, rightTemp, tailOff, vol, leftDelay, rightDelay;
    bool noteTail;
    
    
    
    SynthVoice() : m_ramp(SAMPLE_RATE, 0.01, 0.02), sr(SAMPLE_RATE), tailOff(0), vol(0), noteTail(false)
    {
        
        m_filterCoeffs = IIRCoefficients::makeLowPass(sr, lowpassValue);
        
        m_leftFilter.setCoefficients(m_filterCoeffs);
        m_rightFilter.setCoefficients(m_filterCoeffs);
        
		m_leftFilter.reset();
		m_rightFilter.reset();
        
    }
    
    ~SynthVoice()
    {
    }
    
    bool canPlaySound(SynthesiserSound* sound) override {
        return true;
    }
    
    //called to start a note
    void startNote(int midiNoteNum, float velocity, SynthesiserSound* sound, int /*currentpwPos*/) override {
        
        m_ramp.trigger(true);
        tailOff = 1.0;
        vol = velocity * 0.1;
        
        float noteInHz = MidiMessage::getMidiNoteInHertz(midiNoteNum);
        float mod = 1000.0f;
        noteInDelay = (mod / noteInHz) * 0.001f;
    
    }
    
    //called to stop a note
    void stopNote(float /*velocity*/, bool allowTailOff) override {
        noteTail = true;
        
    }

    //unused
    void pitchWheelMoved(int /*newpwVal*/) override {}
    //unused
    void controllerMoved(int /*contNum*/, int /*newContVal*/) override {}
    
    //setters and getters
    static void setLopVal(float val){
        lowpassValue = val;
    }
    
    static void setFeedbackVal(float val){
        feedback = val;
    }
    

    void renderNextBlock(AudioBuffer<float>& buffer, int startSample, int numSamples) override {
       
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);
        
		float output(0);
        

  
        for (startSample = 0; startSample < numSamples; startSample++) {
            //update when value changes
            m_filterCoeffs = IIRCoefficients::makeLowPass(sr, lowpassValue);

            m_leftFilter.setCoefficients(m_filterCoeffs);
            m_rightFilter.setCoefficients(m_filterCoeffs);
            
            
            output = random.nextFloat() * vol - (vol * 0.5f); //initial volume
            
            tempOut = output * m_ramp.getValue(); //apply ramp on noise gen
            
            leftDelay = m_leftDelay.getDelay(noteInDelay * SAMPLE_RATE); //noteInDelay is the delay time (changes the pitch)
            rightDelay = m_rightDelay.getDelay(noteInDelay * SAMPLE_RATE);
            
            leftChannel[startSample] +=  leftDelay * tailOff; //adding the delays into the buffer
            rightChannel[startSample] += rightDelay * tailOff;
            
            leftTemp = leftChannel[startSample] * tailOff; //filtering occurs here
            leftTemp = m_leftFilter.processSingleSampleRaw(tempOut + leftDelay);
            
            rightTemp = rightChannel[startSample] * tailOff;
            rightTemp = m_rightFilter.processSingleSampleRaw(tempOut + rightDelay);
            
            m_leftDelay.write(leftTemp * feedback); //writing the delays with feedback amount
            m_rightDelay.write(rightTemp * feedback);
            
            if (noteTail == true){
                tailOff *= 0.99999f;
                if (tailOff < 0.0005f){
                    m_leftFilter.reset();
                    m_rightFilter.reset();
                    clearCurrentNote();
                }
            }
            
            
        }
    }
};


#endif /* Synth_h */

