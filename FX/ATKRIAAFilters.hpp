#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct RIAAFilter: public ATKFilter
    {
        ATK::RIAACoefficients<DspFloatType> * filter;

        RIAAFilter() : ATKFilter()
        {
            filter = new ATK::RIAACoefficients<DspFloatType>(2);
        }
        ~RIAAFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_CUTOFF
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) filter->set_cut_frequency(v);
        }
    };
    struct InverseRIAAFilter: public ATKFilter
    {
        ATK::InverseRIAACoefficients<DspFloatType> * filter;

        InverseRIAAFilter() : ATKFilter()
        {
            filter = new ATK::InverseRIAACoefficients<DspFloatType>(2);
        }
        ~InverseRIAAFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_CUTOFF
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) filter->set_cut_frequency(v);
        }
    };
}    