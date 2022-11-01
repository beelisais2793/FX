#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Gamma::Analysis
{
    // ports
    // inv

    // A = amp
    // X = hi
    // Y = lo

    struct Threshold : public FunctionProcessorPlugin<gam::Threshold<double>>
    {        
        Threshold(double thresh, double f = 10.0) 
        : FunctionProcessorPlugin<gam::Threshold<double>>,
            gam::Threshold<double>>(thresh,f)
        {

        }
        
        double Tick(double I, double A=1, double X=1.0, double Y=-1.0)
        {                        
            double x = (*this)(I);
            if(x == 0) return -A*I;
            return A*I;
        }
    };

    struct ModThreshold : public FunctionProcessorPlugin<gam::Threshold<double>>
    {        
        ModThreshold(double thresh, double f = 10.0) 
        : FunctionProcessorPlugin<gam::Threshold<double>>,
            gam::Threshold<double>>(thresh,f)
        {

        }        
        double Tick(double I, double A=1, double X=1.0, double Y=-1.0)
        {                        
            double x = (*this)(I,X,Y);            
            return A*x;
        }
    };

    struct InvThreshold : public FunctionProcessorPlugin<gam::Threshold<double>>
    {        
        Threshold(double thresh, double f = 10.0) 
        : FunctionProcessorPlugin<gam::Threshold<double>>,
            gam::Threshold<double>>(thresh,f)
        {

        }
        
        double Tick(double I, double A=1, double X=1.0, double Y=-1.0)
        {                        
            double x = this->inv(I);            
            if(x == 1) return -A*I;
            return A*I;
        }
    };
}