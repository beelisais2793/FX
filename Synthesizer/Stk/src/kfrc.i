%module kfrc 
%{
#include "capi.h"
#include <cassert>
#include <vector>

%}
%include "stdint.i"
%include "std_vector.i"


enum
{
    KFR_ARCH_X86    = 0,
    KFR_ARCH_SSE2   = 1,
    KFR_ARCH_SSE3   = 2,
    KFR_ARCH_SSSE3  = 3,
    KFR_ARCH_SSE41  = 4,
    KFR_ARCH_SSE42  = 5,
    KFR_ARCH_AVX    = 6,
    KFR_ARCH_AVX2   = 7,
    KFR_ARCH_AVX512 = 8,
};

typedef enum KFR_DFT_PACK_FORMAT
    {
        Perm = 0,
        CCs  = 1
    } KFR_DFT_PACK_FORMAT;


%inline %{

    typedef kfr_c32 complex32;
    typedef kfr_c64 complex64;

    struct DFTPlanComplex32 {
        KFR_DFT_PLAN_F32 *plan;

        DFTPlanComplex32(size_t n) {
            plan = kfr_dft_create_plan_f32(n);
            assert(plan != NULL);
        }
        ~DFTPlanComplex32() {
            if(plan) 
                kfr_dft_delete_plan_f32(plan);
        }

        void dump() {
            kfr_dft_dump_f32(plan);
        }
        size_t get_size() {
            return kfr_dft_get_size_f32(plan);
        }
        size_t get_temp_size() {
            return kfr_dft_get_temp_size_f32(plan);
        }
        void forward(complex32* out, const complex32* in, uint8_t * temp) {
            kfr_dft_execute_f32(plan,out,in,temp);
        }
        void inverse(complex32* out, const complex32* in, uint8_t * temp) {
            kfr_dft_execute_inverse_f32(plan,out,in,temp);
        }
    };

    struct DFTPlanComplex64 {
        KFR_DFT_PLAN_F64 *plan;

        DFTPlanComplex64(size_t n) {
            plan = kfr_dft_create_plan_f64(n);
            assert(plan != NULL);
        }
        ~DFTPlanComplex64() {
            if(plan) 
                kfr_dft_delete_plan_f64(plan);
        }

        void dump() {
            kfr_dft_dump_f64(plan);
        }
        size_t get_size() {
            return kfr_dft_get_size_f64(plan);
        }
        size_t get_temp_size() {
            return kfr_dft_get_temp_size_f64(plan);
        }
        void forward(complex64* out, const complex64* in, uint8_t * temp) {
            kfr_dft_execute_f64(plan,out,in,temp);
        }
        void inverse(complex64* out, const complex64* in, uint8_t * temp) {
            kfr_dft_execute_inverse_f64(plan,out,in,temp);
        }
    };

    struct DFTPlanReal32 {
        KFR_DFT_REAL_PLAN_F32 *plan;

        DFTPlanReal32(size_t n, KFR_DFT_PACK_FORMAT format) {
            plan = kfr_dft_real_create_plan_f32(n,format);
            assert(plan != NULL);
        }
        ~DFTPlanReal32() {
            if(plan) 
                kfr_dft_real_delete_plan_f32(plan);
        }

        void dump() {
            kfr_dft_real_dump_f32(plan);
        }
        size_t get_size() {
            return kfr_dft_real_get_size_f32(plan);
        }
        size_t get_temp_size() {
            return kfr_dft_real_get_temp_size_f32(plan);
        }
        void forward(complex32* out, const float* in, uint8_t * temp) {
            kfr_dft_real_execute_f32(plan,out,in,temp);
        }
        void inverse(float* out, const complex32* in, uint8_t * temp) {
            kfr_dft_real_execute_inverse_f32(plan,out,in,temp);
        }
    };

    struct DFTPlanReal64 {
        KFR_DFT_REAL_PLAN_F64 *plan;

        DFTPlanReal64(size_t n, KFR_DFT_PACK_FORMAT format) {
            plan = kfr_dft_real_create_plan_f64(n,format);
            assert(plan != NULL);
        }
        ~DFTPlanReal64() {
            if(plan) 
                kfr_dft_real_delete_plan_f64(plan);
        }

        void dump() {
            kfr_dft_real_dump_f64(plan);
        }
        size_t get_size() {
            return kfr_dft_real_get_size_f64(plan);
        }
        size_t get_temp_size() {
            return kfr_dft_real_get_temp_size_f64(plan);
        }
        void forward(complex64* out, const double* in, uint8_t * temp) {
            kfr_dft_real_execute_f64(plan,out,in,temp);
        }
        void inverse(double* out, const complex64* in, uint8_t * temp) {
            kfr_dft_real_execute_inverse_f64(plan,out,in,temp);
        }
    };

    struct DCTPlan32 {
        KFR_DCT_PLAN_F32 *plan;

        DCTPlan32(size_t n) {
            plan = kfr_dct_create_plan_f32(n);
            assert(plan != NULL);
        }
        ~DCTPlan32() {
            if(plan) 
                kfr_dct_delete_plan_f32(plan);
        }

        void dump() {
            kfr_dct_dump_f32(plan);
        }
        size_t get_size() {
            return kfr_dct_get_size_f32(plan);
        }
        size_t get_temp_size() {
            return kfr_dct_get_temp_size_f32(plan);
        }
        void forward(float* out, const float* in, uint8_t * temp) {
            kfr_dct_execute_f32(plan,out,in,temp);
        }
        void inverse(float* out, const float* in, uint8_t * temp) {
            kfr_dct_execute_inverse_f32(plan,out,in,temp);
        }
    };


    struct DCTPlan64 {
        KFR_DCT_PLAN_F64 *plan;

        DCTPlan64(size_t n) {
            plan = kfr_dct_create_plan_f64(n);
            assert(plan != NULL);
        }
        ~DCTPlan64() {
            if(plan) 
                kfr_dct_delete_plan_f64(plan);
        }

        void dump() {
            kfr_dct_dump_f64(plan);
        }
        size_t get_size() {
            return kfr_dct_get_size_f64(plan);
        }
        size_t get_temp_size() {
            return kfr_dct_get_temp_size_f64(plan);
        }
        void forward(double* out, const double* in, uint8_t * temp) {
            kfr_dct_execute_f64(plan,out,in,temp);
        }
        void inverse(double* out, const double* in, uint8_t * temp) {
            kfr_dct_execute_inverse_f64(plan,out,in,temp);
        }
    };

    struct FirFilter32
    {
        KFR_FILTER_F32* filter;

        FirFilter32(const float * taps, size_t size) {
            filter = kfr_filter_create_fir_plan_f32(taps,size);
            assert(filter != nullptr);
        }
        ~FirFilter32() {
            if(filter) kfr_filter_delete_plan_f32(filter);
        }

        void process(float * output, const float * input, size_t size) {
            kfr_filter_process_f32(filter,output,input,size);
        }

        void reset() {
            kfr_filter_reset_f32(filter);
        }
    };
    struct FirFilter64
    {
        KFR_FILTER_F64* filter;

        FirFilter64(const double * taps, size_t size) {
            filter = kfr_filter_create_fir_plan_f64(taps,size);
            assert(filter != nullptr);
        }
        ~FirFilter64() {
            if(filter) kfr_filter_delete_plan_f64(filter);
        }

        void process(double * output, const double * input, size_t size) {
            kfr_filter_process_f64(filter,output,input,size);
        }

        void reset() {
            kfr_filter_reset_f64(filter);
        }
    };


    struct ConvolutionFilter32
    {
        KFR_FILTER_F32* filter;

        ConvolutionFilter32(const float * taps, size_t size, size_t block_size) {
            filter = kfr_filter_create_convolution_plan_f32(taps,size,block_size);
            assert(filter != nullptr);
        }
        ~ConvolutionFilter32() {
            if(filter) kfr_filter_delete_plan_f32(filter);
        }

        void process(float * output, const float * input, size_t size) {
            kfr_filter_process_f32(filter,output,input,size);
        }

        void reset() {
            kfr_filter_reset_f32(filter);
        }
    };
    struct ConvolutionFilter64
    {
        KFR_FILTER_F64* filter;

        ConvolutionFilter64(const double * taps, size_t size, size_t block_size) {
            filter = kfr_filter_create_convolution_plan_f64(taps,size,block_size);
            assert(filter != nullptr);
        }
        ~ConvolutionFilter64() {
            if(filter) kfr_filter_delete_plan_f64(filter);
        }

        void process(double * output, const double * input, size_t size) {
            kfr_filter_process_f64(filter,output,input,size);
        }

        void reset() {
            kfr_filter_reset_f64(filter);
        }
    };
  
    struct IirFilter32
    {
        KFR_FILTER_F32* filter;

        IirFilter32(const float * sos, size_t size) {
            filter = kfr_filter_create_iir_plan_f32(sos,size);
            assert(filter != nullptr);
        }
        ~IirFilter32() {
            if(filter) kfr_filter_delete_plan_f32(filter);
        }

        void process(float * output, const float * input, size_t size) {
            kfr_filter_process_f32(filter,output,input,size);
        }

        void reset() {
            kfr_filter_reset_f32(filter);
        }
    };
    struct IirFilter64
    {
        KFR_FILTER_F64* filter;

        IirFilter64(const double * sos, size_t size) {
            filter = kfr_filter_create_iir_plan_f64(sos,size);
            assert(filter != nullptr);
        }
        ~IirFilter64() {
            if(filter) kfr_filter_delete_plan_f64(filter);
        }

        void process(double * output, const double * input, size_t size) {
            kfr_filter_process_f64(filter,output,input,size);
        }

        void reset() {
            kfr_filter_reset_f64(filter);
        }
    };

%}