#pragma once

#include "StkHeaders.hpp"

namespace Stk 
{
    // todo: ports
    struct Iir : public FilterProcessorPlugin<stk::Iir>
    {
        Iir(std::vector<StkFloat> & a, std::vector<StkFloat> & b) : FilterProcessorPlugin<stk::Iir>()
        {
            this->setCoefficients(a,b,true);
        }        
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            return A*this->tick(I);
        }
    };
}