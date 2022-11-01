#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct GainMaxCompressor : public ATKFilter
    {
        ATK::GainMaxCompressorFilter<DspFloatType> * filter;

        GainMaxCompressor()
        {
            filter = new ATK::GainMaxCompressorFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~GainMaxCompressor() {
            if(filter) delete filter;
        }
        enum {
            PORT_SOFTNESS,
            PORT_REDUCTION,
            PORT_REDUCTIONDB,            
        };
        void setPort(int port, DspFloatType value) {
            switch(port) {
                case PORT_SOFTNESS: filter->set_softness(port,value); break;
                case PORT_REDUCTION: filter->set_reduction(port,value); break;
                case PORT_REDUCTIONDB: filter->set_reduction_db(port,value); break;
            }
        }
    };
