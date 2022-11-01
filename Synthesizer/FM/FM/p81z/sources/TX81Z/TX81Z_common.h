#pragma once
#include <algorithm>
#include <array>
#include <cmath>

namespace TX81Z {
typedef float float32;
typedef double float64;

template <class F> inline F dBtoGain(F x)
{
    return std::pow(F(10.0), F(0.05) * x);
}

template <class F> inline F clamp(F x, F xmin, F xmax)
{
    return std::max(xmin, std::min(xmax, x));
}

template <class F> inline F noteNumberToFrequency(F note)
{
    return F(440.0) * std::exp2((note - F(69.0)) * F(1.0 / 12.0));
}

// Tuning for how much operator modulation occurs
static constexpr float operatorFactor = 4.0f;

extern const float feedbackValues[];

extern const float keyVelocityMapFactor1[];
extern const float keyVelocityMapFactor2[];
extern const float keyVelocityMapFactor3[];
extern const float keyVelocityMapFactor4[];
extern const float keyVelocityMapFactor5[];

static float feedbackAmount(int level)
{
    return feedbackValues[level];
}

float keyVelocityMapping(int level, float velocity);

float levelToGain(int level);

float64 envelopeDecayLevel(int level);

float64 enveloperAttackRate(float64 sampleRate, float rate, float adjustment);

// At setting 11, decay time is 1 sec for -24db of attenuation
// Reducing the rate by 2 doubles the decay time
float64 enveloperDecayRateFactor(float64 sampleRate, float rate, float adjustment);

// Release rate 1 = decay rate 3, with each change in release rate
// being two steps of decay rate
float64 envelopeReleaseRateFactor(float64 sampleRate, float rate, float adjustment);

// Rate scaling affects the envelope rate based on the note being played
// Modelled as linear between a lower and upper note
float rateScalingFactor(int rateScaling, float note);

// 8 waveshapes defined within the instrument, all variations on the sin function
void initWavetables();
const float *getWavetable(unsigned waveshape);
enum { wavetableSize = 1024 };

}  // namespace TX81Z
