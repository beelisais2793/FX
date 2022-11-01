#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Analysis
{
    struct ZeroCrossRate : public FunctionProcessorPlugin<gam::ZeroCrossRate<double>>
    {
        ZeroCrossRate(int winsize=256)
        : FunctionProcessorPlugin<gam::ZeroCrossRate<double>>,
            gam::ZeroCrossRate<double>(winsize)
        {

        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {                        
            double r = (*this)(I);
            return r;
        }
    };
}