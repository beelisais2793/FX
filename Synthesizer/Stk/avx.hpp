#ifndef AVX_HPP
#define AVX_HPP

#include <functional>
#include <immintrin.h>
#include <iostream>

namespace simd {
  // AVX float
  struct alignas(32) float8 {

      float8()
          : xmm(_mm256_setzero_ps()) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      float8(T v)
        : xmm(_mm256_set1_ps(static_cast<T>(v))) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      float8(const T v1, const T v2, const T v3, const T v4,
             const T v5, const T v6, const T v7, const T v8)
        : xmm(_mm256_setr_ps(v1, v2, v3, v4, v5, v6, v7, v8)) {
      }

      float8(const __m256& datas)
        : xmm(datas) {
      }

      void loadAligned(const float* datas) {
        xmm = _mm256_load_ps(datas);
      }

      void loadUnaligned(const float* datas) {
        xmm = _mm256_loadu_ps(datas);
      }

      void storeAligned(float* datas) {
        _mm256_store_ps(datas, xmm);
      }

      void storeUnaligned(float* datas) {
        _mm256_storeu_ps(datas, xmm);
      }

      const float operator[](const std::size_t& index) const {
        return xmm[index];
      }

      float operator[](const std::size_t& index) {
        return xmm[index];
      }

      const __m256& get() const {
        return xmm;
      }

      friend std::ostream& operator<<(std::ostream& os, const float8& v) {
        os << "[ ";
        for (std::size_t i = 0; i < v.SZ; ++i) {
          os << v.xmm[i];
          os << (i < v.SZ - 1 ? ", " : " ]");
        }
        return os;
      }

      void operator++(const int i) {
        xmm = _mm256_add_ps(xmm, _mm256_set1_ps(1));
      }

      void operator--(const int i) {
        xmm = _mm256_sub_ps(xmm, _mm256_set1_ps(1));
      }

      float8& operator=(const __m256& m256) {
        xmm = m256;
        return *this;
      }

      float8& operator=(const float8& v) {
        xmm = v.xmm;
        return *this;
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      void set(const T v1, const T v2, const T v3, const T v4,
               const T v5, const T v6, const T v7, const T v8) {
        xmm = _mm256_set_ps(v1, v2, v3, v4, v5, v6, v7, v8);
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      void setReversed(const T v1, const T v2, const T v3, const T v4,
                       const T v5, const T v6, const T v7, const T v8) {
        xmm = _mm256_setr_ps(v1, v2, v3, v4, v5, v6, v7, v8);
      }

      float8 operator+(const float8& v) {
        return _mm256_add_ps(xmm, v.xmm);
      }

      template<typename T>
        float8 operator+(const T v) {
          return float8(_mm256_add_ps(xmm, _mm256_set1_ps(static_cast<float>(v))));
        }

      float8 operator+(const __m256& m256) {
        return _mm256_add_ps(xmm, m256);
      }

      void operator+=(const float8& v) {
        xmm = _mm256_add_ps(xmm, v.xmm);
      }

      void operator+=(const __m256& m256) {
        xmm = _mm256_add_ps(xmm, m256);
      }

      template<typename T>
        void operator+=(const T v) {
          xmm = _mm256_add_ps(xmm, _mm256_set1_ps(static_cast<float>(v)));
        }

      float8 operator-(const float8& v) {
        return _mm256_sub_ps(xmm, v.xmm);
      }

      float8 operator-(const __m256& m256) {
        return _mm256_sub_ps(xmm, m256);
      }

      template<typename T>
        float8 operator-(const T v) {
          return float8(
              _mm256_sub_ps(xmm, _mm256_set1_ps(static_cast<float>(v))));
        }

      void operator-=(const float8& v) {
        xmm = _mm256_sub_ps(xmm, v.xmm);
      }

      void operator-=(const __m256& m256) {
        xmm = _mm256_sub_ps(xmm, m256);
      }

      template<typename T>
        void operator-=(const T v) {
          xmm = _mm256_sub_ps(xmm, _mm256_set1_ps(static_cast<float>(v)));
        }

      float8 operator*(const float8& v) {
        return _mm256_mul_ps(xmm, v.xmm);
      }

      float8 operator*(const __m256& m256) {
        return _mm256_mul_ps(xmm, m256);
      }

      template<typename T>
        float8 operator*(const T v) {
          return float8(
              _mm256_mul_ps(xmm, _mm256_set1_ps(static_cast<float>(v))));
        }

      void operator*=(const float8& v) {
        xmm = _mm256_mul_ps(xmm, v.xmm);
      }

      void operator*=(const __m256& m256) {
        xmm = _mm256_mul_ps(xmm, m256);
      }

      template<typename T>
        void operator*=(const T v) {
          xmm = _mm256_mul_ps(xmm, _mm256_set1_ps(static_cast<float>(v)));
        }

      float8 operator/(const float8& v) {
        return _mm256_div_ps(xmm, v.xmm);
      }

      float8 operator/(const __m256& m256) {
        return _mm256_div_ps(xmm, m256);
      }

      template<typename T>
        float8 operator/(const T v) {
          return float8(
              _mm256_div_ps(xmm, _mm256_set1_ps(static_cast<float>(v))));
        }

      void operator/=(const float8& v) {
        xmm = _mm256_div_ps(xmm, v.xmm);
      }

      void operator/=(const __m256& m256) {
        xmm = _mm256_div_ps(xmm, m256);
      }

      template<typename T>
        void operator/=(const T v) {
          xmm = _mm256_div_ps(xmm, _mm256_set1_ps(static_cast<float>(v)));
        }

      bool operator==(const float8& v) {
        return cmpAsBool(v, _CMP_EQ_OQ);
      }

      float8 equal(float8& v) {
        return cmp(v, _CMP_EQ_OQ);
      }

      bool operator!=(const float8& v) {
        return cmpAsBool(v, _CMP_NEQ_OQ);
      }

      float8 notEqual(float8& v) {
        return cmp(v, _CMP_NEQ_OQ);
      }

      bool operator>(const float8& v) {
        return cmpAsBool(v, _CMP_GT_OQ);
      }

      float8 greater(float8& v) {
        return cmp(v, _CMP_GT_OQ);
      }

      bool operator<(const float8& v) {
        return cmpAsBool(v, _CMP_LT_OQ) > 0;
      }

      float8 lesser(float8& v) {
        return cmp(v, _CMP_LT_OQ);
      }

      bool operator>=(const float8& v) {
        return cmpAsBool(v, _CMP_GE_OQ);
      }

      float8 greaterEqual(float8& v) {
        return cmp(v, _CMP_GE_OQ);
      }

      bool operator<=(const float8& v) {
        return cmpAsBool(v, _CMP_LE_OQ);
      }

      float8 lesserEqual(float8& v) {
        return cmp(v, _CMP_LE_OQ);
      }

      float8 operator&(const float8& v) {
        return _mm256_and_ps(xmm, v.xmm);
      }

      const std::size_t size() const {
        return SZ;
      }

    private:
      static const std::size_t SZ = 8;
      __m256 xmm;

      bool cmpAsBool(const float8& v, const int comparator) {
        return _mm256_movemask_ps(_mm256_cmp_ps(xmm, v.xmm, comparator)) > 0;
      }

      float8 cmp(const float8& v, const int comparator) {
        return _mm256_cmp_ps(xmm, v.xmm, comparator);
      }
  };

  // AVX double
  struct alignas(32) double4 {

      double4()
        : xmm(_mm256_setzero_pd()) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      double4(T v)
        : xmm(_mm256_set1_pd(v)) {
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      double4(const T v1, const T v2, const T v3, const T v4)
        : xmm(_mm256_setr_pd(v1, v2, v3, v4)) {
      }

      double4(const __m256d& datas)
          : xmm(datas) {
      }

      void loadAligned(const double* datas) {
        xmm = _mm256_load_pd(datas);
      }

      void loadUnaligned(const double* datas) {
        xmm = _mm256_loadu_pd(datas);
      }

      void storeAligned(double* datas) {
        _mm256_store_pd(datas, xmm);
      }

      void storeUnaligned(double* datas) {
        _mm256_storeu_pd(datas, xmm);
      }

      const double operator[](const std::size_t& index) const {
        return xmm[index];
      }

      double operator[](const std::size_t& index) {
        return xmm[index];
      }

      const __m256d& get() const {
        return xmm;
      }

      friend std::ostream& operator<<(std::ostream& os, const double4& v) {
        os << "[ ";
        for (std::size_t i = 0; i < v.SZ; ++i) {
          os << v.xmm[i];
          os << (i < v.SZ - 1 ? ", " : " ]");
        }
        return os;
      }

      void operator++(const int i) {
        xmm = _mm256_add_pd(xmm, _mm256_set1_pd(1));
      }

      void operator--(const int i) {
        xmm = _mm256_sub_pd(xmm, _mm256_set1_pd(1));
      }

      double4& operator=(const __m256d& m256) {
        xmm = m256;
        return *this;
      }

      double4& operator=(const double4& v) {
        xmm = v.xmm;
        return *this;
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      void set(const T v1, const T v2, const T v3, const T v4) {
        xmm = _mm256_set_pd(v1, v2, v3, v4);
      }

      template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
      void setReversed(const T v1, const T v2, const T v3, const T v4) {
        xmm = _mm256_setr_pd(v1, v2, v3, v4);
      }

      double4 operator+(const double4& v) {
        return _mm256_add_pd(xmm, v.xmm);
      }

      template<typename T>
        double4 operator+(const T v) {
          return double4(
              _mm256_add_pd(xmm, _mm256_set1_pd(static_cast<double>(v))));
        }

      double4 operator+(const __m256d& m256) {
        return _mm256_add_pd(xmm, m256);
      }

      void operator+=(const double4& v) {
        xmm = _mm256_add_pd(xmm, v.xmm);
      }

      void operator+=(const __m256d& m256) {
        xmm = _mm256_add_pd(xmm, m256);
      }

      template<typename T>
        void operator+=(const T v) {
          xmm = _mm256_add_pd(xmm, _mm256_set1_pd(static_cast<double>(v)));
        }

      double4 operator-(const double4& v) {
        return _mm256_sub_pd(xmm, v.xmm);
      }

      double4 operator-(const __m256d& m256) {
        return _mm256_sub_pd(xmm, m256);
      }

      template<typename T>
        double4 operator-(const T v) {
          return double4(
              _mm256_sub_pd(xmm, _mm256_set1_pd(static_cast<double>(v))));
        }

      void operator-=(const double4& v) {
        xmm = _mm256_sub_pd(xmm, v.xmm);
      }

      void operator-=(const __m256d& m256) {
        xmm = _mm256_sub_pd(xmm, m256);
      }

      template<typename T>
        void operator-=(const T v) {
          xmm = _mm256_sub_pd(xmm, _mm256_set1_pd(static_cast<double>(v)));
        }

      double4 operator*(const double4& v) {
        return _mm256_mul_pd(xmm, v.xmm);
      }

      double4 operator*(const __m256d& m256) {
        return _mm256_mul_pd(xmm, m256);
      }

      template<typename T>
        double4 operator*(const T v) {
          return double4(
              _mm256_mul_pd(xmm, _mm256_set1_pd(static_cast<double>(v))));
        }

      void operator*=(const double4& v) {
        xmm = _mm256_mul_pd(xmm, v.xmm);
      }

      void operator*=(const __m256d& m256) {
        xmm = _mm256_mul_pd(xmm, m256);
      }

      template<typename T>
        void operator*=(const T v) {
          xmm = _mm256_mul_pd(xmm, _mm256_set1_pd(static_cast<double>(v)));
        }

      double4 operator/(const double4& v) {
        return _mm256_div_pd(xmm, v.xmm);
      }

      double4 operator/(const __m256d& m256) {
        return _mm256_div_pd(xmm, m256);
      }

      template<typename T>
        double4 operator/(const T v) {
          return double4(
              _mm256_div_pd(xmm, _mm256_set1_pd(static_cast<double>(v))));
        }

      void operator/=(const double4& v) {
        xmm = _mm256_div_pd(xmm, v.xmm);
      }

      void operator/=(const __m256d& m256) {
        xmm = _mm256_div_pd(xmm, m256);
      }

      template<typename T>
        void operator/=(const T v) {
          xmm = _mm256_div_pd(xmm, _mm256_set1_pd(static_cast<double>(v)));
        }

      bool operator==(const double4& v) {
        return cmpAsBool(v, _CMP_EQ_OQ);
      }

      double4 equal(double4& v) {
        return cmp(v, _CMP_EQ_OQ);
      }

      bool operator!=(const double4& v) {
        return cmpAsBool(v, _CMP_NEQ_OQ);
      }

      double4 notEqual(double4& v) {
        return cmp(v, _CMP_NEQ_OQ);
      }

      bool operator>(const double4& v) {
        return cmpAsBool(v, _CMP_GT_OQ);
      }

      double4 greater(double4& v) {
        return cmp(v, _CMP_GT_OQ);
      }

      bool operator<(const double4& v) {
        return cmpAsBool(v, _CMP_LT_OQ) > 0;
      }

      double4 lesser(double4& v) {
        return cmp(v, _CMP_LT_OQ);
      }

      bool operator>=(const double4& v) {
        return cmpAsBool(v, _CMP_GE_OQ);
      }

      double4 greaterEqual(double4& v) {
        return cmp(v, _CMP_GE_OQ);
      }

      bool operator<=(const double4& v) {
        return cmpAsBool(v, _CMP_LE_OQ);
      }

      double4 lesserEqual(double4& v) {
        return cmp(v, _CMP_LE_OQ);
      }

      double4 operator&(const double4& v) {
        return _mm256_and_pd(xmm, v.xmm);
      }

      const std::size_t size() const {
        return SZ;
      }

    private:
      static const std::size_t SZ = 4;
      __m256d xmm;

      bool cmpAsBool(const double4& v, const uint32_t comparator) {
        return _mm256_movemask_pd(_mm256_cmp_pd(xmm, v.xmm, comparator)) > 0;
      }

      double4 cmp(const double4& v, const uint32_t comparator) {
        return _mm256_cmp_pd(xmm, v.xmm, comparator);
      }
  };

  namespace avx {

    float8 sqrt(const float8& v) {
      return _mm256_sqrt_ps(v.get());
    }

    double4 sqrt(const double4& v) {
      return _mm256_sqrt_pd(v.get());
    }

  }

}

#endif // AVX_HPP