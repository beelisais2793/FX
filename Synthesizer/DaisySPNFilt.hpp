#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Filters
{
    // how to calculate the coefficients?
    struct NFilt : public FilterProcessorPlugin<daisysp::NFilt>
    {
        NFilt() : FilterProcessorPlugin<daisysp::NFilt>()
        {
            this->Init();
        }

    };
}