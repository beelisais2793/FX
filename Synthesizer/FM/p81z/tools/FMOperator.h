#pragma once
#include "TX81Z/TX81Z_envelope.h"
#include "TX81Z/TX81Z_oscillator.h"
#include <memory>

class FMOperator {
public:
    void setup(float sampleRate);
    void clear();
    void adjustBuffer(unsigned size);
    void noteOn(int note, float velocity);
    void noteOff();
    void pitchBend(float bendSemitones);
    void run(const float *audioIn, float *audioOut, unsigned numFrames);
    void runAdding(const float *audioIn, float *audioOut, unsigned numFrames);

    bool hasReleased() const { return eg_.hasReleased(); }
    bool hasFinished() const { return eg_.hasFinished(); }

public:
    struct Settings {
        TX81Z::Envelope::Settings eg;
        TX81Z::Oscillator::Settings osc;
    };
    void useSettings(const Settings *s);

private:
    const Settings *settings_ = nullptr;

private:
    TX81Z::Envelope eg_;
    TX81Z::Oscillator osc_;

    unsigned bufferSize_ = 0;
    std::unique_ptr<float[]> buffer_;
};
