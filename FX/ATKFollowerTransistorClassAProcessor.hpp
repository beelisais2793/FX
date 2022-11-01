#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Preamp
{
    
    struct FollowerTransistorClassA : public ATKFilter
    {
        ATK::FollowerTransistorClassAFilter<DspFloatType> * filter;

        FollowerTransistorClassA() : ATKFilter()  {
            filter = new ATK::FollowerTransistorClassAFilter<DspFloatType>(2);
        }
        ~FollowerTransistorClassA() {
            if(filter) delete filter;
        }
    };
