#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct Simple : public GeneratorProcessorPlugin<stk::Simple>
    {
        Simple() : GeneratorProcessorPlugin<stk::Simple>()
        {

        }
        enum {
            PORT_FREQ,
            PORT_KEYON,
            PORT_KEYOFF,
            PORT_NOTEON,
            PORT_NOTEOFF,
            PORT_CC,
        };
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}