#pragma once
#include "Component.hpp"

struct Effect : public Component
{
    virtual Sample Apply(Sample) = 0;
};

struct StereoEffect
{
    Effect& left;
    Effect& right;

    StereoEffect(Effect& left, Effect& right) : left(left), right(right) {};
};


Sample operator>>(Sample b, Effect& a);


Stereo operator>>(Sample b, const StereoEffect& a);

Stereo operator>>(Stereo b, const StereoEffect& a);

