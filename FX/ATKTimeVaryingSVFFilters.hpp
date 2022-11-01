#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct TimeVaryingSecondOrderSVFLowPassFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFLowPassCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFLowPassFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFLowPassFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFLowPassFilter() {
            delete filter;
        }
        enum  {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingSecondOrderSVFHighPassFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFHighPassCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFHighPassFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFHighPassFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFHighPassFilter() {
            delete filter;
        }
        enum  {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingSecondOrderSVFBandPassFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFBandPassCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFBandPassFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFBandPassFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFBandPassFilter() {
            delete filter;
        }
        enum  {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingSecondOrderSVFNotchPassFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFNotchPassCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFNotchPassFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFNotchPassFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFNotchPassFilter() {
            delete filter;
        }
        enum  {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingSecondOrderSVFPeakFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFPeakCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFPeakFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFPeakFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFPeakFilter() {
            delete filter;
        }
        enum  {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_Q: filter->set_Q(v); break;
            }
        }
    };
    struct TimeVaryingSecondOrderSVFBellFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFBellCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFBellFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFBellFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFBellFilter() {
            delete filter;
        }
        enum  {
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
    struct TimeVaryingSecondOrderSVFLowShelfFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFLowShelfCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFLowShelfFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFLowShelfFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFLowShelfFilter() {
            delete filter;
        }
        enum  {
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
    struct TimeVaryingSecondOrderSVFHighShelfFilter: public ATKFilter
    {
        ATK::TimeVaryingSecondOrderSVFHighShelfCoefficients<DspFloatType> * filter;

        TimeVaryingSecondOrderSVFHighShelfFilter() : ATKFilter()
        {
            filter = new ATK::TimeVaryingSecondOrderSVFHighShelfFilter<DspFloatType>(2);
        }
        ~TimeVaryingSecondOrderSVFHighShelfFilter() {
            delete filter;
        }
        enum  {
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