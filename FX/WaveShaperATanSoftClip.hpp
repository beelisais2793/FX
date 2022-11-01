#pragma once
#include "ClippingFunctions.hpp"

namespace FX::WaveShaping
{
    struct ATanSoftClipper : public AmplifierProcessor
    {
        double gain = 1.0;
        ATanSoftClipper(double g = 1.0) : AmplifierProcessor()
        {
            gain = pow(10.0,g/20.0);
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*atanSoftClippin(gain*I);
        }
    };
}