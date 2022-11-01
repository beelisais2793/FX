#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Analysis
{
    // ports
    // none

    // A = amp
    // X = none
    // Y = none
    struct ZeroCross : public FunctionProcessorPlugin<gam::ZeroCross<double>>
    {
        ZeroCross(double prev = 0.0)
        : FunctionProcessorPlugin<gam::Threshold>,
            gam::ZeroCross<double>>(prev)
        {

        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {                    
            return (*this)(I);            
        }
    };
}