#pragma once
#include "../Effect.hpp"

#define M_PI 3.14159265359
#include <cmath>

class Filter : public Effect
{
protected:
    Sample x[3];
    Sample y[3];

    double cutoff = 1000;

public:
    Filter& Cutoff(double);
};