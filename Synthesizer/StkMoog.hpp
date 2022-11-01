#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct Moog : public GeneratorProcessorPlugin<stk::Moog>
    {
        Moog() : GeneratorProcessorPlugin<stk::Moog>()
        {

        }
        enum {
            PORT_FREQ,
            PORT_NOTEON,
            PORT_MODSPEED,
            PORT_MODDEPTH,
            PORT_CC,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_FREQ: this->setFrequency(v); break;
                case PORT_MODSPEED: this->setModulationSpeed(v); break;
                case PORT_MODDEPTH: this->setModulationDepth(v); break;
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_NOTEON: this->setNoteOn(a,b); break;
                case PORT_CC: this->setControlChange(a,b); break;
            }
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
}