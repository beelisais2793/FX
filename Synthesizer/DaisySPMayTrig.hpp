#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct MayTrig : public FunctionProcessorPlugin<daisysp::MayTrig>
    {
        std::function<void (MayTrig * p)> callback = [](MayTrig * ptr){};
        MayTrig() : FunctionProcessorPlugin<daisysp::MayTrig>()
        {

        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            if( (bool)this->Process(I) == true ) callback(this);
            return A*I;
        }
    };
}