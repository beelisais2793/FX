#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct LinkwitzRileyLowPassFilter: public ATKFilter
    {
        ATK::LinwitzRileyLowPassCoefficients<DspFloatType> * filter;

        LinkwitzRileyLowPassFilter() : ATKFilter() {
            filter = new ATK::LinwitzRileyLowPassCoefficients<DspFloatType>(2);
        }
        ~LinkwitzRileyLowPassFilter() {
            delete filter;
        }
        enum {
            PORT_CUTOFF
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) filter->set_cut_frequency(v);
        }
    };
    struct LinkwitzRileyHighPassFilter: public ATKFilter
    {
        ATK::LinwitzRileyHighPassCoefficients<DspFloatType> * filter;

        LinwitzRileyHighPassFilter() : ATKFilter() {
            filter = new ATK::LinwitzRileyHighPassCoefficients<DspFloatType>(2);
        }
        ~LinkwitzRileyHighPassFilter() {
            delete filter;
        }
        enum {
            PORT_CUTOFF
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) filter->set_cut_frequency(v);
        }
    };
    struct LinkwitzRiley4LowPassFilter: public ATKFilter
    {
        ATK::LinwitzRiley4LowPassCoefficients<DspFloatType> * filter;

        LinkwitzRiley4LowPassFilter() : ATKFilter() {
            filter = new ATK::LinwitzRiley4LowPassCoefficients<DspFloatType>(2);
        }
        ~LinkwitzRiley4LowPassFilter() {
            delete filter;
        }
        enum {
            PORT_CUTOFF
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) filter->set_cut_frequency(v);
        }
    };
    struct LinkwitzRiley4HighPassFilter: public ATKFilter
    {
        ATK::LinwitzRiley4HighPassCoefficients<DspFloatType> * filter;

        LinwitzRiley4HighPassFilter() : ATKFilter() {
            filter = new ATK::LinwitzRiley4HighPassCoefficients<DspFloatType>(2);
        }
        ~LinkwitzRiley4HighPassFilter() {
            delete filter;
        }
        enum {
            PORT_CUTOFF
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) filter->set_cut_frequency(v);
        }
    };
}