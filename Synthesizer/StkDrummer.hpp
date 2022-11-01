#pragma once
#include "StkHeaders.hpp"

namespace Stk::Drummer
{
     // todo: ports
    struct Drummer : public GeneratorProcessorPlugin<stk::Drummer>
    {
        Drummer() : GeneratorProcessorPlugin<stk::Drummer>()
        {

        }

        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
}