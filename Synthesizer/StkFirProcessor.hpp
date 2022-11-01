#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct Fir : public FilterProcessorPlugin<stk::Fir>
    {
        Fir(std::vector<float> &coeffs) : FilterProcessorPlugin<stk::Fir>()
        {
            this->setCoefficients(coeffs,true);
        }

        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->tick(I);
        }
    };
}