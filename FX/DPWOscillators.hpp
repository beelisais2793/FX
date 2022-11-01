// THESE CLASSES USE THE WELL-KNOWN DPW ALGORITHM (FIRST-ORDER) TO GENERATE SAWTOOTH AND RECTANGULAR 
// OSCILLATORS WITH REDUCED ALIASING. BASED ON THE ARTICLE:
// 
// "ALIAS-SUPPRESSED OSCILLATORS BASED ON DIFFERENTIATED POLYNOMIAL WAVEFORMS" BY
// V. VÄLIMÄKI, JUHAN NAM AND JULIUS SMITH
// IEEE TRANS. AUDIO, SPEECH, LANGUAGE PROCESS. (MAY 2010)
// 
// NOTES: THE DPW METHOD IS SIMPLE AND WORKS FINE FOR MOST TRIVIAL APPLICATIONS. HOWEVER, IT'S
// NOT RECOMMENDED FOR APPLICATIONS REQUIRING TIME-VARYING PITCH MODULATION OR OSCILLATOR SYNCHRONIZATION. FOR
// MORE ROBUST ALGORITHMS SEE THE E-PTR AND POLYBLEP METHODS.
//
// THIS CODE IS PROVIDED "AS-IS", WITH NO GUARANTEE OF ANY KIND.
// 
// CODED BY F. ESQUEDA - JULY 2013
// 
// ADAPTED FOR VCV RACK JANUARY 2018
// 
// TODO: 
// 		ADD PULSE WIDTH DEFINITON FOR SQUARE WAVE

#pragma once

#include "SoundObject.hpp"

namespace Analog::Oscillators::DPW
{
    class DPWSawtooth : public OscillatorProcessor
    {

    private:

        // Default sample rate. Use constructor to overwrite.
        DspFloatType sampleRate = 44100.0f;
        DspFloatType freq;
        DspFloatType state = 0.0f;
        DspFloatType phase = 0.0f;
        DspFloatType output = 0.0f;

    public:

        DPWSawtooth() {}
        DPWSawtooth(DspFloatType SR) : sampleRate(SR) { freq=440.0; }
        ~DPWSawtooth() {}

        void setFrequency(DspFloatType f) {
            freq = f;
        }
        void setSampleRate(DspFloatType SR) {
            sampleRate = SR;
        }

        void overridePhase(DspFloatType newPhase) {
            phase = newPhase; 
        }

        void generateSamples(DspFloatType f0) {

            // Implement DPW algorithm
            DspFloatType delta = f0/sampleRate;
            DspFloatType scalingFactor = sampleRate/(4.0f*f0);
            DspFloatType modPhase = 2.0f*phase - 1.0f;
            DspFloatType parWaveform = modPhase*modPhase;
            DspFloatType dyWaveform = parWaveform - state;

            state = parWaveform;
            output = scalingFactor * dyWaveform; 

            phase += delta;
            if (phase >= 1.0f)
                phase -= 1.0f;

        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType f0 = freq;
            DspFloatType p  = phase;
            phase = fmod(phase + Y,1);
            generateSamples(f0*X);
            phase = p;
            return getSawtoothWaveform();

        }
        DspFloatType getSawtoothWaveform() {
            return output;
        }

    };

    class DPWSquare : public OscillatorProcessor
    {

    private:

        DspFloatType output = 0.0;
        DspFloatType freq = 440.0;

        DPWSawtooth sawtoothOne;
        DPWSawtooth sawtoothTwo;

    public:

        DPWSquare() : OscillatorProcessor() { sawtoothTwo.overridePhase(0.5f); }
        DPWSquare(DspFloatType SR) : OscillatorProcessor() {
            sawtoothTwo.overridePhase(0.5f);
            setSampleRate(SR); 
        }

        ~DPWSquare() {}

        void setFrequency(DspFloatType f) {
            freq = f;
        }
        void setSampleRate(DspFloatType SR) {
            sawtoothOne.setSampleRate(SR);
            sawtoothTwo.setSampleRate(SR);
        }

        void generateSamples(DspFloatType f0) {

            // DPW Sawtooth is generated from two sawooth waveforms
            sawtoothOne.generateSamples(f0);
            sawtoothTwo.generateSamples(f0);
            output = sawtoothOne.getSawtoothWaveform() - sawtoothTwo.getSawtoothWaveform();
        }

        DspFloatType getSquareWaveform() {
            return output;
        }
        DspFloatType Tick(DspFloatType I=1, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType f0 = freq;
            DspFloatType p  = phase;
            phase = fmod(phase + Y,1);
            generateSamples(f0*X);
            phase = p;
            return getSquareWaveform();

        }
    };
}