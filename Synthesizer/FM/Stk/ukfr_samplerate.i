%{
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <random> 

#include <kfr/kfr.h>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>
#include "kfrcore.hpp"

using namespace kfr;
%}



namespace kfr::CMT_ARCH_NAME 
{
    template <typename T>
    struct samplerate_converter
    {    
        using itype = i64;
        using ftype = subtype<T>;
        static size_t filter_order(sample_rate_conversion_quality quality);
        static ftype sidelobe_attenuation(sample_rate_conversion_quality quality);
        static ftype transition_width(sample_rate_conversion_quality quality);
        static ftype window_param(sample_rate_conversion_quality quality);        
        samplerate_converter(sample_rate_conversion_quality quality, itype interpolation_factor, itype decimation_factor, ftype scale = ftype(1), ftype cutoff = 0.5f);
        itype input_position_to_intermediate(itype in_pos) const;
        itype output_position_to_intermediate(itype out_pos) const;
        itype input_position_to_output(itype in_pos) const;
        itype output_position_to_input(itype out_pos) const;
        itype output_size_for_input(itype input_size) const;
        itype input_size_for_output(itype output_size) const;
        size_t skip(size_t output_size, univector<T> input);        
        size_t process(kfr::univector<T> output, univector<T> input);
        double get_fractional_delay() const;
        size_t get_delay() const;

        ftype kaiser_beta;
        itype depth;
        itype taps;
        size_t order;
        itype interpolation_factor;
        itype decimation_factor;
        //univector<T> filter;
        //univector<T> delay;
        itype input_position;
        itype output_position;
    };
}

%template(samplerate_converter) kfr::CMT_ARCH_NAME::samplerate_converter<SampleType>;
