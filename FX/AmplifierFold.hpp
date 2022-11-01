#pragma once
#include "Amplifiers.hpp"

namespace FX::Distortion
{
    struct Fold : public AmplifierProcessor
    {
        Fold() : AmplifierProcessor()
        {

        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return A*Fold(I);
        }
    };

       
}