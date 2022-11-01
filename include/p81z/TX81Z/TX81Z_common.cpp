#include "TX81Z_common.h"

namespace TX81Z {
const float feedbackValues[] = {0.0f,  0.008f, 0.015f, 0.024f,
                                0.07f, 0.12f,  0.19f,  0.26f};

const float keyVelocityMapFactor1[] = {0,       -34.2f,  -59.6f,  -110.5f,
                                       -145.5f, -184.7f, -147.4f, -98.8f};
const float keyVelocityMapFactor2[] = {0,      83.9f,  146.5f, 266.3f,
                                       351.8f, 447.4f, 366.2f, 259.8f};
const float keyVelocityMapFactor3[] = {0,       -76.2f,  -135.9f, -236.0f,
                                       -313.0f, -399.4f, -346.2f, -274.3f};
const float keyVelocityMapFactor4[] = {0,      36.7f,  69.2f,  110.7f,
                                       147.4f, 188.2f, 185.8f, 178.0f};
const float keyVelocityMapFactor5[] = {0,      -15.5f, -24.8f, -34.2f,
                                       -43.7f, -53.8f, -59.7f, -64.9f};

float keyVelocityMapping(int level, float velocity)
{
    float velocity2 = velocity * velocity;

    float dB = keyVelocityMapFactor1[level] * (velocity2 * velocity2) +
               keyVelocityMapFactor2[level] * (velocity2 * velocity) +
               keyVelocityMapFactor3[level] * velocity2 +
               keyVelocityMapFactor4[level] * velocity + keyVelocityMapFactor5[level];

    return dBtoGain(dB);
}

float levelToGain(int level)
{
    return dBtoGain(0.74f * float(level + 1) - 73.26f);
}

float64 envelopeDecayLevel(int level)
{
    return (level == 0) ? 0.0f : dBtoGain(-3.0f * float(15 - level));
}

float64 enveloperAttackRate(float64 sampleRate, float rate, float adjustment)
{
    float64 attackSeconds = pow(2.0, (3.5 - ((rate + adjustment) * 0.5f)));

    // Modelled as a linear attack
    return clamp(1.0 / (sampleRate * attackSeconds), 0.0, 1.0);
}

// At setting 11, decay time is 1 sec for -24db of attenuation
// Reducing the rate by 2 doubles the decay time
float64 enveloperDecayRateFactor(float64 sampleRate, float rate, float adjustment)
{
    if (rate == 0)
        return 1.0;

    float64 attenuationTime = pow(2.0, (5.5 - ((rate + adjustment) * 0.5)));
    float64 samplesPer24db = sampleRate * attenuationTime;

    return 1.0 / exp(log(16.0) / samplesPer24db);
}

// Release rate 1 = decay rate 3, with each change in release rate
// being two steps of decay rate
float64 envelopeReleaseRateFactor(float64 sampleRate, float rate, float adjustment)
{
    return enveloperDecayRateFactor(sampleRate, 1.0f + rate * 2.0f, adjustment);
}

// Rate scaling affects the envelope rate based on the note being played
// Modelled as linear between a lower and upper note
float rateScalingFactor(int rateScaling, float note)
{
    constexpr float lowerAttenuationFactors[4] = {-0.5f, -0.5f, 0.0f, 0.5f};
    constexpr float upperAttenuationFactors[4] = {1.0f, 3.0f, 7.0f, 15.0f};

    float lowerNote = 28.0f;
    float upperNote = 110.0f;
    float lowerAttenuation = lowerAttenuationFactors[rateScaling];
    float upperAttenuation = upperAttenuationFactors[rateScaling];

    if (note < lowerNote)
        return lowerAttenuation;

    if (note > upperNote)
        return upperAttenuation;

    float notePosition = ((note - lowerNote) / (upperNote - lowerNote));
    return lowerAttenuation + (upperAttenuation - lowerAttenuation) * notePosition;
}

static inline double sinOfPhase(double phase)
{
    return std::sin(phase * (2 * M_PI));
}
static inline double sinOf2Phase(double phase)
{
    return std::sin(phase * (4 * M_PI));
}

// 8 waveshapes defined within the instrument, all variations on the sin function
static double getWaveshape(int waveshape, double phase)
{
    switch (waveshape) {
    case 0:
        return sinOfPhase(phase);

    case 1:
        if (phase < 0.25)
            return sinOfPhase(phase - 0.25) + 1.0;
        if (phase < 0.5)
            return sinOfPhase(phase + 0.25) + 1.0;
        if (phase < 0.75)
            return sinOfPhase(phase - 0.25) - 1.0;
        return sinOfPhase(phase + 0.25) - 1.0;

    case 2:
        return phase < 0.5 ? sinOfPhase(phase) : 0.0;

    case 3:
        if (phase < 0.25)
            return sinOfPhase(phase - 0.25) + 1.0;
        if (phase < 0.5)
            return sinOfPhase(phase + 0.25) + 1.0;
        return 0.0;

    case 4:
        return phase < 0.5 ? sinOfPhase(2.0 * phase) : 0.0;

    case 5:
        if (phase < 0.125)
            return sinOf2Phase(phase - 0.125) + 1.0;
        if (phase < 0.25)
            return sinOf2Phase(phase + 0.125) + 1.0;
        if (phase < 0.375)
            return sinOf2Phase(phase - 0.125) - 1.0;
        if (phase < 0.5)
            return sinOf2Phase(phase + 0.125) - 1.0;
        return 0.0;

    case 6:
        if (phase < 0.25)
            return sinOf2Phase(phase);
        if (phase < 0.5)
            return -sinOf2Phase(phase);
        return 0.0;

    case 7:
        if (phase < 0.125)
            return 1.0 + sinOf2Phase(phase - 0.125);
        if (phase < 0.25)
            return 1.0 + sinOf2Phase(phase + 0.125);
        if (phase < 0.375)
            return 1.0 - sinOf2Phase(phase - 0.125);
        if (phase < 0.5)
            return 1.0 - sinOf2Phase(phase + 0.125);
        return 0.0;
    }

    return 0.0;
}

typedef std::array<float, wavetableSize + 1> Wavetable;
static Wavetable waveRom[8];

void initWavetables()
{
    for (unsigned waveshape = 0; waveshape < 8; ++waveshape) {
        Wavetable &table = waveRom[waveshape];
        for (unsigned i = 0; i < table.size(); ++i) {
            double phase = (double)i / table.size();
            table[i] = getWaveshape(waveshape, phase);
        }
        table[wavetableSize] = table[0];
    }
}

const float *getWavetable(unsigned waveshape)
{
    return waveRom[waveshape].data();
}

}  // namespace TX81Z
