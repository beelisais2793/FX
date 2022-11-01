#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Preamp
{
     struct Triode2 : public ATKFilter
    {
        ATK::Triode2Filter<DspFloatType> * filter;

        Triode2() : ATKFilter() {
            filter = new ATK::Triode2Filter<DspFloatType>(2);
        }
        ~Triode2() {
            if(filter) delete filter;
        }
    };
}   
 