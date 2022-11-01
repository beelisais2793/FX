#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Delays
{
    ATK::FeedbackDelayNetworkFilter<DspFloatType> * filter;
    // Feedback Delay Network
    struct FDNDelay : public ATKFilter
    {
        FDNDelay(size_t max_delay) :         
        ATKFilter()
        {
            filter = new ATK::FeedbackDelayNetworkFilter(max_delay);
            this->setFilter(filter);
        }
        ~FDNDelay() {
            if(filter) delete filter;
        }
        enum {
            PORT_DELAY,
            PORT_INGAIN,
            PORT_FEEDBACK,
            PORT_OUTGAIN,
        };
        void setPort(size_t port, DspFloatType value)  {
            switch(port)
            {
                case PORT_DELAY: filter->set_delay(value); break;
                case PORT_INGAIN: filter->set_ingain(value); break;
                case PORT_FEEDBACK: filter->set_feedback(value); break;
                case PORT_OUTGAIN: filter->set_outgain(value); break;

            }
        }
    };
}