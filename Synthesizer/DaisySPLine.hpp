#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Control
{
    struct Line : public GeneratorProcessorPlugin<daisyp::Line>
    {
        uint8_t finished = 0;
        std::function<void (Line *)> callback = [](Line * p){};
        Line() : GeneratorProcessorPlugin<daisyp::Line>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_RESET,
        };
        void setPort(int port, double v) {
            if(port == PORT_RESET) finished = 0;
        }
        double Tick(double I=1, double A=1, double X=1, double Y=1) {
            double out = A*this->Process(&finished);
            if(finished == 1) callback(this);
            return out;
        }
    };
}