#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{
    struct BesselLowPassFilter : public ATKFilter
    {
        ATK::BesselLowPassCoefficients<DspFloatType> * filter;

        BesselLowPassFilter(int order, DspFloatType cutoff) : ATKFilter()
        {
            filter = new ATK::BesselLowPassCoefficients<DspFloatType>(2);
            filter->set_order(order);
            filter->set_cut_frequency(cutoff);
            this->setFilter(filter);
        }
        ~BesselLowPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF,
            PORT_ORDER
        };
        void printPorts() {
            printf("PORTS\n");
            printf("PORT_CUTOFF=0\n");
            printf("PORT_ORDER=1\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_ORDER: filter->set_order(v); break;
            }
        }
        DspFloatType getPort(int port) {
            switch(port) {
                case PORT_CUTOFF: return filter->get_cut_frequency(); break;
                case PORT_ORDER: return filter->get_order(); break;
            }
        }
    };
    struct BesselHighPassFilter : public ATKFilter
    {
        ATK::BesselHighPassCoefficients<DspFloatType> * filter;

        BesselHighPassFilter(int order, DspFloatType cutoff) : ATKFilter()
        {
            filter = new ATK::BesselHighPassCoefficients<DspFloatType>(2);
            filter->set_order(order);
            filter->set_cut_frequency(cutoff);
            this->setFilter(filter);
        }
        ~BesselHighPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF,
            PORT_ORDER
        };
        void printPorts() {
            printf("PORTS\n");
            printf("PORT_CUTOFF=0\n");
            printf("PORT_ORDER=1\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: filter->set_cut_frequency(v); break;
                case PORT_ORDER: filter->set_order(v); break;
            }
        }
        DspFloatType getPort(int port) {
            switch(port) {
                case PORT_CUTOFF: return filter->get_cut_frequency(); break;
                case PORT_ORDER: return filter->get_order(); break;
            }
        }
    };
    struct BesselBandStopFilter : public ATKFilter
    {
        ATK::BesselBandStopCoefficients<DspFloatType> * filter;
        DspFloatType f0,f1;
        BesselBandStopFilter(int order, DspFloatType f1, DspFloatType f2) : ATKFilter()
        {
            filter = new ATK::BesselBandStopCoefficients<DspFloatType>(2);
            f0 = f1;
            f1 = f2;
            filter->set_order(order);
            filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1));
            this->setFilter(filter);
        }
        ~BesselBandPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF1,
            PORT_CUTOFF2,
            PORT_ORDER
        };
        void printPorts() {
            printf("PORTS\n");
            printf("PORT_CUTOFF1=0\n");
            printf("PORT_CUTOFF2=1\n");
            printf("PORT_ORDER=2\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF1: filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0=v,f1)); break;
                case PORT_CUTOFF2: filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1=v)); break;
                case PORT_ORDER: filter->set_order(v); break;
            }
        }
        DspFloatType getPort(int port) {
            std::pair<DspFloatType,DspFloatType> x = filter->get_cut_frequencies();
            f0 = x->first;
            f1 = x->second;
            switch(port) {
                case PORT_CUTOFF1: return f0;
                case PORT_CUTOFF2: return f1;
                case PORT_ORDER: return filter->get_order(); break;
            }
        }
    };
    struct BesselBandPassFilter : public ATKFilter
    {
        ATK::BesselBandPassCoefficients<DspFloatType> * filter;
        DspFloatType f0,f1;
        BesselBandStopFilter(int order, DspFloatType f1, DspFloatType f2) : ATKFilter()
        {
            filter = new ATK::BesselBandStopCoefficients<DspFloatType>(2);
            f0 = f1;
            f1 = f2;
            filter->set_order(order);
            filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1));
            this->setFilter(filter);
        }
        ~BesselBandPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF1,
            PORT_CUTOFF2,
            PORT_ORDER
        };
        void printPorts() {
            printf("PORTS\n");
            printf("PORT_CUTOFF1=0\n");
            printf("PORT_CUTOFF2=1\n");
            printf("PORT_ORDER=2\n");
        }
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF1: filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0=v,f1)); break;
                case PORT_CUTOFF2: filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1=v)); break;
                case PORT_ORDER: filter->set_order(v); break;
            }
        }
        DspFloatType getPort(int port) {
            std::pair<DspFloatType,DspFloatType> x = filter->get_cut_frequencies();
            f0 = x->first;
            f1 = x->second;
            switch(port) {
                case PORT_CUTOFF1: return f0;
                case PORT_CUTOFF2: return f1;
                case PORT_ORDER: return filter->get_order(); break;
            }
        }
    };
}