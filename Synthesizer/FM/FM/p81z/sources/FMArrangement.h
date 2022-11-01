#pragma once
#include "FMOperator.h"
#include <memory>

class FMArrangement {
public:
    void setup(float sampleRate);
    void clear();
    void adjustBuffer(unsigned size);
    void noteOn(int note, float velocity);
    void noteOff();
    void pitchBend(float bendSemitones);
    void run(float *audioOut, unsigned numFrames);

    bool hasReleased() const;
    bool hasFinished() const;

public:
    struct Settings {
        unsigned alg = 0;
        FMOperator::Settings op[4];
    };
    void useSettings(const Settings *s);

private:
    const Settings *settings_ = nullptr;

private:
    FMOperator ops_[4];
    std::unique_ptr<float[]> buffer_;
    std::unique_ptr<float[]> zeros_;
};
