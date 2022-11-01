#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct SingWave : public GeneratorProcessorPlugin<stk::SingWave>
    {
        SingWave() : GeneratorProcessorPlugin<stk::SingWave>()
        {

        }

        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };

}