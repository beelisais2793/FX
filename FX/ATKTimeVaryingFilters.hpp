#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct TimeVaryingBandPassFilter: public ATKFilter
    {   
        ATK::TimeVaryingBandPassCoefficients<DspFloatType> * filter;

        TimeVaryingBandPassFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingBandPassCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingBandPassFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingLowPassFilter: public ATKFilter
    {   
        ATK::TimeVaryingLowPassCoefficients<DspFloatType> * filter;

        TimeVaryingLowPassFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingLowPassCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingLowPassFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingHighPassFilter: public ATKFilter
    {   
        ATK::TimeVaryingHighPassCoefficients<DspFloatType> * filter;

        TimeVaryingHighPassFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingHighPassCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingHighPassFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingBandPassPeakFilter: public ATKFilter
    {   
        ATK::TimeVaryingBandPassPeakCoefficients<DspFloatType> * filter;

        TimeVaryingBandPassPeakFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingBandPassPeakCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingBandPassPeakFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
                case PORT_GAIN: filter->set_gain(v); break;
            }
        }
    };
    struct TimeVaryingAllPassFilter: public ATKFilter
    {   
        ATK::TimeVaryingAllPassCoefficients<DspFloatType> * filter;

        TimeVaryingAllPassFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingAllPassCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingAllPassFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingLowShelfFilter: public ATKFilter
    {   
        ATK::TimeVaryingLowShelfCoefficients<DspFloatType> * filter;

        TimeVaryingLowShelfFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingLowShelfCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingLowShelfFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
                case PORT_GAIN: filter->set_gain(v); break;
            }
        }
    };
    struct TimeVaryingHighShelfFilter: public ATKFilter
    {   
        ATK::TimeVaryingHighShelfCoefficients<DspFloatType> * filter;

        TimeVaryingHighShelfFilter(): ATKFilter()
        {
            filter = new ATK::TimeVaryingHighShelfCoefficients<DspFloatType>(2);
        }
        ~TimeVaryingHighShelfFilter() {
            if(filter) delete filter
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
                case PORT_GAIN: filter->set_gain(v); break;
            }
        }
    };
}