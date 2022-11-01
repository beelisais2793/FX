#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Analysis
{
    // ports
    // period n
    // period
    // cycled
    // count
    // reset

    // X > 0  period(X)
    // Y >=1  reset()

    struct PCounter : public GeneratorPcessorPlugin<gam::PCounter>
    {
        std::function<void (PCounter &)> callback = [](PCounter &) {}

        PCounter(unsigned period = 256)
        : GeneratorProcessorPlugin<gam::PCounter>,
            gam::PCounter(period)
        {

        }
        enum {
            PORT_PERIOD,
            PORT_CYCLED,
            PORT_COUNT,
            PORT_RESET,
        };
        void setPort(int port, double v) {
            if(port == PORT_RESET && v == 1.0) this->reset();
        }
        double getPort(int port) {
            switch(port) {
                case PORT_PERIOD: return this->period();
                case PORT_CYCLED: return this->cycled();
                case PORT_COUNT: return this->count();                
            }
            return 0.0;
        }
        double Tick(double I=1, double A=1, double X=0, double Y=0)
        {           
            return (*this)();
        }
    };
}