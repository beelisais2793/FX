#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct GainExpander : public ATKFilter
    {
        ATK::GainExpanderFilter<DspFloatType> * filter;

        GainExpander()
        {
            filter = new ATK::GainExpanderFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~GainExpander() {
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