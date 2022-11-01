#pragma once
// Derived class to use the PolyBLEP Oscillator as an stk::Generator.

#include "PolyBLEP.h"

template<typename T>
class StkPolyBLEP : public stk::Generator<T>, public PolyBLEP {
public:
    StkPolyBLEP(T sampleRate, Waveform waveform = SINE, T initialFrequency = 440.0)
            : PolyBLEP(sampleRate, waveform, initialFrequency) {
        const int numFrames = 1;
        const int numChannels = 1;
        this->lastFrame_.resize(numFrames, numChannels, 0.0);
    }

    virtual ~StkPolyBLEP() {
    }

    virtual T tick() {
        return getAndInc();
    }
    virtual stk::StkFrames<T> &tick(stk::StkFrames<T> &frames, unsigned int channel = 0) override {
        for (size_t frame = 0; frame < frames.size(); ++frame) {
            frames(frame, channel) = getAndInc();
        }
        return frames;
    }

    virtual void sampleRateChanged(double newRate, double oldRate) override {
        PolyBLEP::setSampleRate(newRate);
    }

private:
};

