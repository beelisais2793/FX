#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct SineWave : public GeneratorProcessorPlugin<stk::SineWave>
    {
        SineWave() : GeneratorProcessorPlugin<stk::SineWave>()
        {

        }

        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
}