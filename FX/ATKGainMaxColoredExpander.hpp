#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct GainMaxColoredExpander : public ATKFilter
    {
        ATK::GainMaxColoredExpanderFilter * filter;

        GainMaxColoredExpander()
        {
            filter = new ATK::GainMaxColoredExpanderFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~GainMaxColoredExpander() {
            if(filter) delete filter;
        }
        enum {
            PORT_SOFTNESS,
            PORT_COLOR,
            PORT_QUALITY
            PORT_REDUCTION,
            PORT_REDUCTIONDB,            
        };
        void setPort(int port, DspFloatType value) {
            switch(port) {
                case PORT_SOFTNESS: filter->set_softness(port,value); break;
                case PORT_COLOR: filter->set_color(port,value); break;
                case PORT_QUALITY: filter->set_quality(port,value); break;
                case PORT_REDUCTION: filter->set_reduction(port,value); break;
                case PORT_REDUCTIONDB: filter->set_reduction_db(port,value); break;             
            }
        }
    };
}