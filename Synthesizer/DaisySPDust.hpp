#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Noise
{
    struct Dust : public GeneratorProcessorPlugin<daisysp::Dust>
    {
        Dust() : GeneratorProcessorPlugin<daisysp::Dust>()
        {

        }
        enum {
            PORT_DENSITY,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DENSITY: this->SetDensity(v); break;
            }
        }
        double Tick(double I=1, double A=1, double X=1, double Y=0) {
            return A*this->Process();
        }       
    };  
}