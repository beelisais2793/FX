#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Filters
{

    struct MovingAvg : public FilterProcessorPlugin<gam::MovingAvg>
    {

        MovingAvg() : FilterProcessorPlugin<gam::MovingAvg>()
        {

        }        
        double Tick(double I, double A=1, double X=1, double Y=1) {
            return A*(*this)(I);
        }
    };
}