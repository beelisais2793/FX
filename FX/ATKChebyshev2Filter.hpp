#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct Chebyshev2LowPassFilter : public ATKFilter
    {
        ATK::Chebyshev2LowPassCoefficients<DspFloatType> * filter;        
        Chebyshev2LowPassFilter(int order, DspFloatType fc) : ATKFilter()
        {
            filter = new ATK::Chebyshev2LowPassCoefficients<DspFloatType>(2);
            filter->set_order(order);
            filter->set_cut_frequency(fc);
            this->setFilter(filter);
        }
        ~Chebyshev2LowPassFilter() {
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
    struct Chebyshev2HighPassFilter : public ATKFilter
    {
        ATK::Chebyshev2HighPassCoefficients<DspFloatType> * filter;        
        Chebyshev2HighPassFilter(int order, DspFloatType fc) : ATKFilter()
        {
            filter = new ATK::Chebyshev2HighCoefficients<DspFloatType>(2);
            filter->set_order(order);
            filter->set_cut_frequency(fc);
            this->setFilter(filter);
        }
        ~Chebyshev2HighPassFilter() {
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
    struct Chebyshev2BandPassFilter : public ATKFilter
    {
        ATK::Chebyshev2BandPassCoefficients<DspFloatType> * filter;        
        DspFloatType f0,f1;
        Chebyshev2BandPassFilter(int order, DspFloatType f1, DspFloatType f2) : ATKFilter()
        {
            filter = new ATK::Chebyshev2BandPassCoefficients<DspFloatType>(2);
            f0 = f1;
            f1 = f2;
            filter->set_order(order);
            filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1));
            this->setFilter(filter);
        }
        ~Chebyshev2BandPassFilter() {
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
    struct Chebyshev2BandStopFilter : public ATKFilter
    {
        ATK::Chebyshev2BandStopCoefficients<DspFloatType> * filter;        
        DspFloatType f0,f1;
        Chebyshev2BandStopFilter(int order, DspFloatType f1, DspFloatType f2) : ATKFilter()
        {
            filter = new ATK::Chebyshev2BandStopCoefficients<DspFloatType>(2);
            f0 = f1;
            f1 = f2;
            filter->set_order(order);
            filter->set_cut_frequencies(std::pair<DspFloatType,DspFloatType>(f0,f1));
            this->setFilter(filter);
        }
        ~Chebyshev2BandStopFilter() {
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