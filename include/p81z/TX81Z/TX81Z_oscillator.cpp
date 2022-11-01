#include "TX81Z_oscillator.h"
#include <cstdio>

namespace TX81Z {
void Oscillator::setup(float sampleRate)
{
    sampleRate_ = sampleRate;
}

void Oscillator::clear()
{
    noteActive_ = false;
    oscillatorValue_ = 0;
}

void Oscillator::noteOn(int note)
{
    notePitch_ = note;
    bendSemitones_ = 0.0f;
    calculatePhaseIncrement();

    if (!noteActive_)
        phase_ = 0.0f;

    noteActive_ = true;
}

void Oscillator::noteOff()
{
    noteActive_ = false;
}

void Oscillator::pitchBend(float bendSemitones)
{
    bendSemitones_ = bendSemitones;
    calculatePhaseIncrement();
}

void Oscillator::calculatePhaseIncrement()
{
    const Settings *settings = settings_;
    float noteFrequency =
        settings->fixedPitch + settings->multiplier * noteNumberToFrequency(notePitch_ + bendSemitones_);
    phaseIncrement_ = noteFrequency / sampleRate_;
}

template <bool adding>
void Oscillator::internalRun(const float *amplitudeIn, const float *modulatorIn,
                             float *audioOut, unsigned numFrames)
{
    float phase = phase_;
    float oscillatorValue = oscillatorValue_;
    const float phaseIncrement = phaseIncrement_;

    const Settings *settings = settings_;
    const float *table = TX81Z::getWavetable(settings->waveshape);

    float gain = levelToGain(settings->level);
    float feedback = feedbackAmount(settings->feedbackFactor);

    for (unsigned i = 0; i < numFrames; ++i) {
        phase += phaseIncrement;
        phase -= (int)phase;

        float phaseMod = phase + (operatorFactor * modulatorIn[i]) +
            (oscillatorValue * feedback);
        phaseMod -= (int)phaseMod;
        phaseMod = (phaseMod < 0) ? (1 + phaseMod) : phaseMod;

        const float frac = TX81Z::wavetableSize * phaseMod;
        const unsigned index = (int)frac;

        oscillatorValue = amplitudeIn[i] *
            (table[index] + (frac - index) * (table[index + 1] - table[index]));

        if (adding)
            audioOut[i] += gain * oscillatorValue;
        else
            audioOut[i] = gain * oscillatorValue;
    }

    phase_ = phase;
    oscillatorValue_ = oscillatorValue;
}

void Oscillator::run(const float *amplitudeIn, const float *modulatorIn,
                     float *audioOut, unsigned numFrames)
{
    internalRun<false>(amplitudeIn, modulatorIn, audioOut, numFrames);
}

void Oscillator::runAdding(const float *amplitudeIn, const float *modulatorIn,
                           float *audioOut, unsigned numFrames)
{
    internalRun<true>(amplitudeIn, modulatorIn, audioOut, numFrames);
}
}  // namespace TX81Z
