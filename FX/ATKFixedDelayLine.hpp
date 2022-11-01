#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Delays
{
    // Fixed Delay Line = this is variable...
    struct FDLDelay : public ATKFilter
    {
        ATK::FixedDelayLineFilter<DspFloatType> *filter;
        FDLDelay(size_t max_delay) :         
        ATKFilter()
        {
            filter = new ATK::FixedDelayLineFilter(max_delay);
            this->setFilter(filter);
        }
        ~FDLDelay() {
            if(filter) delete filter;
        }
        enum {
            PORT_DELAY            
        };
        void setPort(size_t port, DspFloatType value)  {
            switch(port)
            {
                case PORT_DELAY: filter->set_delay(value); break;
            }
        }
    };
}