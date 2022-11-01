#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct Noise : public GeneratorProcessorPlugin<stk::Noise>
    {
        Noise() : GeneratorProcessorPlugin<stk::Noise>()
        {

        }
        enum {
            PORT_SEED,
            PORT_LASTOUT,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_SEED: this->setSeed(v); break;
                default: printf("No port %d\n",port);
            }
        }
        double getPort(int port) {
            if(port == PORT_LASTOUT) return this->lastOut();
            printf("No port %d\n",port);
            return 0.0;
        }
        double Tick(double I=0, double A=1, double X=1, double Y=1)
        {
            return this->tick();
        }
    };
}