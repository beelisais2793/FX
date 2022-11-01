#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Preamp
{
        struct Triode : public ATKFilter
    {
        ATK::TriodeFilter<DspFloatType> * filter;

        Triode() : ATKFilter() {
            filter = new ATK::TriodeFilter<DspFloatType>(2);
        }
        ~Triode() {
            if(filter) delete filter;
        }
    };
}
