#pragma once
#include "ClipFunctions.hpp"

namespace FX::Distortion::Clip
{
    struct SerpentCurve : public AmplifierProcessor
    {
        double gain = 1;
        SerpentCurve(double g = 1.0) : AmplifierProcessor(), 
        {
            gain = pow(10.0,g/20.0);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*serpent_curve(I,gain);
        }
    };
}