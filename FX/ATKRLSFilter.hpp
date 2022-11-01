#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Adaptive
{

    struct RLSFilter : public ATKFilter
    {
        enum {
            PORT_SIZE,            
            PORT_MEMORY,           
            PORT_LEARNING,            
        };
        ATK::RLSFilter<DspFloatType> * filter;
        RLSFilter(size_t n) :                 
        ATKFilter()
        {
            this->setFilter(filter = new ATK::RLSFilter<DspFloatType>(n));
            
        }
        ~RLSFilter() {
            if(filter) delete filter;
        }
        void setPort(int port, DspFloatType value) {
            switch(port)
            {
            case PORT_SIZE: filter->set_size(value); break;
            case PORT_MEMORY: filter->set_memory(value); break;
            case PORT_LEARNING: filter->set_learning(value); break;
            }
        }
    };

}