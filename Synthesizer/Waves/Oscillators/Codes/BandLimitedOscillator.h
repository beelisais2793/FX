#pragma once
#include "PolyBLEP.h"
#include <vector>


namespace SoundWave
{
    struct BandlimitedOscillator
    {
        enum Waveform {
            SINE = PolyBLEP::Waveform::SINE,
            COSINE = PolyBLEP::Waveform::COSINE,
            TRIANGLE = PolyBLEP::Waveform::TRIANGLE,
            SQUARE = PolyBLEP::Waveform::SQUARE,
            RECTANGLE = PolyBLEP::Waveform::RECTANGLE,
            SAWTOOTH = PolyBLEP::Waveform::SAWTOOTH,
            RAMP = PolyBLEP::Waveform::RAMP,
            MODIFIED_TRIANGLE = PolyBLEP::Waveform::MODIFIED_TRIANGLE,
            MODIFIED_SQUARE = PolyBLEP::Waveform::MODIFIED_SQUARE,
            HALF_WAVE_RECTIFIED_SINE = PolyBLEP::Waveform::HALF_WAVE_RECTIFIED_SINE,
            FULL_WAVE_RECTIFIED_SINE = PolyBLEP::Waveform::FULL_WAVE_RECTIFIED_SINE,
            TRIANGULAR_PULSE = PolyBLEP::Waveform::TRIANGULAR_PULSE,
            TRAPEZOID_FIXED = PolyBLEP::Waveform::TRAPEZOID_FIXED,
            TRAPEZOID_VARIABLE = PolyBLEP::Waveform::TRAPEZOID_VARIABLE
        };

        PolyBLEP *osc;

        BandlimitedOscillator(double sampleRate, Waveform waveform = SINE, double initialFrequency = 440.0)
        {
            osc = new PolyBLEP(sampleRate, static_cast<PolyBLEP::Waveform>(waveform), initialFrequency);
            assert(osc != NULL);
        }
        ~BandlimitedOscillator() {
            if(osc) delete osc;
        }

        void setFrequency(double freqInHz) { osc->setFrequency(freqInHz); }

        void setSampleRate(double sampleRate) { osc->setSampleRate(sampleRate); }

        void setWaveform(Waveform waveform) { osc->setWaveform(static_cast<PolyBLEP::Waveform>(waveform)); }

        void setPulseWidth(double pw) { osc->setPulseWidth(pw); }

        double Tick() {
            double r = osc->getAndInc();
            return r;
        }
        double Tick(double in) {
            double r = in*osc->getAndInc();
            return r;
        }

        void Process(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = osc->getAndInc();
        }
        void Process(float * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = osc->getAndInc();
        }
        void sync(double phase) { osc->sync(phase); }

    };
}
