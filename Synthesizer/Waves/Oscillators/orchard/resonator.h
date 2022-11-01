#pragma once

#include "Filters/svf.h"
#include "Utility/delayline.h"
#include "Utility/dsp.h"

using namespace daisysp;

#define MAX_DELAY static_cast<size_t>(48000 * 1.f)

namespace orchard
{
    constexpr int kMaxPoles{5};

    struct Pole
    {
        DelayLine<float, MAX_DELAY> *leftDel;
        DelayLine<float, MAX_DELAY> *rightDel;
        float currentLeftDelay;
        float currentRightDelay;
        float leftDelayTarget;
        float rightDelayTarget;

        Svf filt;

        float sampleRate_{0.f};
        float damp_{0.f};
        float decay_{0.f};
        float detune_{0.f};
        float basePitch_{60.f};
        float pitch_{0.f};

        void Init(float sampleRate, DelayLine<float, MAX_DELAY> *lDel, DelayLine<float, MAX_DELAY> *rDel)
        {
            leftDel = lDel;
            rightDel = rDel;
            sampleRate_ = sampleRate;
            filt.Init(sampleRate_);
            filt.SetDrive(0.1f);
        }

        void SetFrequency()
        {
            filt.SetFreq(fclamp(mtof(pitch_) + damp_, 0.f, sampleRate_ / 3.f));
        }

        void SetDelayTimes()
        {
            leftDelayTarget = pow10f((pitch_ - detune_) / 20.0f);  // ms
            leftDelayTarget *= sampleRate_ * 0.001f;              // ms to samples ?
            rightDelayTarget = pow10f((pitch_ + detune_) / 20.0f); // ms
            rightDelayTarget *= sampleRate_ * 0.001f;             // ms to samples ?
        }

        void SetDetune(float detune)
        {
            detune_ = detune;
            SetDelayTimes();
            SetFrequency();
        }

        void SetDamp(float damp)
        {
            damp_ = damp;
            SetFrequency();
        }

        void SetPitch(float pitch)
        {
            basePitch_ = pitch;
            pitch_ = basePitch_;
            pitch_ *= -0.5017f;
            pitch_ += 17.667f;
            SetDelayTimes();
            SetFrequency();
        }

        float ProcessLeft(float in)
        {
            fonepole(currentLeftDelay, leftDelayTarget, .0002f);
            leftDel->SetDelay(currentLeftDelay);

            float leftW = leftDel->Read();
            filt.Process(leftW);
            leftW = filt.Low();
            leftDel->Write((decay_ * leftW) + in);

            return leftW;
        }

        float ProcessRight(float in)
        {
            fonepole(currentRightDelay, rightDelayTarget, .0002f);
            rightDel->SetDelay(currentRightDelay);

            float rightW = rightDel->Read();
            filt.Process(rightW);
            rightW = filt.Low();
            rightDel->Write((decay_ * rightW) + in);

            return rightW;
        }
    };

    class Resonator
    {

    public:
        Resonator() {}
        ~Resonator() {}

        void Init(float sampleRate)
        {
            sampleRate_ = sampleRate;
        }
        void AddPole(DelayLine<float, MAX_DELAY> *left, DelayLine<float, MAX_DELAY> *right)
        {
            poles_[nPoles_].Init(sampleRate_, left, right);
            nPoles_++;
        }
        void SetDamp(float damp)
        {
            for (int i = 0; i < nPoles_; i++)
            {
                poles_[i].SetDamp(damp);
            }
        }
        void SetReso(float reso)
        {
            for (int i = 0; i < nPoles_; i++)
            {
                poles_[i].filt.SetRes(reso);
            }
        }
        void SetDecay(float decay)
        {
            for (int i = 0; i < nPoles_; i++)
            {
                poles_[i].decay_ = decay;
            }
        }
        void SetDetune(float detune)
        {
            for (int i = 0; i < nPoles_; i++)
            {
                poles_[i].SetDetune(detune);
            }
        }
        void SetPitch(int pole, float pitch)
        {
            poles_[pole].SetPitch(pitch);
        }

        void Process(float &left, float &right)
        {
            float leftW{0.f};
            float rightW{0.f};
            for (int i = 0; i < nPoles_; i++)
            {
                leftW += poles_[i].ProcessLeft(left) * (1.f / nPoles_);
                rightW += poles_[i].ProcessRight(right) * (1.f / nPoles_);
            }
            left = leftW;
            right = rightW;
        }

    private:
        Pole poles_[kMaxPoles];
        float sampleRate_;
        float damp_{0.f};   // 0.0 and sample_rate / 3
        float reso_{0.f};   // 0.0 : 0.4
        float decay_{0.f};  // 0.0 : ?
        float detune_{0.f}; // 0.0 : 0.07
        float pitches_[kMaxPoles];
        int nPoles_{0};
    };
}