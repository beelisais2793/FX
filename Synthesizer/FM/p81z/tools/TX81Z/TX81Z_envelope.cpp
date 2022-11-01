#include "TX81Z_envelope.h"
#include <cstdio>
#include <cstring>

namespace TX81Z {
constexpr float64 Envelope::envelopeLimit;

void Envelope::setup(float sampleRate)
{
    sampleRate_ = sampleRate;
}

void Envelope::clear()
{
    status_ = kIdle;
    value_ = 0;
}

void Envelope::noteOn(int note, float velocity)
{
    const float sampleRate = sampleRate_;
    const Settings *settings = settings_;

    status_ = kAttack;
    keyScaling_ = TX81Z::keyVelocityMapping(settings->keyVelocitySensitivity, velocity);

    float rateScalingFactor = TX81Z::rateScalingFactor(settings->keyRateScaling, note);

    attackFactor_ = TX81Z::enveloperAttackRate(sampleRate, settings->attackRate, rateScalingFactor);
    decay1Target_ = TX81Z::envelopeDecayLevel(settings->decay1Level);
    decay1Factor_ = TX81Z::enveloperDecayRateFactor(sampleRate, settings->decay1Rate, rateScalingFactor);
    decay2Factor_ = TX81Z::enveloperDecayRateFactor(sampleRate, settings->decay2Rate, rateScalingFactor);
    releaseFactor_ = TX81Z::envelopeReleaseRateFactor(sampleRate, settings->releaseRate, rateScalingFactor);
}

void Envelope::noteOff()
{
    status_ = kIdle;
}

void Envelope::run(float *audioOut, unsigned numFrames)
{
    int status = status_;
    float64 value = value_;

    const float32 keyScaling = keyScaling_;
    const float64 attackFactor = attackFactor_;
    const float64 decay1Target = decay1Target_;
    const float64 decay1Factor = decay1Factor_;
    const float64 decay2Factor = decay2Factor_;
    const float64 releaseFactor = releaseFactor_;

    unsigned i = 0;

    while (i < numFrames) {
        switch (status) {
        case kAttack:
            status = (value < 1.0) ? kAttack : kDecay1;
            while (i < numFrames && status == kAttack) {
                // Model as linear attack - not accurate (more like an S shape in the instrument)
                value += attackFactor;
                audioOut[i++] = keyScaling * float(value);
                status = (value < 1.0) ? kAttack : kDecay1;
            }
            break;

        case kDecay1:
            status = (value > decay1Target) ? kDecay1 : kDecay2;
            while (i < numFrames && status == kDecay1) {
                value *= decay1Factor;
                audioOut[i++] = keyScaling * float(value);
                status = (value > decay1Target) ? kDecay1 : kDecay2;
            }
            break;

        case kDecay2:
            status = (value < envelopeLimit) ? kIdle : kDecay2;
            while (i < numFrames && status == kDecay2) {
                value *= decay2Factor;
                audioOut[i++] = keyScaling * float(value);
                status = (value < envelopeLimit) ? kIdle : kDecay2;
            }
            break;

        case kRelease:
            status = (value < envelopeLimit) ? kIdle : kRelease;
            while (i < numFrames && status == kRelease) {
                value *= releaseFactor;
                audioOut[i++] = keyScaling * float(value);
                status = (value < envelopeLimit) ? kIdle : kRelease;
            }
            break;

        case kIdle:
            std::memset(audioOut + i, 0, (numFrames - i) * sizeof(*audioOut));
            i = numFrames;
            break;
        }
    }

    status_ = status;
    value_ = value;
}
}  // namespace TX81Z
