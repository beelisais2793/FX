#pragma once

#include "DaisySP.hpp"

namespace DaisySP::PhysicalModels
{
    struct Resonator : public FilterProcessorPlugin<daisysp::ResonatorSvf>
    {
        enum {
            LP,
            BP,
            BPN,
            HP
        };
        int type = LP;
        double f,q,g;

        Resonator() : FilterProcessorPlugin<daisysp::Resonator>()
        {
            this->Init();
            f = 1000.0;
            q = 0.5;
            g = 1.0;
        }
        enum {
            PORT_TYPE,
            PORT_FREQ,
            PORT_Q,
            PORT_GAIN
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_TYPE: type = v; break;
                case PORT_FREQ: f = v; break;
                case PORT_Q: q = v; break;
                case PORT_G: g = v; break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            float out = I;
            switch(type)
            {
                case  HP:  this->Process<FilterMode::HIGH_PASS,true>(&f,&q,&g,I,&out); break;
                case  BP:  this->Process<FilterMode::BAND_PASS,true>(&f,&q,&g,I,&out); break;
                case  BPN: this->Process<FilterMode::BAND_PASS_NORMALIZED,true>(&f,&q,&g,I,&out); break;
                default:   this->Process<FilterMode::LOW_PASS,true>(&f,&q,&g,I,&out); break;
            }
            return A*out;
        }
    };
}