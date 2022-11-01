#ifndef __CPPMKL_CBLAS_H__
#define __CPPMKL_CBLAS_H__
#include <assert.h>
#include <mkl_cblas.h>
#include "cppmkl/cppmkl_type_utils.h"

namespace cppmkl
{
  /**
 * Template function wrappers for BLAS routines.
 * Vector types must have a size() function. The
 * Matrix types must have size1(), size2() functions, and value_type typedef. 
 * size1() returns the number of rows, size2() the number of columns
 *
 * The template functions here assume all Matrix types are row major. 
 * This can be generalised by adding type traits.
 */
  // in order to default alpha and beta we need something that works for 
  // both real and complex, this can (hopefully) be made much simpler:
  template<typename T>
   inline T _alpha_default()
    {
      return T();
    }
  template<>
   inline double _alpha_default<double>()
    {
      return 1.0;
    }
  template<>
   inline float _alpha_default<float>()
    {
      return 1.0;
    }
  template<>
    inline MKL_Complex8 _alpha_default<MKL_Complex8>()
    {
      static MKL_Complex8 c;
      c.real = 1.0;
      c.imag = 0.0;
      return c;

    }
  template<>
    inline MKL_Complex16 _alpha_default<MKL_Complex16>()
    {
      static MKL_Complex16 c;
      c.real = 1.0;
      c.imag = 0.0;
      return c;

    }
  template<typename T>
    inline T _beta_default()
    {
      return T();
    }
  inline float cblas_asum(const MKL_INT N, const float *X, const MKL_INT incX)
  {
      return cblas_sasum(N, X, incX);
  }
  inline float cblas_asum(const MKL_INT N, const MKL_Complex8 *X, const MKL_INT incX)
  {
    return cblas_scasum(N, static_cast<const void*>(X), incX);
  }
  inline double cblas_asum(const MKL_INT N, const double *X, const MKL_INT incX)
  { 
    return cblas_dasum(N, X, incX);
  }
  inline double cblas_asum(const MKL_INT N, const MKL_Complex16 *X, const MKL_INT incX)
  {
    return cblas_dzasum(N, static_cast<const void*>(X), incX);
  }

  //always return a double, auto conversion to float in calling code
  template <typename VECTOR_T>
  double cblas_asum(const VECTOR_T& v, const MKL_INT incX=1)
  {
    return cblas_asum(v.size()/incX, ptr_to_first(v), incX);
  }

  inline void cblas_axpy(const MKL_INT N, const float alpha, const float *X,
                 const MKL_INT incX, float *Y, const MKL_INT incY)
  {
    cblas_saxpy(N, alpha, X, incX, Y, incY);
  }
  inline void cblas_axpy(const MKL_INT N, const double alpha, const double *X,
                 const MKL_INT incX, double *Y, const MKL_INT incY)
  {
    cblas_daxpy(N, alpha, X, incX, Y, incY);
  }
  inline void cblas_axpy(const MKL_INT N, const float alpha, const MKL_Complex8 *X,
                 const MKL_INT incX, MKL_Complex8 *Y, const MKL_INT incY)
  {
    cblas_caxpy(N, static_cast<const void*>(&alpha), static_cast<const void*>(X), 
                incX, static_cast<void*>(Y), incY);
  }
  inline void cblas_axpy(const MKL_INT N, const double alpha, const MKL_Complex16 *X,
                 const MKL_INT incX, MKL_Complex16 *Y, const MKL_INT incY)
  {
    cblas_zaxpy(N, static_cast<const void *>(&alpha), static_cast<const void *>(X),
                incX, static_cast<void*>(Y), incY);
  }
  template<typename VECTOR_T>
  inline void cblas_axpy(const VECTOR_T& x, VECTOR_T& y, double a=1.0, 
                         const MKL_INT incX=1, const MKL_INT incY=1)
  {
    cblas_axpy(x.size()/incX, a, ptr_to_first(x), incX, ptr_to_first(y), incY);
  }


  inline void cblas_copy(const MKL_INT N, const float* X, const MKL_INT incX, 
                         float* Y, const MKL_INT incY)
  {
    cblas_scopy(N, X, incX, Y, incY);    
  }
  inline void cblas_copy(const MKL_INT N, const double* X, const MKL_INT incX, 
                         double* Y, const MKL_INT incY)
  {
    cblas_dcopy(N, X, incX, Y, incY);    
  }
  inline void cblas_copy(const MKL_INT N, const MKL_Complex8* X, const MKL_INT incX, 
                         MKL_Complex8* Y, const MKL_INT incY)
  {
    cblas_ccopy(N, static_cast<const void*>(X), incX, static_cast<void*>(Y), incY);    
  }
  inline void cblas_copy(const MKL_INT N, const MKL_Complex16* X, const MKL_INT incX, 
                         MKL_Complex16* Y, const MKL_INT incY)
  {
    cblas_zcopy(N, static_cast<const void*>(X), incX, static_cast<void*>(Y), incY);    
  }
   
  template <typename VECTOR_T>
  inline void cblas_copy(const VECTOR_T& x, VECTOR_T& y, const MKL_INT incX=1, const MKL_INT incY=1)
  {
    cblas_copy(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY);
  }

  // dot product of vectors
  inline double cblas_dot(const MKL_INT N, const float* X, const MKL_INT incX, 
                         float* Y, const MKL_INT incY)
  {
    return cblas_sdot(N, X, incX, Y, incY);    
  }
  inline double cblas_dot(const MKL_INT N, const double* X, const MKL_INT incX, 
                         double* Y, const MKL_INT incY)
  {
    return cblas_ddot(N, X, incX, Y, incY);    
  }
   
  template <typename VECTOR_T>
  inline double cblas_dot(const VECTOR_T& x, VECTOR_T& y, const MKL_INT incX=1, const MKL_INT incY=1)
  {
    return cblas_dot(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY);
  }
  

  /*
 * The ?sdot routines compute the inner product of two vectors with extended precision. Both
 * routines use extended precision accumulation of the intermediate results, but the sdsdot
 * routine outputs the final result in single precision, whereas the dsdot routine outputs the double
 * precision result. The function sdsdot also adds scalar value sb to the inner product.
 *
 * not sure I understand this one, tricky to wrap, going to use dsdot for both variants
 * for now, add sb in the wrapper
 */
  inline double cblas_sdot(const MKL_INT N, const float  *X, const MKL_INT incX,
                  const float  *Y, const MKL_INT incY)
  {
    return cblas_dsdot(N, X, incX, Y, incY);    
  }
   
  template <typename VECTOR_T>
  inline double cblas_sdot(const VECTOR_T& x, VECTOR_T& y, const double sb=0.0,
      const MKL_INT incX=1, const MKL_INT incY=1)
  {
    return cblas_sdot(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY) + sb;
  }

  // dot product of conjugate(x) and y, complex
  inline void cblas_dotc(const MKL_INT N, const MKL_Complex8* X, const MKL_INT incX, 
                         const MKL_Complex8* Y, const MKL_INT incY, MKL_Complex8& result)
  {
    cblas_cdotc_sub(N, static_cast<const void*>(X), incX, static_cast<const void*>(Y), incY, 
                   static_cast<void*>(&result));    
  }
  inline void cblas_dotc(const MKL_INT N, const MKL_Complex16* X, const MKL_INT incX, 
                         const MKL_Complex16* Y, const MKL_INT incY, MKL_Complex16& result)
  {
    cblas_zdotc_sub(N, static_cast<const void*>(X), incX, static_cast<const void*>(Y), incY,
                          static_cast<void*>(&result));    
  }
   
  template <typename VECTOR_T, typename RESULT_T>
  inline void cblas_dotc(const VECTOR_T& x, const VECTOR_T& y, RESULT_T& result, 
    const MKL_INT incX=1, const MKL_INT incY=1)
  {
    return cblas_dotc(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY, result);
  }
  // dot product of x and y, complex
  inline void cblas_dotu(const MKL_INT N, const MKL_Complex8* X, const MKL_INT incX, 
                         const MKL_Complex8* Y, const MKL_INT incY, MKL_Complex8& result)
  {
    cblas_cdotu_sub(N, static_cast<const void*>(X), incX, static_cast<const void*>(Y), incY, 
                   static_cast<void*>(&result));    
  }
  inline void cblas_dotu(const MKL_INT N, const MKL_Complex16* X, const MKL_INT incX, 
                         const MKL_Complex16* Y, const MKL_INT incY, MKL_Complex16& result)
  {
    cblas_zdotu_sub(N, static_cast<const void*>(X), incX, static_cast<const void*>(Y), incY,
                          static_cast<void*>(&result));    
  }
   
  template <typename VECTOR_T, typename RESULT_T>
  inline void cblas_dotu(const VECTOR_T& x, const VECTOR_T& y, RESULT_T& result, 
    const MKL_INT incX=1, const MKL_INT incY=1)
  {
    return cblas_dotu(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY, result);
  }

  // vector 2-norm
  inline double cblas_nrm2(const MKL_INT N, const float *X, const MKL_INT incX)
  {
    return cblas_snrm2(N, X, incX);
  }
  inline double cblas_nrm2(const MKL_INT N, const double *X, const MKL_INT incX)
  {
    return cblas_dnrm2(N, X, incX);
  }
  inline double cblas_nrm2(const MKL_INT N, const MKL_Complex8 *X, const MKL_INT incX)
  {
    return cblas_scnrm2(N, static_cast<const void*>(X), incX);
  }
  inline double cblas_nrm2(const MKL_INT N, const MKL_Complex16 *X, const MKL_INT incX)  
  {
    return cblas_dznrm2(N, static_cast<const void *>(X), incX);
  }
  template <typename VECTOR_T>
  inline double cblas_nrm2(const VECTOR_T& x, const MKL_INT incX=1)
  {
    return cblas_nrm2(x.size() / incX, ptr_to_first(x), incX);
  }

  // rot, rotate vectors
 
  inline void cblas_rot(const MKL_INT N, float* X, const MKL_INT incX,
                float *Y, const MKL_INT incY, const float c, const float s)
  {
    cblas_srot(N, X, incX, Y, incY, c, s);
  }

  inline void cblas_rot(const MKL_INT N, double* X, const MKL_INT incX,
                double *Y, const MKL_INT incY, const double c, const double s)
  {
    cblas_drot(N, X, incX, Y, incY, c, s);
  }

  inline void cblas_rot(const MKL_INT N, MKL_Complex8 *X, const MKL_INT incX,
                 MKL_Complex8 *Y, const MKL_INT incY, const float c, const float s)
  {
    cblas_csrot(N, static_cast<void*>(X), incX, 
              static_cast<void *>(Y), incY, c, s);
  }
  inline void cblas_rot(const MKL_INT N, MKL_Complex16 *X, const MKL_INT incX,
                 MKL_Complex16 *Y, const MKL_INT incY, const double c, const double s)
  {
    cblas_zdrot(N, static_cast<void*>(X), incX, 
              static_cast<void *>(Y), incY, c, s);
  }

  template <typename VECTOR_T>
  inline void cblas_rot(VECTOR_T& x, VECTOR_T& y, const double c, const double s,
                      const MKL_INT incX=1, const MKL_INT incY=1)
  {
    cblas_rot(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY, c, s);
  }
 
  //rotg, Givens rotation params calculation, docs are wrong on parameter types here. 
  inline void cblas_rotg(float& a, float& b, float& c, float& s)
  {
    cblas_srotg(&a, &b, &c, &s);
  }
  inline void cblas_rotg(double& a, double& b, double& c, double& s)
  {
    cblas_drotg(&a, &b, &c, &s);
  }
  inline void cblas_rotg(MKL_Complex8& a, MKL_Complex8& b, float& c, MKL_Complex8& s)
  {
    cblas_crotg(static_cast<void *>(&a), static_cast<const void *>(&b), &c, static_cast<void*>(&s));
  }
  inline void cblas_rotg(MKL_Complex16& a, MKL_Complex16& b, double& c, MKL_Complex16& s)
  {
    cblas_zrotg(static_cast<void *>(&a), static_cast<const void *>(&b), &c, static_cast<void*>(&s));
  }

  //TODO rotm
  //TODO rotmg
  
  //?scal: multiply vector by scalar
  inline void cblas_scal(const MKL_INT N, const float alpha, float *X, const MKL_INT incX)
  {
    cblas_sscal(N, alpha, X, incX);
  }
  inline void cblas_scal(const MKL_INT N, const double alpha, double *X, const MKL_INT incX)
  {
    cblas_dscal(N, alpha, X, incX);
  }
  inline void cblas_scal(const MKL_INT N, const MKL_Complex8& alpha, MKL_Complex8 *X, const MKL_INT incX)
  {
    cblas_cscal(N, static_cast<const void *>(&alpha), static_cast<void *>(X), incX);
  }
  inline void cblas_scal(const MKL_INT N, const MKL_Complex16& alpha, MKL_Complex16 *X, const MKL_INT incX)
  {
    cblas_zscal(N, static_cast<const void *>(&alpha), static_cast<void *>(X), incX);
  }
  inline void cblas_scal(const MKL_INT N, const float alpha, MKL_Complex8* X, const MKL_INT incX)
  {
    cblas_csscal(N, alpha, static_cast<void *>(X), incX);
  }
  inline void cblas_scal(const MKL_INT N, const double alpha, MKL_Complex16 *X, const MKL_INT incX)
  {
    cblas_zdscal(N, alpha, static_cast<void *>(X), incX);
  }
  template <typename VECTOR_T, typename SCALAR_T>
  inline void cblas_scal(VECTOR_T& x, const SCALAR_T& a, const MKL_INT incX=1)
  {
    cblas_scal(x.size()/incX, a, ptr_to_first(x), incX);
  }
  
  // swap: swap vectors
  
  inline void cblas_swap(const MKL_INT N, float *X, const MKL_INT incX,
                 float *Y, const MKL_INT incY)
  {
    cblas_sswap(N, X,incX, Y, incY);
  }
  inline void cblas_swap(const MKL_INT N, double *X, const MKL_INT incX,
                 double *Y, const MKL_INT incY)
  { 
    cblas_dswap(N, X, incX, Y, incY);
  }
  inline void cblas_swap(const MKL_INT N, MKL_Complex8 *X, const MKL_INT incX,
                 MKL_Complex8 *Y, const MKL_INT incY) 
  {
    cblas_cswap(N, static_cast<void *>(X), incX, static_cast<void *>(Y), incY);
  }
  inline void cblas_swap(const MKL_INT N, MKL_Complex16 *X, const MKL_INT incX,
                 MKL_Complex16 *Y, const MKL_INT incY) 
  {
    cblas_zswap(N, static_cast<void *>(X), incX, static_cast<void *>(Y), incY);
  }

  template<typename VECTOR_T>
  inline void cblas_swap(VECTOR_T& x, VECTOR_T& y, const MKL_INT incX=1, const MKL_INT incY=1)
  {
    cblas_swap(x.size()/incX, ptr_to_first(x), incX, ptr_to_first(y), incY);
  }

  //iamax: find max element in vector
  inline CBLAS_INDEX cblas_iamax(const MKL_INT N, const float  *X, const MKL_INT incX)
  {
    return cblas_isamax(N, X, incX);
  }
  inline CBLAS_INDEX cblas_iamax(const MKL_INT N, const double *X, const MKL_INT incX)
  { 
    return cblas_idamax(N, X, incX);
  }
  inline CBLAS_INDEX cblas_iamax(const MKL_INT N, const MKL_Complex8 *X, const MKL_INT incX)
  {
    return cblas_icamax(N, static_cast<const void *>(X), incX);
  }
  inline CBLAS_INDEX cblas_iamax(const MKL_INT N, const MKL_Complex16 *X, const MKL_INT incX)
  {
    return cblas_izamax(N, static_cast<const void*>(X), incX);
  }

  template<typename VECTOR_T>
  inline CBLAS_INDEX cblas_iamax(VECTOR_T& x, const MKL_INT incX=1)
  {
    return cblas_iamax(x.size()/incX, ptr_to_first(x), incX);
  } 
  //iamin: find min element in vector
  inline CBLAS_INDEX cblas_iamin(const MKL_INT N, const float  *X, const MKL_INT incX)
  {
    return cblas_isamin(N, X, incX);
  }
  inline CBLAS_INDEX cblas_iamin(const MKL_INT N, const double *X, const MKL_INT incX)
  { 
    return cblas_idamin(N, X, incX);
  }
  inline CBLAS_INDEX cblas_iamin(const MKL_INT N, const MKL_Complex8 *X, const MKL_INT incX)
  {
    return cblas_icamin(N, static_cast<const void *>(X), incX);
  }
  inline CBLAS_INDEX cblas_iamin(const MKL_INT N, const MKL_Complex16 *X, const MKL_INT incX)
  {
    return cblas_izamin(N, static_cast<const void*>(X), incX);
  }

  template<typename VECTOR_T>
  inline CBLAS_INDEX cblas_iamin(VECTOR_T& x, const MKL_INT incX=1)
  {
    return cblas_iamin(x.size()/incX, ptr_to_first(x), incX);
  } 

  inline double cblas_cabs1(const MKL_Complex16& z)
  {
    return cblas_dcabs1(static_cast<const void*>(&z));
  }

  /**
 * CBLAS BLAS Level 2 wrappers
 */
 
  //gemv Matrix vector product
  inline void cblas_gemv(const  CBLAS_ORDER order,
                 const  CBLAS_TRANSPOSE TransA, const MKL_INT M, const MKL_INT N,
                 const double alpha, const float *A, const MKL_INT lda,
                 const float *X, const MKL_INT incX, const double beta,
                 float *Y, const MKL_INT incY)
  {
    cblas_sgemv(order, TransA, M, N, alpha, A, lda, X, incX, beta, Y, incY);
  }
  inline void cblas_gemv(const  CBLAS_ORDER order,
                 const  CBLAS_TRANSPOSE TransA, const MKL_INT M, const MKL_INT N,
                 const double alpha, const double *A, const MKL_INT lda,
                 const double *X, const MKL_INT incX, const double beta,
                 double *Y, const MKL_INT incY)
  {
    cblas_dgemv(order, TransA, M, N, alpha, A, lda, X, incX, beta, Y, incY);
  }
  inline void cblas_gemv(const  CBLAS_ORDER order,
                 const  CBLAS_TRANSPOSE TransA, const MKL_INT M, const MKL_INT N,
                 const MKL_Complex8& alpha, const MKL_Complex8 *A, const MKL_INT lda,
                 const MKL_Complex8 *X, const MKL_INT incX, const MKL_Complex8& beta,
                 MKL_Complex8 *Y, const MKL_INT incY)
  {
    cblas_cgemv(order, TransA, M, N, static_cast<const void*>(&alpha), static_cast<const void*>(A), lda, 
                static_cast<const void*>(X), incX, static_cast<const void*>(&beta), static_cast<void*>(Y), incY);
  }
  inline void cblas_gemv(const  CBLAS_ORDER order,
                 const  CBLAS_TRANSPOSE TransA, const MKL_INT M, const MKL_INT N,
                 const MKL_Complex16& alpha, const MKL_Complex16 *A, const MKL_INT lda,
                 const MKL_Complex16 *X, const MKL_INT incX, const MKL_Complex16& beta,
                 MKL_Complex16 *Y, const MKL_INT incY)
  {
    cblas_zgemv(order, TransA, M, N, static_cast<const void*>(&alpha), static_cast<const void*>(A), lda, 
                static_cast<const void*>(X), incX, static_cast<const void*>(&beta), static_cast<void*>(Y), incY);
  }
  
  template<typename MATRIX_T, typename VECTOR_T, typename SCALAR_T>
  inline void cblas_gemv(const MATRIX_T& A, const VECTOR_T& x, VECTOR_T& y,
                   const SCALAR_T& alpha, 
                   const SCALAR_T& beta, 
                   const CBLAS_TRANSPOSE TransA=CblasNoTrans,
                   const MKL_INT incX=1, const MKL_INT incY=1)
  {
    CBLAS_ORDER order = CblasRowMajor;
    bool doTransA = TransA != CblasNoTrans;
    const size_t opA_row_count = doTransA == false ? A.size1() : A.size2();//rows of op(A)
    const size_t opA_col_count = doTransA == false ? A.size2() : A.size1();//cols of op(A)
    const MKL_INT M = A.size1();
    const MKL_INT N = A.size2();
    const MKL_INT lda = doTransA == false ? opA_col_count : opA_row_count;
    cblas_gemv(order, TransA, M, N, alpha, ptr_to_first(A), lda, ptr_to_first(x), incX, beta, ptr_to_first(y), incY); 
  }
  //ger rank 1 update of a general matrix
  //gerc rank 1 update of a conjugated general matrix


  /** cppmkl wrapper for cblas_dgemm, BLAS level 3 matrix multiplication
   * MATRIX_T can be any row-major matrix type that has functions size1(), size2() 
   * giving row count and column count respectively 
   */
  inline void cblas_gemm(const  CBLAS_ORDER Order, const  CBLAS_TRANSPOSE TransA,
                 const  CBLAS_TRANSPOSE TransB, const MKL_INT M, const MKL_INT N,
                 const MKL_INT K, const double alpha, const double *A,
                 const MKL_INT lda, const double *B, const MKL_INT ldb,
                 const double beta, double *C, const MKL_INT ldc)
  {
    cblas_dgemm(Order, TransA, TransB, M, N,K, alpha, A, lda, B, ldb,
                beta, C, ldc);
  }
  inline void cblas_gemm(const  CBLAS_ORDER Order, const  CBLAS_TRANSPOSE TransA,
                 const  CBLAS_TRANSPOSE TransB, const MKL_INT M, const MKL_INT N,
                 const MKL_INT K, const float alpha, const float *A,
                 const MKL_INT lda, const float *B, const MKL_INT ldb,
                 const float beta, float *C, const MKL_INT ldc)
  {
    cblas_sgemm(Order, TransA, TransB, M, N,K, alpha, A, lda, B, ldb,
                beta, C, ldc);
  }
  inline void cblas_gemm(const  CBLAS_ORDER Order, const  CBLAS_TRANSPOSE TransA,
                 const  CBLAS_TRANSPOSE TransB, const MKL_INT M, const MKL_INT N,
                 const MKL_INT K, const MKL_Complex8 alpha, const MKL_Complex8 *A,
                 const MKL_INT lda, const MKL_Complex8 *B, const MKL_INT ldb,
                 const MKL_Complex8 beta, MKL_Complex8 *C, const MKL_INT ldc)
  {
    cblas_cgemm(Order, TransA, TransB, M, N,K, 
        static_cast<const void*>(&alpha), 
        static_cast<const void*>(A), lda, 
        static_cast<const void*>(B), ldb,
        static_cast<const void*>(&beta), 
        static_cast<void*>(C), ldc);
  }
  inline void cblas_gemm(const  CBLAS_ORDER Order, const  CBLAS_TRANSPOSE TransA,
                 const  CBLAS_TRANSPOSE TransB, const MKL_INT M, const MKL_INT N,
                 const MKL_INT K, const MKL_Complex16 alpha, const MKL_Complex16 *A,
                 const MKL_INT lda, const MKL_Complex16 *B, const MKL_INT ldb,
                 const MKL_Complex16 beta, MKL_Complex16 *C, const MKL_INT ldc)
  {
    cblas_zgemm(Order, TransA, TransB, M, N,K, 
        static_cast<const void*>(&alpha), 
        static_cast<const void*>(A), lda, 
        static_cast<const void*>(B), ldb,
        static_cast<const void*>(&beta), 
        static_cast<void*>(C), ldc);
  }
  template <typename MATRIX_T>
    inline void cblas_gemm(const MATRIX_T& A, const MATRIX_T& B, MATRIX_T& C,
        const CBLAS_TRANSPOSE TransA=CblasNoTrans, const CBLAS_TRANSPOSE TransB=CblasNoTrans,
        const typename MATRIX_T::value_type alpha=_alpha_default<typename MATRIX_T::value_type>(), 
        const typename MATRIX_T::value_type beta=_beta_default<typename MATRIX_T::value_type>())
    {
      CBLAS_ORDER order = CblasRowMajor;
      bool doTransA = TransA != CblasNoTrans;
      bool doTransB = TransB != CblasNoTrans;
      const size_t opA_row_count = doTransA == false ? A.size1() : A.size2();//rows of op(A)
      const size_t opB_col_count = doTransB == false ? B.size2() : B.size1();//cols of op(B) 
      const size_t opA_col_count = doTransA == false ? A.size2() : A.size1();//cols of op(A)
      //const size_t opB_row_count = doTransB == false ? B.size1() : B.size2(); //rows of op(B)
      const MKL_INT lda = doTransA == false ? opA_col_count : opA_row_count;
      const MKL_INT ldb = doTransB == false ? opB_col_count : opA_col_count;
      const MKL_INT ldc = opB_col_count; 
      //opB_row_count;
      //assert(opA_col_count == opB_row_count);
      assert(C.size1() == opA_row_count);
      assert(C.size2() == opB_col_count);
      // call the appropriate overloaded cblas_gemm function based on the type of X
      cblas_gemm(order, TransA, TransB, opA_row_count, opB_col_count, opA_col_count, alpha, ptr_to_first(A), lda, ptr_to_first(B), ldb, beta, ptr_to_first(C), ldc);     
    }

}

#endif
