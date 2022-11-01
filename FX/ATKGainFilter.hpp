#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct GainFilter : public ATKFilter
    {
        ATK::GainFilter<DspFloatType> * filter;

        GainFilter()
        {
            filter = new ATK::GainFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~GainFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_THRESHOLD,
            PORT_THRESHOLDDB,
            PORT_RATIO,
        };
        void setPort(int port, DspFloatType value) {
            switch(port) {
                case PORT_THRESHOLD: filter->set_threshold(port,value); break;
                case PORT_THRESHOLDDB: filter->set_threshold_db(port,value); break;
                case PORT_RATIO: filter->set_ratio(port,value); break;
            }
        }
    };
}