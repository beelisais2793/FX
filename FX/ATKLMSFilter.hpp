#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Adaptive
{

    struct LMSFilter : public ATKFilter
    {
        enum {
            PORT_SIZE,            
            PORT_MEMORY,
            PORT_MU,
            PORT_LEARNING,
            PORT_MODE,
        };
        ATK::LMSFilter<DspFloatType> * filter;
        LMSFilter(size_t n) :                 
        ATKFilter()
        {
            this->setFilter(filter = new ATK::LMSFilter<DspFloatType>(n));
        }
        ~LMSFilter() {
            if(filter) delete filter;
        }
        void setPort(int port, DspFloatType value) {
            switch(port)
            {
            case PORT_SIZE: filter->set_size(value); break;
            case PORT_MEMORY: filter->set_memory(value); break;
            case PORT_MU: filter->set_mu(value); break;
            case PORT_LEARNING: filter->set_learning(value); break;
            case PORT_MODE: filter->set_mode((ATK::LMSFilter<DspFloatType>::Mode)value); break;
            }
        }
    };

}