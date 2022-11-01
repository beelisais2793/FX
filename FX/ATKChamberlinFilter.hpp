#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct ChamberlinFilter : public ATKFilter
    {
        ATK::ChamberlinFilter<DspFloatType> * filter;

        ChamberlinFilter() : ATKFilter() {
            filter = new ATK::ChamberlinFilter<DspFloatType>;            
            this->setFilter(filter);
        }
        ~ChamberlinFilter() {
            if(filter) delete filter;
        }
        enum {
            PORT_CUTOFF,
            PORT_ATTEN,
            PORT_LP,
            PORT_BP,
            PORT_HP,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_LP: filter->select(0); break;
                case PORT_BP: filter->select(1); break;
                case PORT_HP: filter->select(2); break;
            }
        }
        DspFloatType getPort(int port) {
            if(port == PORT_ATTEN) return filter->get_attenuation();
            return 0;
        }
    };
}