#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct GainLimiter : public ATKFilter
    {
        ATK::GainLimterFilter<DspFloatType> * filter;

        GainLimiter()
        {
            filter = new ATK::GainLimiterFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~GainLimiter() {
            if(filter) delete filter;
        }
        enum {
            PORT_SOFTNESS,         
        };
        void setPort(int port, DspFloatType value) {
            switch(port) {
                case PORT_SOFTNESS: filter->set_softness(port,value); break;
            }
        }
    };
}