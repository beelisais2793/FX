#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct Modulate : public GeneratorProcessorPlugin<stk::Modulate>
    {
        Modulate() : GeneratorProcessorPlugin<stk::Modulate>()
        {

        }
        enum {
            PORT_RESET,
            PORT_VIBRATORATE,
            PORT_VIBRATOGAIN,
            PORT_RANDOMRATE,
            PORT_RANDOMGAIN,
            PORT_LASTOUT,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_RESET: this->reset(); break;
                case PORT_VIBRATORATE: this->seVibratoRate(v); break;
                case PORT_VIBRATOGAIN: this->seVibratoGain(v); break;
                case PORT_RANDOMRATE: this->setRandomRate(v); break;
                case PORT_RANDOMGAIN: this->setRandomGain(v); break;                
            }
        }
        double getPort(int port) {
            if(port == PORT_LASTOUT) return this->lastOut();
            return 0.0;
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
}