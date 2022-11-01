#pragma once

#include "DaisySP.hpp"

namespace DaisySP::FX
{
    struct Overdrive : public MonoFXProcessorPlugin<daisyp::Overdrive>
    {
        Overdrive() : MonoFXProcessorPlugin<daisyp::Overdrive>()
        {
            this->Init();
        }
        enum {
            PORT_DRIVE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_DRIVE: this->SetDrive(v); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*this->Process(I);
        }
        void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}