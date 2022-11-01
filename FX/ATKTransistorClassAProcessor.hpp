#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Preamp
{
    struct TransistorClassA : public ATKFilter
    {
        ATK::TransistorClassAFilter<DspFloatType> * filter;

        TransistorClassA() : ATKFilter() {
            filter = ATK::TransistorClassAFilter<DspFloatType>(2);
        }
        ~TransistorClassA() {

        }
    };
}