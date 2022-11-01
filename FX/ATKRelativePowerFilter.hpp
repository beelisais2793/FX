#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct RelativePowerFilter : public ATKFilter
    {
        ATK::RelativePowerFilter<DspFloatType> * filter;

        RelativePowerFilter()
        {
            filter = new ATK::PowerFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~RelativePowerFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_MEMORY,
        };
        void setPort(int port, DspFloatType value) {
            switch(port) {
                case PORT_MEMORY: filter->set_memory(port,value); break;
            }
        }

    };
}