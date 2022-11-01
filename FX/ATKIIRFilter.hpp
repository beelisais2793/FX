#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct IIRFilter : public ATKFilter
    {
        ATK::IIRFilterCoefficients<ATK::CustomIIRFilterCoefficients<DspFloatType>> * filter;

        IIRFilter(const std::vector<DspFloatType> & b,const std::vector<DspFloatType> & a) : ATKFilter()
        {
            filter = new ATK::IIRFilter<ATK::CustomFIRFilterCoefficients<DspFloatType>>(2);            
            filter->set_coefficients_in(b);
            filter->set_coefficients_out(a);
            this->setFilter(filter);
        }
        ~IIRFilter() {
            if(filter) delete filter;
        }
        void setCoefficientsconst(const std::vector<DspFloatType> & b,const std::vector<DspFloatType> & a) 
        {            
            filter->set_coefficients_in(b);
            filter->set_coefficients_out(a);
        }
    };
