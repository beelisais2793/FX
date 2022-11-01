%module kfr_fft
%{
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <random> 

#include <kfr/kfr.h>
#include <kfr/dsp.hpp>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>

using namespace kfr;

%}

%include "stdint.i"
%include "std_vector.i"
%include "std_complex.i"

namespace kfr::CMT_ARCH_NAME
{
    namespace intrinsics {
        template <typename T> univector<T> convolve(const univector_ref<const T>& src1, const univector_ref<const T>& src2);
        template <typename T> univector<T> correlate(const univector_ref<const T>& src1, const univector_ref<const T>& src2);
        template <typename T> univector<T> autocorrelate(const univector_ref<const T>& src1);
    } 
        
    template <typename T, univector_tag Tag1, univector_tag Tag2> univector<T> convolve(const univector<T, Tag1>& src1, const univector<T, Tag2>& src2);
    template <typename T, univector_tag Tag1, univector_tag Tag2> univector<T> correlate(const univector<T, Tag1>& src1, const univector<T, Tag2>& src2);        
    template <typename T, univector_tag Tag1>  univector<T> autocorrelate(const univector<T, Tag1>& src);

}
namespace kfr {
    template <typename T> class convolve_filter : public filter<T>
    {
    public:
        
        explicit convolve_filter(size_t size, size_t block_size = 1024);
        explicit convolve_filter(const univector_ref<const T>& data, size_t block_size = 1024);
        
        void set_data(const univector_ref<const T>& data);
        void reset() final;
        /// Apply filter to multiples of returned block size for optimal processing efficiency.
        size_t input_block_size() const { return block_size; }    

        void apply(univector<T> & in);
        void apply(univector<T> & in, univector<T> & out);

    };

    template <typename T> filter<T>* make_convolve_filter(const univector_ref<const T>& taps, size_t block_size);    
}

%template (convolve_filter) kfr::convolve_filter<SampleType>;
%template (make_convolve_filter) kfr::make_convolve_filter<SampleType>;

namespace kfr {
    
    
    enum class dft_type
    {
        both,
        direct,
        inverse
    };

    enum class dft_order
    {
        normal,
        internal, // possibly bit/digit-reversed, implementation-defined, faster to compute
    };

    enum class dft_pack_format
    {
        Perm, // {X[0].r, X[N].r}, ... {X[i].r, X[i].i}, ... {X[N-1].r, X[N-1].i}
        CCs // {X[0].r, 0}, ... {X[i].r, X[i].i}, ... {X[N-1].r, X[N-1].i},  {X[N].r, 0}
    };    

    template <typename T>
    struct dft_plan
    {
        size_t size;
        size_t temp_size;

        explicit dft_plan(size_t size, dft_order order = dft_order::normal);
        ~dft_plan() {}

        void dump() const;
        void execute(complex<T>* out, const complex<T>* in, u8* temp,bool inverse = false) const;        
        

        template <bool inverse> void execute(complex<T>* out, const complex<T>* in, u8* temp, cbool_t<inverse> inv) const;

        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<complex<T>, Tag1>& out, const univector<complex<T>, Tag2>& in,univector<u8, Tag3>& temp, bool inverse = false) const;
        
        template <bool inverse, univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<complex<T>, Tag1>& out, const univector<complex<T>, Tag2>& in,
                                    univector<u8, Tag3>& temp, cbool_t<inverse> inv) const;

        
        size_t data_size;
        
    };

    template <typename T>
    struct dft_plan_real
    {
        size_t size;
    
        explicit dft_plan_real(size_t size, dft_pack_format fmt = dft_pack_format::CCs);
                                
        void execute(complex<T>* out, const T* in, u8* temp) const;
        void execute(T* out, const complex<T>* in, u8* temp) const;
        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<complex<T>, Tag1>& out, const univector<T, Tag2>& in,
                                    univector<u8, Tag3>& temp) const;
        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<T, Tag1>& out, const univector<complex<T>, Tag2>& in,
                                    univector<u8, Tag3>& temp) const;
                
    };

    /// @brief DCT type 2 (unscaled)
    template <typename T>
    struct dct_plan
    {
        dct_plan(size_t size);
        void execute(T* out, const T* in, u8* temp, bool inverse = false) const;

        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<T, Tag1>& out, const univector<T, Tag2>& in,
                                    univector<u8, Tag3>& temp, bool inverse = false) const;
    };

    
}

%template(dft_plan) kfr::dft_plan<SampleType>;
%template(dft_plan_real) kfr::dft_plan_real<SampleType>;
%template(dct_plan) kfr::dct_plan<SampleType>;
