#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{

    struct ToneStackFilter: public ATKFilter
    {
        ATK::ToneStackCoefficients<DspFloatType> * filter;

        ToneStackFilter() : ATKFilter() {
            filter = new ATK::ToneStackCoefficients<DspFloatType>(2);
        }
        ~ToneStackFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_LOW,
            PORT_MID,
            PORT_HIGH
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_LOW: filter->set_low(v); break;
                case PORT_MID: filter->set_middle(v); break;
                case PORT_HIGH: filter->set_high(v); break;

            }
        }
    };
}
    