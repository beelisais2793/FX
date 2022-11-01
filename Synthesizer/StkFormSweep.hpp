#pragma once
#include "StkHeaders.hpp"
namespace Stk::Generators
{
    struct FormSwep : public GeneratorProcessorPlugin<stk::FormSwep>
    {
        FormSwep() : GeneratorProcessorPlugin<stk::FormSwep>()
        {

        }
        enum {
            PORT_RESONANCE,
            PORT_STATES,
            PORT_TARGETS,
            PORT_SWEEPRATE,
            PORT_SWEEPTIME,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_RESONANCE: this->setResonance(v); break;
                case PORT_SWEEPRATE: this->setSweepRate(v); break;
                case PORT_SWEEPTIME: this->setSweepTime(v); break;
            }
        }
        void setPortV(int port, const std::vector<double> & v) {
            switch(port) {
                case PORT_STATE: this->setStates(v[0],v[1],v[2]); break;
                case PORT_TARGETS: this->setTargets(v[0],v[1],v[2]); break;
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->tick(I);
        }
    };
}