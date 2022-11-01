#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::Dynamics
{
    struct AttackRelease : public ATKFilter
    {
        ATK::AttackReleaseFilter<DspFloatType> * filter;

        AttackRelease()
        {
            filter = new ATK::AttackReleaseFilter<DspFloatType>(2);
            this->setFilter(filter);
        }
        ~AttackRelease() {
            if(filter) delete filter;
        }
        enum {
            PORT_ATTACK,
            PORT_RELEASE            
        };
        void printPorts() {
            printf("PORTS\n");
            printf("PORT_ATTACK=0\n");
            printf("PORT_RELEASE=1\n");
        }
        void setPort(int port, DspFloatType value) {
            switch(port) {
                case PORT_ATTACK: filter->set_attack(port,value); break;
                case PORT_RELEASE: filter->set_release(port,value); break;
            }
        }
    };
}