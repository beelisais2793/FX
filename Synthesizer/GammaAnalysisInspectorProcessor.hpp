#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Analysis
{
    // call back everysample 
    // pass through I
    
    // ports
    // status
    // peak
    // DC
    // maxDC
    // reset
    // print

    struct Inspector : public FunctionProcessorPlugin<gam::Inspector<double>>
    {
        std::function<void (Inspector &, unsigned)> callback = [](Inspector &, unsigned) {}

        Inspector(int winsize=256)
        : FunctionProcessorPlugin<gam::Inspector<double>>,
            gam::Inspector<double>(winsize)
        {

        }
        double Tick(double I, double A=1, double X=0, double Y=0)
        {                        
            unsigned r = (*this)(I);
            if(r != 0) callback(*this,r);
            return I;
        }
    };
};