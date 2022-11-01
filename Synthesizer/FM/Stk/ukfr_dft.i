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
namespace kfr 
{
    
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

        
        void execute(univector<complex<T>>& out, const univector<complex<T>>& in,univector<u8>& temp, bool inverse = false) const;                
        //void execute(univector<complex<T>>& out, const univector<complex<T>>& in,univector<u8>& temp, cbool_t<kfr::dft_type::inverse> inv) const;

        
        size_t data_size;
        
    };

    template <typename T>
    struct dft_plan_real
    {
        size_t size;
    
        explicit dft_plan_real(size_t size, dft_pack_format fmt = dft_pack_format::CCs);
                                
        void execute(complex<T>* out, const T* in, u8* temp) const;
        void execute(T* out, const complex<T>* in, u8* temp) const;
        void execute(univector<complex<T>>& out, const univector<T>& in,univector<u8>& temp) const;        
        void execute(univector<T>& out, const univector<complex<T>>& in,univector<u8>& temp) const;
                
    };

    /// @brief DCT type 2 (unscaled)
    template <typename T>
    struct dct_plan
    {
        dct_plan(size_t size);
        void execute(T* out, const T* in, u8* temp, bool inverse = false) const;
        
        void execute(univector<T>& out, const univector<T>& in,univector<u8>& temp, bool inverse = false) const;
    };

    
}

%template(dft_plan) kfr::dft_plan<SampleType>;
%template(dft_plan_real) kfr::dft_plan_real<SampleType>;
%template(dct_plan) kfr::dct_plan<SampleType>;

%template(dft) DSP::run_dft<SampleType>;
%template(realdft) DSP::run_realdft<SampleType>;
%template(idft) DSP::run_idft<SampleType>;
%template(irealdft) DSP::run_irealdft<SampleType>;

%template (DCTPlan) DSP::DCTPlan<SampleType>;
%template (DFTPlan) DSP::DFTPlan<SampleType>;
%template (DFTRealPlan) DSP::DFTRealPlan<SampleType>;
