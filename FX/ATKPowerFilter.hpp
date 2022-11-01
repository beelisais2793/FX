#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct PowerFilter : public ATKFilter
    {
        ATK::PowerFilter<DspFloatType> * filter;

        PowerFilter()
        {
            filter = new ATK::PowerFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~PowerFilter() {
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