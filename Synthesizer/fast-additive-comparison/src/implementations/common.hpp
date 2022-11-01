#ifndef GOLDENROCEKEFELLER_FAST_ADDITIVE_COMPARISON_CONSTANTS_HPP
#define GOLDENROCEKEFELLER_FAST_ADDITIVE_COMPARISON_CONSTANTS_HPP

#include <cmath>
#include <array>
#include "xsimd/xsimd.hpp"

namespace goldenrockefeller{ namespace fast_additive_comparison{
    // template <typename T>
    // struct typed_constants{
    //     static const T pi;
    //     static const T tau;
    //     static const T inv_tau;
    // };

    // template <typename T>
    // struct typed_constants{
    //     static constexpr T pi = T(3.141592653589793238462643383279502884197L);
    //     static constexpr T tau = T(6.283185307179586476925286766559005768394L);
    //     static constexpr T inv_tau = T(0.1591549430918953357688837633725143620345L);
    // };

    template <typename T>
    inline constexpr T pi() {return T(3.141592653589793238462643383279502884197L);}

    template <typename T>
    inline constexpr T tau() {return T(6.283185307179586476925286766559005768394L);}

    template <typename T>
    inline constexpr T inv_tau() {return T(0.1591549430918953357688837633725143620345L);}

    // template <typename T>
    // struct typed_constants{
    //     static T pi; 
    //     static T tau;
    //     static T inv_tau;
    // };

    // template <>
    // struct typed_constants<float> : typed_constants_numeric<float> {};

    // template <>
    // struct typed_constants<double> : typed_constants_numeric<double> {};

    // template <>
    // struct typed_constants<long double> : typed_constants_numeric<long double> {};

    template <typename T>
    inline T wrap_phase(const T& phase) { 
        /* Wrap phase between -pi, and pi */
        return phase - round(phase * inv_tau<T>()) * tau<T>();
    }

    template <typename T>
    inline T wrap_phase_bounded(const T& phase) {
        
        
        /* Wrap phase between -pi, and pi, but raw phase must be between -pi and 3 * pi */
        return phase - T(phase > pi<T>()) * tau<T>();
    }   

    template <typename T>
    inline T wrap_phase_offset(const T& phase) {
        
        
        /* Wrap phase between 0, and 2 * pi */
        return phase - floor(phase * inv_tau<T>()) * tau<T>();
    } 

    template <typename T>
    inline T cos(const T& x);

    using std::cos;

    template <typename sample_type, typename operand_type>
    inline void load(const sample_type* ptr, operand_type& operand);

    template <typename sample_type>
    inline void load(const sample_type* ptr, sample_type& operand) {
        operand = *ptr;
    }

    template <typename sample_type, typename operand_type>
    inline void store(sample_type* ptr, const operand_type& operand);


    template <typename sample_type>
    inline void store(sample_type* ptr, const sample_type& operand) {
        *ptr = operand;
    }

    
    template<>
	inline void load<float, xsimd::batch<float, xsimd::avx>>(const float* ptr, xsimd::batch<float, xsimd::avx>& operand) {
		operand = xsimd::batch<float, xsimd::avx>::load_unaligned(ptr);
	}

	template<>
	inline void store<float, xsimd::batch<float, xsimd::avx>>(float* ptr, const xsimd::batch<float, xsimd::avx>& operand) {
		operand.store_unaligned(ptr);
	}

        
    template<>
	inline void load<double, xsimd::batch<double, xsimd::avx>>(const double* ptr, xsimd::batch<double, xsimd::avx>& operand) {
		operand = xsimd::batch<double, xsimd::avx>::load_unaligned(ptr);
	}

	template<>
	inline void store<double, xsimd::batch<double, xsimd::avx>>(double* ptr, const xsimd::batch<double, xsimd::avx>& operand) {
		operand.store_unaligned(ptr);
	}

    template <typename operand_type>
    inline operand_type approx_cos_deg_14(const operand_type& x) {
        // TODO replace with FMA

        // ((C0 + C2 x2) + (C4 + c6 x2) x4) + ((C8 + C10 x2) + (C12 + c14 x2) x4) x8

        const operand_type c0 = operand_type(0x1.ffffffff470fdp-1);
        const operand_type c2 = operand_type(-0x1.ffffffec1c40dp-2);
        const operand_type c4 = operand_type(0x1.555553f050eb2p-5);
        const operand_type c6 = operand_type(-0x1.6c169b776ec06p-10);
        const operand_type c8 = operand_type(0x1.a0160ea01af9bp-16);
        const operand_type c10 = operand_type(-0x1.27abf550a036ap-22);
        const operand_type c12 = operand_type(0x1.1b5c0b8055789p-29);
        const operand_type c14 = operand_type(-0x1.577f9d3aa99cep-37);

        auto x2 = x * x;
        auto x4 = x2 * x2;
        auto x8 = x4 * x4;
        return ((c0 + c2 * x2) + (c4 + c6 *x2)* x4) + ((c8 + c10* x2) + (c12 + c14 *x2) *x4) *x8;
        
    }

    template <typename operand_type>
    inline operand_type approx_cos_deg_10(const operand_type& x) {
        // TODO replace with FMA

        // ((C0 + C2 x2) + (C4 + c6 x2) x4) + (C8 + C10 x2) x8

        const operand_type c0 = operand_type(0x1.ffffeae5f1250p-1);
        const operand_type c2 = operand_type(-0x1.fffeb753ad1bbp-2);
        const operand_type c4 = operand_type(0x1.55487f14c051dp-5);
        const operand_type c6 = operand_type(-0x1.6b5c05901b865p-10);
        const operand_type c8 = operand_type(0x1.9682ebc930c70p-16);
        const operand_type c10 = operand_type(-0x1.da3f341d8d3f1p-23);

        auto x2 = x * x;
        auto x4 = x2 * x2;
        auto x8 = x4 * x4;
        return ((c0 + c2 * x2) + (c4 + c6 *x2)* x4) + (c8 + c10* x2) *x8;
    }
}}

#endif