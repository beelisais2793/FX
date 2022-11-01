#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Delays
{
    // Universal Variable Delay Line
    struct UVDLDelay : public ATKFilter
    {
        ATK::UniversalVariableDelayLineFilter<DspFloatType> *filter;
        UVDLDelay(size_t max_delay) :         
        ATKFilter()
        {
            filter = new ATK::UniversalVariableDelayLineFilter(max_delay);
            this->setFilter(filter);
        }
        ~UVDLDelay() {
            if(filter) delete filter;
        }
        enum {
            PORT_DELAY,
            PORT_BLEND,
            PORT_FEEDBACK,
            PORT_FEEDFORWARD,                 
        };
        void setPort(size_t port, DspFloatType value)  {
            switch(port)
            {
                case PORT_DELAY: filter->set_delay(value); break;
                case PORT_BLEND: filter->set_blend(value); break;
                case PORT_FEEDBACK: filter->set_feedback(value); break;
                case PORT_FEEDFORWARD: filter->set_feedforward(value); break;
            }
        }
    };
}