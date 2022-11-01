#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Filters
{
    struct SVF : public FilterProcessorPlugin<daisysp::SVF>
    {
        enum {
            LP,
            HP,
            BP,
            NOTCH,
            PEAK,
        };
        int type = LP;

        SVF() :FilterProcessorPlugin<daisysp::SVF>()
        {
            this->Init(sampleRate);
        }
        enum {
            PORT_CUTOFF,
            PORT_RES,
            PORT_DRIVE,
            PORT_TYPE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: this->SetFreq(v); break;
                case PORT_RES: this->SetRes(v); break;
                case PORT_DRIVE: this->SetDrive(v); break;
                case PORT_TYPE: this->type = (int)v % (PEAK+1); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->Process(I);
        }
    };
}