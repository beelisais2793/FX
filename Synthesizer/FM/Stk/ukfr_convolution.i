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
    namespace intrinsics {
        template <typename T> univector<T> convolve(const univector_ref<const T>& src1, const univector_ref<const T>& src2);
        template <typename T> univector<T> correlate(const univector_ref<const T>& src1, const univector_ref<const T>& src2);
        template <typename T> univector<T> autocorrelate(const univector_ref<const T>& src1);
    } 
        
    template <typename T> univector<T> convolve(const univector<T>& src1, const univector<T>& src2);
    template <typename T> univector<T> correlate(const univector<T>& src1, const univector<T>& src2);        
    template <typename T> univector<T> autocorrelate(const univector<T>& src);

}
namespace kfr {
    template <typename T> class convolve_filter
    {
    public:
        
        explicit convolve_filter(size_t size, size_t block_size = 1024);
        explicit convolve_filter(const univector<T>& data, size_t block_size = 1024);
        
        void set_data(const univector<T>& data);
        void reset() final;
        /// Apply filter to multiples of returned block size for optimal processing efficiency.
        size_t input_block_size() const { return block_size; }    

        void apply(univector<T> & in);
        void apply(univector<T> & in, univector<T> & out);
        
        void apply(T* in,size_t);
        void apply(T* dst, T* src,size_t size);

    };

    template <typename T> filter<T>* make_convolve_filter(const univector<T>& taps, size_t block_size);    
    
}

%template (convolve_filter) kfr::convolve_filter<SampleType>;
%template (make_convolve_filter) kfr::make_convolve_filter<SampleType>;

%template (convolve) DSP::convolve<SampleType>;
%template (autocorrelate) DSP::autocorrelate<SampleType>;
%template (correlate) DSP::correlate<SampleType>;
