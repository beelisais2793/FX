#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Adaptive
{

    struct BlockLMSFilter : public ATKFilter
    {
        ATK::BlockLMSFilter<DspFloatType> * filter;
        
        enum {
            PORT_SIZE,
            PORT_BLOCKSIZE,
            PORT_MEMORY,
            PORT_MU,
            PORT_LEARNING,
        };
        
        BlockLMSFilter(size_t n) :         
        ATKFilter()
        {
            this->setFilter(filter = new ATK::BlockLMSFilter<DspFloatType>(n));
            
        }        
        ~BlockLMSFilter() {
            if(filter) delete filter;
        }
        void setPort(int port, DspFloatType value) {
            
            switch(port)
            {
            case PORT_SIZE: filter->set_size(value); break;
            case PORT_BLOCKSIZE: filter->set_block_size(value); break;
            case PORT_MEMORY: filter->set_memory(value); break;
            case PORT_MU: filter->set_mu(value); break;
            case PORT_LEARNING: filter->set_learning(value); break;
            }
        }
    };
}