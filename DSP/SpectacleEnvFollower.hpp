#pragma once

#include "Spectacle.hpp"

namespace DSP::Spectacle
{
    class ARFollower {
    private:
        float fConst0;
        float fRec1[2];
        float fRec0[2];
        float fControl[2];

    public:
        void init(float sample_rate)
        {
            fConst0 = (1.0f / sample_rate);
            clear();
            setAttackTime(0.1);
            setReleaseTime(0.1);
        }

        void clear()
        {
            for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
                fRec1[l0] = 0.0f;
            }
            for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
                fRec0[l1] = 0.0f;
            }
        }

        void setAttackTime(float at)
        {
            fControl[0] = std::exp((0.0f - (fConst0 / float(at))));
        }

        void setReleaseTime(float rt)
        {
            fControl[1] = std::exp((0.0f - (fConst0 / float(rt))));
        }

        void configureLike(const ARFollower &other) {
            fConst0 = other.fConst0;
            fControl[0] = other.fControl[0];
            fControl[1] = other.fControl[1];
        }

        float compute(float x)
        {
            float fTemp0 = float(x);
            float fTemp1 = ((fRec0[1] > fTemp0) ? fControl[1] : fControl[0]);
            fRec1[0] = ((fRec1[1] * fTemp1) + (fTemp0 * (1.0f - fTemp1)));
            fRec0[0] = fRec1[0];
            float y = float(fRec0[0]);
            fRec1[1] = fRec1[0];
            fRec0[1] = fRec0[0];
            return y;
        }
    };
}