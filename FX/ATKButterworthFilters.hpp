#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct ButterworthLowPassFilter : public ATKFilter
    {
        ATK::ButterworthLowPassCoefficients<DspFloatType> * filter;        
        ButterworthLowPassFilter(int order, DspFloatType fc) : ATKFilter()
        {
            filter = new ATK::ButterworthLowPassCoefficients<DspFloatType>(2);
            filter->set_order(order);
            filter->set_cut_frequency(fc);
            this->setFilter(filter);
        }
        ~ButterworthLowPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF,            
            PORT_ORDER
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF:  filter->set_cut_frequency(v); break;
                case PORT_ORDER:   filter->set_order(v); break;
            }
        }
        DspFloatType getPort(int port) {            
            switch(port) {
                case PORT_CUTOFF1: return filter->get_cut_frequency();                 
                case PORT_ORDER: return filter->get_order(); break;
            }
        }
    };
    struct ButterworthHighPassFilter : public ATKFilter
    {
        ATK::ButterworthHighPassCoefficients<DspFloatType> * filter;        
        ButterworthHighPassFilter(int order, DspFloatType fc) : ATKFilter()
        {
            filter = new ATK::ButterworthHighCoefficients<DspFloatType>(2);
            filter->set_order(order);
            filter->set_cut_frequency(fc);
            this->setFilter(filter);
        }
        ~ButterworthHighPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF,            
            PORT_ORDER
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF:  filter->set_cut_frequency(v); break;
                case PORT_ORDER:   filter->set_order(v); break;
            }
        }
        DspFloatType getPort(int port) {            
            switch(port) {
                case PORT_CUTOFF1: return filter->get_cut_frequency();                 
                case PORT_ORDER: return filter->get_order(); break;
            }
        }
    };
    struct ButterworthBandPassFilter : public ATKFilter
    {
        ATK::ButterworthBandPassCoefficients<DspFloatType> * filter;        
        DspFloatType f0,f1;
        ButterworthBandPassFilter(int order, DspFloatType f1, DspFloatType f2) : ATKFilter()
        {
            filter = new ATK::ButterworthBandPassCoefficients<DspFloatType>(2);
            f0 = f1;
            f1 = f2;
            filter->set_order(order);
            filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1));
            this->setFilter(filter);
        }
        ~ButterworthBandPassFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF1,
            PORT_CUTOFF2,
            PORT_ORDER
        };
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
    struct ButterworthBandStopFilter : public ATKFilter
    {
        ATK::ButterworthBandStopCoefficients<DspFloatType> * filter;        
        DspFloatType f0,f1;
        ButterworthBandStopFilter(int order, DspFloatType f1, DspFloatType f2) : ATKFilter()
        {
            filter = new ATK::ButterworthBandStopCoefficients<DspFloatType>(2);
            f0 = f1;
            f1 = f2;
            filter->set_order(order);
            filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1));
            this->setFilter(filter);
        }
        ~ButterworthBandStopFilter() {
            if(filter) delete filter;            
        }
        enum {
            PORT_CUTOFF1,
            PORT_CUTOFF2,
            PORT_ORDER
        };
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