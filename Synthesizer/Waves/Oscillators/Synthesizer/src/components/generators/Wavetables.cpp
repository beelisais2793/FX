#define _USE_MATH_DEFINES
#include "Wavetables.hpp"
#include <cmath>

double Wavetables::Square::Value(double phase)
{
    return phase > 0.5 ? -1 : 1;
}

double Wavetables::Sine::Value(double phase)
{
    return std::sin(phase * M_PI * 2);
}

double Wavetables::Saw::Value(double phase)
{
    return 2 * (-phase + 0.5);
}

double Wavetables::Triangle::Value(double phase)
{
    return 4 * std::abs(0.5 - phase) - 1;
}

double Wavetables::Basic::Value(double phase)
{
    double p = std::fmod(phase * sync, 1);
    if (position < 0.33) 
    {
        double r = position * 3;
        return triangle[p] * r + sine[p] * (1 - r);
    }
    else if (position < 0.66) 
    {
        double r = (position - 0.33) * 3;
        return saw[p] * r + triangle[p] * (1 - r);
    }
    else
    {
        double r = (position - 0.66) * 3;
        return square[p] * r + saw[p] * (1 - r);
    }
}

void Wavetables::Basic::Position(double p)
{
    position = p;
}

void Wavetables::Basic::Sync(double p)
{
    sync = p;
}
