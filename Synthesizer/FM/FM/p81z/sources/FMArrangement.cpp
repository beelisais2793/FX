#include "FMArrangement.h"

void FMArrangement::setup(float sampleRate)
{
    for (FMOperator &op : ops_)
        op.setup(sampleRate);
}

void FMArrangement::clear()
{
    for (FMOperator &op : ops_)
        op.clear();
}

void FMArrangement::adjustBuffer(unsigned size)
{
    for (FMOperator &op : ops_)
        op.adjustBuffer(size);

    buffer_.reset(new float[size]);
    zeros_.reset(new float[size]());
}

void FMArrangement::noteOn(int note, float velocity)
{
    for (FMOperator &op : ops_)
        op.noteOn(note, velocity);
}

void FMArrangement::noteOff()
{
    for (FMOperator &op : ops_)
        op.noteOff();
}

void FMArrangement::pitchBend(float bendSemitones)
{
    for (FMOperator &op : ops_)
        op.pitchBend(bendSemitones);
}

void FMArrangement::run(float *audioOut, unsigned numFrames)
{
    float *temp = buffer_.get();
    float *zeros = zeros_.get();

    FMOperator &op1 = ops_[0];
    FMOperator &op2 = ops_[1];
    FMOperator &op3 = ops_[2];
    FMOperator &op4 = ops_[3];

    switch (settings_->alg) {
    default:
    case 0:
        op4.run(zeros, temp, numFrames);
        op3.run(temp, temp, numFrames);
        op2.run(temp, temp, numFrames);
        op1.run(temp, audioOut, numFrames);
        break;
    case 1:
        op3.run(zeros, temp, numFrames);
        op4.runAdding(zeros, temp, numFrames);
        op2.run(temp, temp, numFrames);
        op1.run(temp, audioOut, numFrames);
        break;
    case 2:
        op3.run(zeros, temp, numFrames);
        op2.run(temp, temp, numFrames);
        op4.runAdding(zeros, temp, numFrames);
        op1.run(temp, audioOut, numFrames);
        break;
    case 3:
        op4.run(zeros, temp, numFrames);
        op3.run(temp, temp, numFrames);
        op2.runAdding(temp, temp, numFrames);
        op1.run(temp, audioOut, numFrames);
        break;
    case 4:
        op2.run(zeros, temp, numFrames);
        op1.run(temp, audioOut, numFrames);
        op4.run(zeros, temp, numFrames);
        op3.runAdding(temp, audioOut, numFrames);
        break;
    case 5:
        op4.run(zeros, temp, numFrames);
        op1.run(temp, audioOut, numFrames);
        op2.runAdding(temp, audioOut, numFrames);
        op3.runAdding(temp, audioOut, numFrames);
        break;
    case 6:
        op1.run(zeros, audioOut, numFrames);
        op2.runAdding(zeros, audioOut, numFrames);
        op4.run(zeros, temp, numFrames);
        op3.runAdding(temp, audioOut, numFrames);
        break;
    case 7:
        op1.run(zeros, audioOut, numFrames);
        op2.runAdding(zeros, audioOut, numFrames);
        op3.runAdding(zeros, audioOut, numFrames);
        op4.runAdding(zeros, audioOut, numFrames);
        break;
    }
}

bool FMArrangement::hasReleased() const
{
    const FMOperator &op1 = ops_[0];
    return op1.hasReleased();
}

bool FMArrangement::hasFinished() const
{
    const FMOperator &op1 = ops_[0];
    const FMOperator &op2 = ops_[1];
    const FMOperator &op3 = ops_[2];
    const FMOperator &op4 = ops_[3];

    switch (settings_->alg) {
    default:
    case 0:
    case 1:
    case 2:
    case 3:
        return op1.hasFinished();
    case 4:
        return op1.hasFinished() && op3.hasFinished();
    case 5:
    case 6:
        return op1.hasFinished() && op2.hasFinished() && op3.hasFinished();
    case 7:
        return op1.hasFinished() && op2.hasFinished() && op3.hasFinished() && op4.hasFinished();
    }
}

void FMArrangement::useSettings(const Settings *s)
{
    settings_ = s;
    for (unsigned i = 0; i < 4; ++i)
        ops_[i].useSettings(s ? &s->op[i] : nullptr);
}
