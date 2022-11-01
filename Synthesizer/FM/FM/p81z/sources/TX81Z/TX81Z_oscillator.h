#pragma once
#include "TX81Z_common.h"

namespace TX81Z {
//==============================================================================
struct Oscillator {
    void setup(float sampleRate);
    void clear();
    void noteOn(int note);
    void noteOff();
    void pitchBend(float bendSemitones);
    void run(const float *amplitudeIn, const float *modulatorIn,
             float *audioOut, unsigned numFrames);
    void runAdding(const float *amplitudeIn, const float *modulatorIn,
                   float *audioOut, unsigned numFrames);

private:
    void calculatePhaseIncrement();

    template <bool adding>
    void internalRun(const float *amplitudeIn, const float *modulatorIn,
             float *audioOut, unsigned numFrames);

private:
    float sampleRate_ = 0;

public:
    struct Settings {
        int waveshape = 0;
        int level = 0;
        float fixedPitch = 0;
        float multiplier = 0;
        int feedbackFactor = 0;
    };
    void useSettings(const Settings *s) { settings_ = s; }

private:
    const Settings *settings_ = nullptr;

private:
    float bendSemitones_ = 0, notePitch_ = 0, phase_ = 0, phaseIncrement_ = 0;

    bool noteActive_ = false;

    float oscillatorValue_ = 0;
};
}  // namespace TX81Z
