#pragma once

#include "ATK.hpp"

namespace Filters::AudioTK::EQ
{  
    struct RemezBasedFilter: public ATKFilter
    {
        ATK::RemezBasedCoefficients<DspFloatType> * filter;

        RemezBasedFilter() : ATKFilter()
        {
            filter = ATK::RemezBasedCoefficients<DspFloatType>(2);
        }
        ~RemezBasedFilter() {
            if(filter) delete filter;
        }
        void setTemplate(std::vector<std::pair<std::pair<DspFloatType,DspFloatType>,std::pair<DspFloatType,DspFloatType>>> & target)
        {
            filter->set_template(target);
        }
    };
}