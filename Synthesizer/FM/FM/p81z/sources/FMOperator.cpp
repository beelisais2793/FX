#include "FMOperator.h"

void FMOperator::setup(float sampleRate)
{
    eg_.setup(sampleRate);
    osc_.setup(sampleRate);
}

void FMOperator::clear()
{
    eg_.clear();
    osc_.clear();
}

void FMOperator::adjustBuffer(unsigned size)
{
    buffer_.reset(new float[size]);
}

void FMOperator::noteOn(int note, float velocity)
{
    eg_.noteOn(note, velocity);
    osc_.noteOn(note);
}

void FMOperator::noteOff()
{
    eg_.noteOff();
    osc_.noteOff();
}

void FMOperator::pitchBend(float bendSemitones)
{
    osc_.pitchBend(bendSemitones);
}

void FMOperator::run(const float *audioIn, float *audioOut, unsigned numFrames)
{
    float *egTemp = buffer_.get();
    eg_.run(egTemp, numFrames);
    osc_.run(egTemp, audioIn, audioOut, numFrames);
}

void FMOperator::runAdding(const float *audioIn, float *audioOut, unsigned numFrames)
{
    float *egTemp = buffer_.get();
    eg_.run(egTemp, numFrames);
    osc_.runAdding(egTemp, audioIn, audioOut, numFrames);
}

void FMOperator::useSettings(const Settings *s)
{
    settings_ = s;
    eg_.useSettings(s ? &s->eg : nullptr);
    osc_.useSettings(s ? &s->osc : nullptr);
}
