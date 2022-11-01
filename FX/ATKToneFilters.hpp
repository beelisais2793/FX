#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct SD1ToneFilter: public ATKFilter
    {
        ATK::SD1ToneCoefficients<DspFloatType> * filter;

        SD1ToneFilter() : ATKFilter()
        {
            filter = new ATK::SD1ToneCoefficients<DspFloatType>(2);            
        }
        ~SD1ToneFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_TONE
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_TONE) filter->set_tone(v);
        }
    };
    struct TS9ToneFilter: public ATKFilter
    {
        ATK::TS9ToneCoefficients<DspFloatType> * filter;

        TS9ToneFilter() : ATKFilter()
        {
            filter = new ATK::TS9ToneCoefficients<DspFloatType>(2);            
        }
        ~TS9ToneFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_TONE
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_TONE) filter->set_tone(v);
        }
    };
}