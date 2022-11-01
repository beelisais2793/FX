#pragma once
#include "../Generator.hpp"

struct Envelope : public Generator
{
    virtual void Trigger() = 0;
    virtual void Gate(bool) = 0;

    virtual Sample Apply(Sample) = 0;
};

Sample operator>>(Sample b, Envelope& a);