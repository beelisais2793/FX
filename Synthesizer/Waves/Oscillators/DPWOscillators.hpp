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

namespace Oscillators
{
    class DPWSawtooth : public OscillatorProcessor
    {

    private:

        // Default sample rate. Use constructor to overwrite.
        double sampleRate = 44100.0f;
        double freq;
        double state = 0.0f;
        double phase = 0.0f;
        double output = 0.0f;

    public:

        DPWSawtooth() {}
        DPWSawtooth(double SR) : sampleRate(SR) { freq=440.0; }
        ~DPWSawtooth() {}

        void setFrequency(double f) {
            freq = f;
        }
        void setSampleRate(double SR) {
            sampleRate = SR;
        }

        void overridePhase(double newPhase) {
            phase = newPhase; 
        }

        void generateSamples(double f0) {

            // Implement DPW algorithm
            double delta = f0/sampleRate;
            double scalingFactor = sampleRate/(4.0f*f0);
            double modPhase = 2.0f*phase - 1.0f;
            double parWaveform = modPhase*modPhase;
            double dyWaveform = parWaveform - state;

            state = parWaveform;
            output = scalingFactor * dyWaveform; 

            phase += delta;
            if (phase >= 1.0f)
                phase -= 1.0f;

        }
        double Tick(double I=1, double A=1, double X=0, double Y=0)
        {
            double f0 = freq;
            double p  = phase;
            phase = fmod(phase + Y,1);
            generateSamples(f0*X);
            phase = p;
            return getSawtoothWaveform();

        }
        double getSawtoothWaveform() {
            return output;
        }

    };

    class DPWSquare : public OscillatorProcessor
    {

    private:

        double output = 0.0;
        double freq = 440.0;

        DPWSawtooth sawtoothOne;
        DPWSawtooth sawtoothTwo;

    public:

        DPWSquare() : OscillatorProcessor() { sawtoothTwo.overridePhase(0.5f); }
        DPWSquare(double SR) : OscillatorProcessor() {
            sawtoothTwo.overridePhase(0.5f);
            setSampleRate(SR); 
        }

        ~DPWSquare() {}

        void setFrequency(double f) {
            freq = f;
        }
        void setSampleRate(double SR) {
            sawtoothOne.setSampleRate(SR);
            sawtoothTwo.setSampleRate(SR);
        }

        void generateSamples(double f0) {

            // DPW Sawtooth is generated from two sawooth waveforms
            sawtoothOne.generateSamples(f0);
            sawtoothTwo.generateSamples(f0);
            output = sawtoothOne.getSawtoothWaveform() - sawtoothTwo.getSawtoothWaveform();
        }

        double getSquareWaveform() {
            return output;
        }
        double Tick(double I=1, double A=1, double X=0, double Y=0)
        {
            double f0 = freq;
            double p  = phase;
            phase = fmod(phase + Y,1);
            generateSamples(f0*X);
            phase = p;
            return getSquareWaveform();

        }
    };
}