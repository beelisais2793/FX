
#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    struct PitShift : public FunctionProcessorPlugin<stk::PitShift>
    {
        PitShift() : FunctionProcessorPlugin<stk::PitShift>()
        {

        }
        enum {
            PORT_CLEAR,
            PORT_SHIFT,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CLEAR: this->clear(); break;
                case PORT_SHIFT: this->setShift(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return this->tick(I);
        }
    };
}