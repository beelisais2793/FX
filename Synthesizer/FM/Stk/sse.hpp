#ifndef SSE_HPP
#define SSE_HPP

#include <emmintrin.h>
#include <iostream>
#include <array>

namespace simd {
// SSE

  struct alignas(16) float4 {

      float4()
        : xmm(_mm_setzero_ps()) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      float4(const T v)
        : xmm(_mm_set1_ps(v)) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      float4(const T v1, const T v2, const T v3, const T v4)
        : xmm(_mm_setr_ps(v1, v2, v3, v4)) {
      }

      float4(const __m128& datas)
        : xmm(datas) {
      }

      void loadAligned(const float* datas) {
        xmm = _mm_load_ps(datas);
      }

      void loadUnaligned(const float* datas) {
        xmm = _mm_loadu_ps(datas);
      }

      void storeAligned(float* datas) {
        _mm_store_ps(datas, xmm);
      }

      void storeUnaligned(float* datas) {
        _mm_storeu_ps(datas, xmm);
      }

      const float operator[](const std::size_t& index) const {
        return xmm[index];
      }

      float operator[](const std::size_t& index) {
        return xmm[index];
      }

      const __m128& get() const {
        return xmm;
      }

      friend std::ostream& operator<<(std::ostream& os, const float4& v) {
        os << "[ ";
        for (std::size_t i = 0; i < v.SZ; ++i) {
          os << v.xmm[i];
          os << (i < v.SZ - 1 ? ", " : " ]");
        }
        return os;
      }

      void operator++(const int i) {
        xmm = _mm_add_ps(xmm, _mm_set1_ps(1));
      }

      void operator--(const int i) {
        xmm = _mm_sub_ps(xmm, _mm_set1_ps(1));
      }

      float4& operator=(const __m128& m128) {
        xmm = m128;
        return *this;
      }

      float4& operator=(const float4& v) {
        xmm = v.xmm;
        return *this;
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      void set(const T v1, const T v2, const T v3, const T v4) {
        xmm = _mm_set_ps(v1, v2, v3, v4);
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      void setReversed(const T v1, const T v2, const T v3, const T v4) {
        xmm = _mm_setr_ps(v1, v2, v3, v4);
      }

      float4 operator+(const float4& v) {
        return _mm_add_ps(xmm, v.xmm);
      }

      template<typename T>
        float4 operator+(const T v) {
          return float4(_mm_add_ps(xmm, _mm_set1_ps(static_cast<float>(v))));
        }

      float4 operator+(const __m128& m128) {
        return _mm_add_ps(xmm, m128);
      }

      void operator+=(const float4& v) {
        xmm = _mm_add_ps(xmm, v.xmm);
      }

      void operator+=(const __m128& m128) {
        xmm = _mm_add_ps(xmm, m128);
      }

      template<typename T>
        void operator+=(const T v) {
          xmm = _mm_add_ps(xmm, _mm_set1_ps(static_cast<float>(v)));
        }

      float4 operator-(const float4& v) {
        return _mm_sub_ps(xmm, v.xmm);
      }

      float4 operator-(const __m128& m128) {
        return _mm_sub_ps(xmm, m128);
      }

      template<typename T>
        float4 operator-(const T v) {
          return float4(_mm_sub_ps(xmm, _mm_set1_ps(static_cast<float>(v))));
        }

      void operator-=(const float4& v) {
        xmm = _mm_sub_ps(xmm, v.xmm);
      }

      void operator-=(const __m128& m128) {
        xmm = _mm_sub_ps(xmm, m128);
      }

      template<typename T>
        void operator-=(const T v) {
          xmm = _mm_sub_ps(xmm, _mm_set1_ps(static_cast<float>(v)));
        }

      float4 operator*(const float4& v) {
        return _mm_mul_ps(xmm, v.xmm);
      }

      float4 operator*(const __m128& m128) {
        return _mm_mul_ps(xmm, m128);
      }

      template<typename T>
        float4 operator*(const T v) {
          return float4(_mm_mul_ps(xmm, _mm_set1_ps(static_cast<float>(v))));
        }

      void operator*=(const float4& v) {
        xmm = _mm_mul_ps(xmm, v.xmm);
      }

      void operator*=(const __m128& m128) {
        xmm = _mm_mul_ps(xmm, m128);
      }

      template<typename T>
        void operator*=(const T v) {
          xmm = _mm_mul_ps(xmm, _mm_set1_ps(static_cast<float>(v)));
        }

      float4 operator/(const float4& v) {
        return _mm_div_ps(xmm, v.xmm);
      }

      float4 operator/(const __m128& m128) {
        return _mm_div_ps(xmm, m128);
      }

      template<typename T>
        float4 operator/(const T v) {
          return float4(_mm_div_ps(xmm, _mm_set1_ps(static_cast<float>(v))));
        }

      void operator/=(const float4& v) {
        xmm = _mm_div_ps(xmm, v.xmm);
      }

      void operator/=(const __m128& m128) {
        xmm = _mm_div_ps(xmm, m128);
      }

      template<typename T>
        void operator/=(const T v) {
          xmm = _mm_div_ps(xmm, _mm_set1_ps(static_cast<float>(v)));
        }

      bool operator==(const float4& v) {
        return _mm_comieq_ss(xmm, v.xmm) > 0;
      }

      float4 equal(const float4& v) {
        return _mm_cmpeq_ps(xmm, v.xmm);
      }

      bool operator!=(const float4& v) {
        return _mm_comineq_ss(xmm, v.xmm) > 0;
      }

      float4 notEqual(const float4& v) {
        return _mm_cmpneq_ps(xmm, v.xmm);
      }

      bool operator>(const float4& v) {
        return _mm_comigt_ss(xmm, v.xmm) > 0;
      }

      float4 greater(const float4& v) {
        return _mm_cmpgt_ps(xmm, v.xmm);
      }

      bool operator<(const float4& v) {
        return _mm_comilt_ss(xmm, v.xmm) > 0;
      }

      float4 lesser(const float4& v) {
        return _mm_cmplt_ps(xmm, v.xmm);
      }

      bool operator>=(const float4& v) {
        return _mm_comige_ss(xmm, v.xmm) > 0;
      }

      float4 greaterEqual(const float4& v) {
        return _mm_cmpge_ps(xmm, v.xmm);
      }

      bool operator<=(const float4& v) {
        return _mm_comile_ss(xmm, v.xmm) > 0;
      }

      float4 lesserEqual(const float4& v) {
        return _mm_cmple_ps(xmm, v.xmm);
      }

      float4 operator&(const float4& v) {
        return _mm_and_ps(xmm, v.xmm);
      }

      const std::size_t size() const {
        return SZ;
      }

    private:
      static const std::size_t SZ = 4;
      __m128 xmm;
  };

  // SSE2
  struct alignas(16) double2 {

      double2()
          : xmm(_mm_setzero_pd()) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      double2(const T v)
        : xmm(_mm_set1_pd(v)) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      double2(const T v1, const T v2)
        : xmm(_mm_setr_pd(v1, v2)) {
      }

      double2(const __m128d& datas)
        : xmm(datas) {
      }

      double2(const double* datas) {
        loadUnaligned(datas);
      }

      void loadAligned(const double* datas) {
        xmm = _mm_load_pd(datas);
      }

      void loadUnaligned(const double* datas) {
        xmm = _mm_loadu_pd(datas);
      }

      void storeAligned(double* datas) {
        _mm_store_pd(datas, xmm);
      }

      void storeUnaligned(double* datas) {
        _mm_storeu_pd(datas, xmm);
      }

      const double operator[](const std::size_t& index) const {
        return xmm[index];
      }

      double operator[](const std::size_t& index) {
        return xmm[index];
      }

      const __m128d& get() const {
        return xmm;
      }

      friend std::ostream& operator<<(std::ostream& os, const double2& v) {
        os << "[ ";
        for (std::size_t i = 0; i < v.SZ; ++i) {
          os << v.xmm[i];
          os << (i < v.SZ - 1 ? ", " : " ]");
        }
        return os;
      }

      void operator++(const int i) {
        xmm = _mm_add_pd(xmm, _mm_set1_pd(1));
      }

      void operator--(const int i) {
        xmm = _mm_sub_pd(xmm, _mm_set1_pd(1));
      }

      double2& operator=(const __m128d& m128d) {
        xmm = m128d;
        return *this;
      }

      double2& operator=(const double2& v) {
        xmm = v.xmm;
        return *this;
      }

      void set(const double v1, const double v2) {
        xmm = _mm_set_pd(v1, v2);
      }

      void setReversed(const double v1, const double v2) {
        xmm = _mm_setr_pd(v1, v2);
      }

      double2 operator+(const double2& v) {
        return _mm_add_pd(xmm, v.xmm);
      }

      template<typename T>
        double2 operator+(const T v) {
          return double2(_mm_add_pd(xmm, _mm_set1_pd(static_cast<double>(v))));
        }

      double2 operator+(const __m128d& m128d) {
        return _mm_add_pd(xmm, m128d);
      }

      void operator+=(const double2& v) {
        xmm = _mm_add_pd(xmm, v.xmm);
      }

      void operator+=(const __m128d& m128d) {
        xmm = _mm_add_pd(xmm, m128d);
      }

      template<typename T>
        void operator+=(const T v) {
          xmm = _mm_add_pd(xmm, _mm_set1_pd(static_cast<double>(v)));
        }

      double2 operator-(const double2& v) {
        return _mm_sub_pd(xmm, v.xmm);
      }

      double2 operator-(const __m128d& m128d) {
        return _mm_sub_pd(xmm, m128d);
      }

      template<typename T>
        double2 operator-(const T v) {
          return double2(_mm_sub_pd(xmm, _mm_set1_pd(static_cast<double>(v))));
        }

      void operator-=(const double2& v) {
        xmm = _mm_sub_pd(xmm, v.xmm);
      }

      void operator-=(const __m128d& m128d) {
        xmm = _mm_sub_pd(xmm, m128d);
      }

      template<typename T>
        void operator-=(const T v) {
          xmm = _mm_sub_pd(xmm, _mm_set1_pd(static_cast<double>(v)));
        }

      double2 operator*(const double2& v) {
        return _mm_mul_pd(xmm, v.xmm);
      }

      double2 operator*(const __m128d& m128d) {
        return _mm_mul_pd(xmm, m128d);
      }

      template<typename T>
        double2 operator*(const T v) {
          return double2(_mm_mul_pd(xmm, _mm_set1_pd(static_cast<double>(v))));
        }

      void operator*=(const double2& v) {
        xmm = _mm_mul_pd(xmm, v.xmm);
      }

      void operator*=(const __m128d& m128d) {
        xmm = _mm_mul_pd(xmm, m128d);
      }

      template<typename T>
        void operator*=(const T v) {
          xmm = _mm_mul_pd(xmm, _mm_set1_pd(static_cast<double>(v)));
        }

      double2 operator/(const double2& v) {
        return _mm_div_pd(xmm, v.xmm);
      }

      double2 operator/(const __m128d& m128d) {
        return _mm_div_pd(xmm, m128d);
      }

      template<typename T>
        double2 operator/(const T v) {
          return double2(_mm_div_pd(xmm, _mm_set1_pd(static_cast<double>(v))));
        }

      void operator/=(const double2& v) {
        xmm = _mm_div_pd(xmm, v.xmm);
      }

      void operator/=(const __m128d& m128d) {
        xmm = _mm_div_pd(xmm, m128d);
      }

      template<typename T>
        void operator/=(const T v) {
          xmm = _mm_div_pd(xmm, _mm_set1_pd(static_cast<double>(v)));
        }

      bool operator==(const double2& v) {
        return _mm_comieq_sd(xmm, v.xmm) > 0;
      }

      double2 equal(double2& v) {
        return _mm_cmpeq_pd(xmm, v.xmm);
      }

      bool operator!=(const double2& v) {
        return _mm_comineq_sd(xmm, v.xmm) > 0;
      }

      double2 notEqual(const double2& v) {
        return _mm_cmpneq_pd(xmm, v.xmm);
      }

      bool operator>(const double2& v) {
        return _mm_comigt_sd(xmm, v.xmm) > 0;
      }

      double2 greater(const double2& v) {
        return _mm_cmpgt_pd(xmm, v.xmm);
      }

      bool operator<(const double2& v) {
        return _mm_comilt_sd(xmm, v.xmm) > 0;
      }

      double2 lesser(const double2& v) {
        return _mm_cmplt_pd(xmm, v.xmm);
      }

      bool operator>=(const double2& v) {
        return _mm_comige_sd(xmm, v.xmm) > 0;
      }

      double2 greaterEqual(const double2& v) {
        return _mm_cmpge_pd(xmm, v.xmm);
      }

      bool operator<=(const double2& v) {
        return _mm_comile_sd(xmm, v.xmm) > 0;
      }

      double2 lesserEqual(const double2& v) {
        return _mm_cmple_pd(xmm, v.xmm);
      }

      const std::size_t size() const {
        return SZ;
      }

    private:
      static const std::size_t SZ = 2;
      __m128d xmm;
  };

  namespace sse {

    float4 sqrt(const float4& v) {
      return _mm_sqrt_ps(v.get());
    }

    double2 sqrt(const double2& v) {
      return _mm_sqrt_pd(v.get());
    }

  }

}

#endif // SSE_HPP