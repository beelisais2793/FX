#pragma once
#include "TX81Z_common.h"

namespace TX81Z {
//==============================================================================
class Envelope {
public:
    void setup(float sampleRate);
    void clear();
    void noteOn(int note, float velocity);
    void noteOff();
    void run(float *audioOut, unsigned numFrames);

    bool hasReleased() const { return status_ == kIdle; }
    bool hasFinished() const { return status_ == kIdle && value_ > envelopeLimit; }

private:
    float sampleRate_ = 0;

public:
    struct Settings {
        float attackRate = 0;
        float decay1Rate = 0;
        int decay1Level = 0;
        float decay2Rate = 0;
        float releaseRate = 0;
        int keyVelocitySensitivity = 0;
        int keyRateScaling = 0;
    };
    void useSettings(const Settings *s) { settings_ = s; }

private:
    const Settings *settings_ = nullptr;

private:
    enum {
        kAttack,
        kDecay1,
        kDecay2,
        kRelease,
        kIdle,
    };

    int status_ = kIdle;
    float32 keyScaling_ = 0;
    float64 attackFactor_ = 0, decay1Target_ = 0, decay1Factor_ = 0,
            decay2Factor_ = 0, releaseFactor_ = 0;
    static constexpr float64 envelopeLimit = 0.0001;

private:
    float64 value_ = 0;
};
}  // namespace TX81Z
