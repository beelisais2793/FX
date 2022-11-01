#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>
#include <complex>

#include "Kfr/kfrcore.hpp"
#include "AudioFFT/AudioFFT.h"
#include "AudioFFT/FFTConvolver.h"

namespace SoundWave
{    
    struct matrix_index {
        size_t i;
        size_t j;
    };

    template<typename T>
    struct vector_matrix : public std::vector<T>
    {
        size_t M,N;
        vector_matrix(size_t m, size_t n) {
            M = m;
            N = n;
            resize(M,N);
        }

        size_t size() const;
        bool   empty() const;

        void   resize(size_t i, size_t j) { this->resize(i*j); M=i; N=j; }
        size_t rows() const { return M; }
        size_t cols() const { return N; }

        T& operator()(size_t i, size_t j) { return (*this)[i*N + j]; }
        T& at(size_t i, size_t j) { return (*this)[i*N + j]; }
    };

    template<typename T>
    struct StereoVector
    {
        std::vector<T> samples[2];

        StereoVector(size_t n) {
            for(size_t i = 0; i < n; i++) samples[i].resize(n);
        }

        std::vector<T>& operator[](size_t channel) { return samples[channel]; }

        std::vector<T> get_left_channel() { return samples[0]; }
        std::vector<T> get_right_channel() { return samples[1]; }

        void set_left_channel(std::vector<T> & v) { samples[0] = v; }
        void set_right_channel(std::vector<T> & v) { samples[1] = v; }
    };


    template<typename T>
    T get_stride(size_t ch, size_t num_channels, size_t pos, std::vector<T> & samples)
    {
        return samples[pos*num_channels + ch];
    }
        
    template<typename T>
    void set_stride(size_t ch, size_t num_channels, size_t pos, std::vector<T> & samples, T sample)
    {
        samples[pos*num_channels + ch] = sample;
    }

    template<typename T>
    std::vector<T> get_left_channel(const std::vector<T> & in) {
        std::vector<T> r(in.size()/2);
        size_t x = 0;
        for(size_t i = 0; i < in.size(); i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    std::vector<T> get_right_channel(const std::vector<T> & in) {
        std::vector<T> r(in.size()/2);
        size_t x = 0;
        for(size_t i = 1; i < in.size(); i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    std::vector<T> get_channel(size_t ch, const std::vector<T> & in) {
        std::vector<T> r(in.size()/2);
        size_t x = 0;
        for(size_t i = ch; i < in.size(); i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    void set_left_channel(const std::vector<T> & left, std::vector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = left[x++];
    }
    template<typename T>
    void set_right_channel(const std::vector<T> & right, std::vector<T> & out) {
        size_t x = 0;
        for(size_t i = 1; i < out.size(); i+=2) out[i] = right[x++];
    }
    template<typename T>
    void set_channel(size_t ch, const std::vector<T> & in, std::vector<T> & out) {
        size_t x = 0;
        for(size_t i = ch; i < out.size(); i+=2) out[i] = in[x++];
    }
    template<typename T>
    std::vector<T> interleave(size_t n, size_t channels, const std::vector<std::vector<T>> & in) {
        std::vector<T> r(n*channels);
        for(size_t i = 0; i < channels; i++)
            for(size_t j = 0; j < n; j++)
                r[j*channels + i] = in[i][j];
        return r;
    }
    template<typename T>
    std::vector<T> interleave(size_t n, size_t channels, const std::vector<T*> & in) {
        std::vector<T> r(n*channels);
        for(size_t i = 0; i < channels; i++)
            for(size_t j = 0; j < n; j++)
                r[j*channels + i] = in[i][j];
        return r;
    }
    template<typename T>
    std::vector<std::vector<T>> deinterleave(size_t n, size_t channels, const std::vector<T> & in) {
        std::vector<std::vector<T>> r(n);
        for(size_t i = 0; i < channels; i++)
        {
            r[i].resize(n);
            for(size_t j = 0; j < n; j++)
                r[i][j] = in[j*channels + i];
        }
        return r;
    }

    /*
    template<typename V, typename T>
    T get_stride(size_t ch, size_t num_channels, size_t pos, V & samples)
    {
        return samples[pos*num_channels + ch];
    }
    template<typename V, typename T>
    void set_stride(size_t ch, size_t num_channels, size_t pos, V & samples, T sample)
    {
        samples[pos*num_channels + ch] = sample;
    }

    template<typename V, typename T>
    std::vector<T> get_left_channel(size_t n, const V& in) {
        std::vector<T> r(n/2);
        size_t x = 0;
        for(size_t i = 0; i < n; i+=2) r[x++] = in[i];
        return r;
    }
    template<typename V, typename T>
    std::vector<T> get_right_channel(size_t n, const V& in) {
        std::vector<T> r(n/2);
        size_t x = 0;
        for(size_t i = 1; i < n; i+=2) r[x++] = in[i];
        return r;
    }
    template<typename V, typename T>
    std::vector<T> get_channel(size_t ch, size_t n, V& in) {
        std::vector<T> r(n/2);
        size_t x = 0;
        for(size_t i = ch; i < n; i+=2) r[x++] = in[i];
        return r;
    }

    template<typename V,typename T>
    void set_left_channel(size_t n, const T* left, V& out) {
        size_t x = 0;
        for(size_t i = 0; i < n; i+=2) out[i] = left[x++];
    }
    template<typename V,typename T>
    void set_right_channel(size_t n, const T* right, V& out) {
        size_t x = 0;
        for(size_t i = 1; i < n; i+=2) out[i] = right[x++];
    }
    template<typename V,typename T>
    void set_channel(size_t ch, size_t n, const T* in, V& out) {
        size_t x = 0;
        for(size_t i = ch; i < n; i+=2) out[i] = in[x++];
    }

    template<typename V,typename T>
    std::vector<T> interleave(size_t n, size_t channels, const V& in) {
        std::vector<T> r(n*channels);
        for(size_t i = 0; i < channels; i++)
            for(size_t j = 0; j < n; j++)
                r[j*channels + i] = in[i][j];
        return r;
    }
    template<typename V, typename T>
    std::vector<std::vector<T>> deinterleave(size_t n, size_t channels, const V& in) {
        std::vector<std::vector<T>> r(n);
        for(size_t i = 0; i < channels; i++)
        {
            r[i].resize(n);
            for(size_t j = 0; j < n; j++)
                r[i][j] = in[j*channels + i];
        }
        return r;
    }
    */

    template<typename T>
    T get_stride(size_t ch, size_t num_channels, size_t pos, T * samples)
    {
        return samples[pos*num_channels + ch];
    }
    template<typename T>
    void set_stride(size_t ch, size_t num_channels, size_t pos, T * samples, T sample)
    {
        samples[pos*num_channels + ch] = sample;
    }

    template<typename T>
    std::vector<T> get_left_channel(size_t n, const T* in) {
        std::vector<T> r(n/2);
        size_t x = 0;
        for(size_t i = 0; i < n; i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    std::vector<T> get_right_channel(size_t n, const T* & in) {
        std::vector<T> r(n/2);
        size_t x = 0;
        for(size_t i = 1; i < n; i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    std::vector<T> get_channel(size_t ch, size_t n, T* in) {
        std::vector<T> r(n/2);
        size_t x = 0;
        for(size_t i = ch; i < n; i+=2) r[x++] = in[i];
        return r;
    }

    template<typename T>
    void set_left_channel(size_t n, const T* left, T* out) {
        size_t x = 0;
        for(size_t i = 0; i < n; i+=2) out[i] = left[x++];
    }
    template<typename T>
    void set_right_channel(size_t n, const T* right, T* out) {
        size_t x = 0;
        for(size_t i = 1; i < n; i+=2) out[i] = right[x++];
    }
    template<typename T>
    void set_channel(size_t ch, size_t n, const T* in, T* out) {
        size_t x = 0;
        for(size_t i = ch; i < n; i+=2) out[i] = in[x++];
    }

    template<typename T>
    std::vector<T> interleave(size_t n, size_t channels, const T** & in) {
        std::vector<T> r(n*channels);
        for(size_t i = 0; i < channels; i++)
            for(size_t j = 0; j < n; j++)
                r[j*channels + i] = in[i][j];
        return r;
    }
    template<typename T>
    std::vector<std::vector<T>> deinterleave(size_t n, size_t channels, const T* & in) {
        std::vector<std::vector<T>> r(n);
        for(size_t i = 0; i < channels; i++)
        {
            r[i].resize(n);
            for(size_t j = 0; j < n; j++)
                r[i][j] = in[j*channels + i];
        }
        return r;
    }

     template<typename T>
     bool equal_vector (std::vector<T> & a, std::vector<T> & b) {
         return std::equal(a.begin(),a.end(),b.end());
     }
    template<typename T>
    void copy_vector(std::vector<T> & dst, std::vector<T> & src) {
        std::copy(src.begin(),src.end(),dst.begin());
    }
    template<typename T>
    void copy_vector(std::vector<T> & dst, size_t n, T * src) {
        std::copy(&src[0],&src[n-1],dst.begin());
    }
    template<typename T>
    std::vector<T> slice_vector(size_t start, size_t end, std::vector<T> & src) {
        std::vector<T> r(end-start);
        std::copy(src.begin()+start,src.begin()+end,r.begin());
        return r;
    }

    template<typename T>
    void copy_buffer(size_t n, T * dst, T * src) {
        memcpy(dst,src,n*sizeof(T));
    }

    template<typename T>
    std::vector<T> slice_buffer(size_t start, size_t end, T * ptr) {
        std::vector<T> r(end-start);
        std::copy(&ptr[start],&ptr[end],r.begin());
        return r;
    }

    template<typename T>
    void split_stereo(size_t n, const T* input, T * left, T * right)
    {
        size_t x=0;
        for(size_t i = 0; i < n; i+=2)
        {
            left[x] = input[i];
            right[x++] = input[i+1];
        }
    }

    template<typename T>
    void split_stereo(const std::vector<T> & input, std::vector<T> & left, std::vector<T> & right) {
        size_t x = input.size();
        left.resize(x/2);
        right.resize(x/2);
        split_stereo(x,input.data(),left.data(),right.data());
    }

    template<typename T>
    T insert_front(size_t n, T in, T * buffer) {
        T r = buffer[n-1];
        for(size_t i=0; i < n-1; i++) buffer[n+1] = buffer[n];
        buffer[0] = in;
        return r;
    }

    //============================================================
    template <class T>
    bool isEmpty(std::vector<T> v)
    {
        return (v.size() == 0);
    }

    //============================================================
    template <class T>
    bool containsOnlyZeros(std::vector<T> v)
    {
        if (!isEmpty(v))
        {
            for (int i = 0;i < v.size();i++)
            {
                if (v[i] != 0)
                {
                    return false;
                }
            }

            return true;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when checking if vector contains only zeros" );
        }
    }

    //============================================================
    template <class T>
    bool isAllPositiveOrZero(std::vector<T> v)
    {
        if (!isEmpty(v))
        {
            for (int i = 0;i < v.size();i++)
            {
                if (v[i] < 0)
                {
                    return false;
                }
            }

            return true;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when checking if vector is all positive" );
        }
    }

    //============================================================
    template <class T>
    bool isAllNegativeOrZero(std::vector<T> v)
    {
        if (!isEmpty(v))
        {
            for (int i = 0;i < v.size();i++)
            {
                if (v[i] > 0)
                {
                    return false;
                }
            }

            return true;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when checking if vector is all negative" );
        }
    }

    //============================================================
    template <class T>
    bool contains(std::vector<T> v, T element)
    {
        for (int i = 0;i < v.size();i++)
        {
            if (v[i] == element)
            {
                return true;
            }
        }

        return false;
    }


    //============================================================
    template <class T>
    T max(std::vector<T> v)
    {
        // if the vector isn't empty
        if (!isEmpty(v))
        {
            // set the first element as the max
            T maxVal = v[0];

            // then for each subsequent value
            for (int i = 1;i < v.size();i++)
            {
                // if it is larger than the max
                if (v[i] > maxVal)
                {
                    // store it as the new max value
                    maxVal = v[i];
                }
            }

            // return the maxVal
            return maxVal;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when calculating max" );
        }
    }

    //============================================================
    template <class T>
    int maxIndex(std::vector<T> v)
    {
        // if the vector isn't empty
        if (!isEmpty(v))
        {
            // set the first element as the max
            T maxVal = v[0];
            int maxIndex = 0;

            // then for each subsequent value
            for (int i = 1;i < v.size();i++)
            {
                // if it is larger than the max
                if (v[i] > maxVal)
                {
                    // store it as the new max value
                    maxVal = v[i];

                    // store the index as the new max index
                    maxIndex = i;
                }
            }

            // return the max index
            return maxIndex;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when calculating max index" );
        }
    }

    //============================================================
    template <class T>
    T min(std::vector<T> v)
    {
        // if the vector isn't empty
        if (!isEmpty(v))
        {
            // set the first element as the min
            T minVal = v[0];

            // then for each subsequent value
            for (int i = 1;i < v.size();i++)
            {
                // if it is smaller than the min
                if (v[i] < minVal)
                {
                    // store it as the new min value
                    minVal = v[i];
                }
            }

            // return the minVal
            return minVal;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when calculating min" );
        }
    }

    //============================================================
    template <class T>
    int minIndex(std::vector<T> v)
    {
        // if the vector isn't empty
        if (!isEmpty(v))
        {
            // set the first element as the min
            T minVal = v[0];
            int minIndex = 0;

            // then for each subsequent value
            for (int i = 1;i < v.size();i++)
            {
                // if it is smaller than the min
                if (v[i] < minVal)
                {
                    // store it as the new min value
                    minVal = v[i];

                    // store the index as the new min index
                    minIndex = i;
                }
            }

            // return the min index
            return minIndex;
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when calculating minIndex" );
        }
    }

    //============================================================
    template <class T>
    void printVector(std::vector<T> v)
    {
        for (int i = 0;i < v.size();i++)
        {
            std::cout << v[i] << std::endl;
        }
    }

    //============================================================
    template <class T>
    T getLastElement(std::vector<T> v)
    {
        // if vector is not empty
        if (v.size() > 0)
        {
            return v[v.size()-1];
        }
        else
        {
            throw std::invalid_argument( "Attempted to get last element of empty vector" );
        }
    }

    //============================================================
    template <class T>
    T getFirstElement(std::vector<T> v)
    {
        // if vector is not empty
        if (v.size() > 0)
        {
            return v[0];
        }
        else
        {
            throw std::invalid_argument( "Attempted to get first element of empty vector" );
        }
    }


    //============================================================
    template <class T>
    std::vector<T> getEveryNthElementStartingFromK(std::vector<T> v,int n,int k)
    {
        if ((n >= v.size()) || (n >= v.size()))
        {
            throw std::invalid_argument( "Invalid arguments for getEveryNthElementStartingFromK()");
        }
        else
        {
            std::vector<T> result;

            int i = k;

            while (i < v.size())
            {
                result.push_back(v[i]);
                i += n;
            }

            return result;
        }
    }

    //============================================================
    template <class T>
    std::vector<T> getEvenElements(std::vector<T> v)
    {
        return getEveryNthElementStartingFromK(v, 2, 0);
    }

    //============================================================
    template <class T>
    std::vector<T> getOddElements(std::vector<T> v)
    {
        return getEveryNthElementStartingFromK(v, 2, 1);
    }

    //============================================================
    template <class T>
    void fillVectorWith(std::vector<T> &v,T element)
    {
        for (int i = 0;i < v.size();i++)
        {
            v[i] = element;
        }
    }

    //============================================================
    template <class T>
    int countOccurrencesOf(std::vector<T> v,T element)
    {
        int count = 0;

        for (int i = 0;i < v.size();i++)
        {
            if (v[i] == element)
            {
                count++;
            }
        }

        return count;
    }

    //============================================================
    template <class T>
    T sum(std::vector<T> v)
    {
        // create a sum
        T sumVal = 0;

        // add up all elements
        for (int i = 0;i < v.size();i++)
        {
            sumVal += v[i];
        }

        // return
        return sumVal;
    }

    //============================================================
    template <class T>
    T product(std::vector<T> v)
    {
        if (!isEmpty(v))
        {
            T prod = (T) v[0];

            for (int i = 1;i < v.size();i++)
            {
                prod *= ((T) v[i]);
            }

            return prod;
        }
        else
        {
            throw std::invalid_argument( "Attempted to calculate the product of an empty vector" );
        }
    }

    //============================================================
    template <class T>
    T mean(std::vector<T> v)
    {
        // if vector is not empty
        if (!isEmpty(v))
        {
            // store the length of the vector as a T
            T L = (T) v.size();

            // stor the sum of the vector as a T
            T sumVal = (T) sum(v);

            // return the mean
            return sumVal / L;
        }
        else // vector is empty
        {
            throw std::invalid_argument( "Received empty vector when calculating mean" );
        }
    }

    //============================================================
    template <class T>
    T median(std::vector<T> v)
    {
        // if vector isn't empty
        if (!isEmpty(v))
        {
            T median;
            size_t L = v.size(); // store the size

            // sort the vector
            std::sort(v.begin(), v.end());

            // if the length is even
            if (L  % 2 == 0)
            {
                // take the average of the middle two elements
                median = ((T)(v[L / 2 - 1] + v[L / 2])) / 2.0;
            }
            else // if the length is odd
            {
                // take the middle element
                median = (T) v[(L-1) / 2];
            }

            // return the median
            return median;
        }
        else // vector is empty
        {
            throw std::invalid_argument( "Received empty vector when calculating median" );
        }
    }

    //============================================================
    template <class T>
    T variance(std::vector<T> v)
    {
        if (!isEmpty(v))
        {
            // calculate the mean of the vector
            T mu = mean(v);

            T sumVal = 0.0;

            // sum the product of all differences from the mean
            for (int i = 0;i < v.size();i++)
            {
                T diff = v[i]-mu;
                sumVal += diff*diff;
            }

            // return the average of the squared differences
            return sumVal / ((T)v.size());
        }
        else
        {
            throw std::invalid_argument( "Received empty vector when calculating variance" );
        }
    }

    //============================================================
    template <class T>
    T standardDeviation(std::vector<T> v)
    {
        // if vector is not empty
        if (!isEmpty(v))
        {
            // calculate the variance
            T var = variance(v);

            // if variance is non-zero
            if (var > 0)
            {
                // return the square root
                return std::sqrt(var);
            }
            else
            {
                // all differences are zero, so return 0.0
                return 0.0;
            }
        }
        else // vector is empty
        {
            throw std::invalid_argument( "Received empty vector when calculating standard deviation" );
        }
    }

    //============================================================
    template <class T>
    T norm1(std::vector<T> v)
    {
        T sumVal = 0.0;

        // sum absolute values
        for (int i = 0;i < v.size();i++)
        {
            if (v[i] > 0)
            {
                sumVal += (T) v[i];
            }
            else
            {
                sumVal += (T) (-1*v[i]);
            }
        }

        return sumVal;
    }

    //============================================================
    template <class T>
    T norm2(std::vector<T> v)
    {
        T sumVal = 0.0;

        // sum squares
        for (int i = 0;i < v.size();i++)
        {
            sumVal += (T) (v[i]*v[i]);
        }

        return std::sqrt(sumVal);
    }

    //============================================================
    template <class T>
    T magnitude(std::vector<T> v)
    {
        // just another name for L2-norm
        return norm2(v);
    }

    //============================================================
    template <class T>
    T normP(std::vector<T> v,T p)
    {
        T sumVal = 0.0;

        for (int i = 0;i < v.size();i++)
        {
            T val;

            if (v[i] > 0)
            {
                val = (T) v[i];
            }
            else
            {
                val = (T) (-1*v[i]);
            }

            sumVal += std::pow(val,p);
        }

        return std::pow(sumVal,1.0/p);
    }

    //============================================================
    template <class T>
    void multiplyInPlace(std::vector<T> &v,T scalar)
    {
        for (int i = 0;i < v.size();i++)
        {
            v[i] *= scalar;
        }
    }

    //============================================================
    template <class T>
    void multiplyInPlace(std::vector<T> &v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            for (int i = 0;i < v1.size();i++)
            {
                v1[i] *= v2[i];
            }
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector multiply");
        }
    }

    //============================================================
    template <class T>
    void divideInPlace(std::vector<T> &v,T scalar)
    {
        if (scalar != 0)
        {
            for (int i = 0;i < v.size();i++)
            {
                v[i] /= scalar;
            }
        }
        else
        {
            throw std::invalid_argument( "Attemted to divide a vector by a zero-valued scalar" );
        }
    }

    //============================================================
    template <class T>
    void divideInPlace(std::vector<T> &v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            if (!contains<T>(v2, 0))
            {
                for (int i = 0;i < v1.size();i++)
                {
                    v1[i] /= v2[i];
                }
            }
            else
            {
                throw std::invalid_argument( "Attempted to divide by vector containing zeros");
            }
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector divide");
        }
    }

    //============================================================
    template <class T>
    void addInPlace(std::vector<T> &v,T value)
    {
        for (int i = 0;i < v.size();i++)
        {
            v[i] += value;
        }
    }

    //============================================================
    template <class T>
    void addInPlace(std::vector<T> &v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            for (int i = 0;i < v1.size();i++)
            {
                v1[i] += v2[i];
            }
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector add");
        }
    }

    //============================================================
    template <class T>
    void subtractInPlace(std::vector<T> &v,T value)
    {
        for (int i = 0;i < v.size();i++)
        {
            v[i] -= value;
        }
    }

    //============================================================
    template <class T>
    void subtractInPlace(std::vector<T> &v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            for (int i = 0;i < v1.size();i++)
            {
                v1[i] -= v2[i];
            }
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector subtraction");
        }

    }

    //============================================================
    template <class T>
    void absInPlace(std::vector<T> &v)
    {
        for (int i = 0;i < v.size();i++)
        {
            if ((v[i] < 0) || (v[i] == -0.0))
            {
                v[i] *= -1;
            }
        }
    }

    //============================================================
    template <class T>
    void squareInPlace(std::vector<T> &v)
    {
        for (int i = 0;i < v.size();i++)
        {
            v[i] = v[i]*v[i];
        }
    }

    //============================================================
    template <class T>
    void squareRootInPlace(std::vector<T> &v)
    {
        if (isAllPositiveOrZero(v))
        {
            for (int i = 0;i < v.size();i++)
            {
                v[i] = (T) std::sqrt((T)v[i]);
            }
        }
        else
        {
            throw std::invalid_argument( "Attempted to compute square root of vector containing negative numbers");
        }
    }


    //============================================================
    template <class T>
    void sort(std::vector<T> &v)
    {
        std::sort(v.begin(),v.end());
    }

    //============================================================
    template <class T>
    void reverse(std::vector<T> &v)
    {
        std::reverse(v.begin(), v.end());
    }

    //============================================================
    template <class T>
    std::vector<T> multiply(std::vector<T> v,T scalar)
    {
        std::vector<T> result;

        for (int i = 0;i < v.size();i++)
        {
            result.push_back(v[i] * scalar);
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> multiply(std::vector<T> v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            std::vector<T> result;

            for (int i = 0;i < v1.size();i++)
            {
                result.push_back(v1[i] * v2[i]);
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector multiply");
        }
    }

    //============================================================
    template <class T>
    std::vector<T> divide(std::vector<T> v,T scalar)
    {
        if (scalar != 0)
        {
            std::vector<T> result;

            for (int i = 0;i < v.size();i++)
            {
                result.push_back(v[i] / scalar);
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Attemted to divide a vector by a zero-valued scalar" );
        }
    }

    //============================================================
    template <class T>
    std::vector<T> divide(std::vector<T> v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            if (!contains<T>(v2, 0))
            {
                std::vector<T> result;

                for (int i = 0;i < v1.size();i++)
                {
                    result.push_back(v1[i] / v2[i]);
                }

                return result;
            }
            else
            {
                throw std::invalid_argument( "Attempted to divide by vector containing zeros");
            }
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector divide");
        }
    }

    //============================================================
    template <class T>
    std::vector<T> add(std::vector<T> v,T value)
    {
        std::vector<T> result;

        for (int i = 0;i < v.size();i++)
        {
            result.push_back(v[i] + value);
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> add(std::vector<T> v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            std::vector<T> result;

            for (int i = 0;i < v1.size();i++)
            {
                result.push_back(v1[i] + v2[i]);
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector add");
        }
    }

    //============================================================
    template <class T>
    std::vector<T> subtract(std::vector<T> v,T value)
    {
        std::vector<T> result;

        for (int i = 0;i < v.size();i++)
        {
            result.push_back(v[i] - value);
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> subtract(std::vector<T> v1,std::vector<T> v2)
    {
        if (v1.size() == v2.size())
        {
            std::vector<T> result;

            for (int i = 0;i < v1.size();i++)
            {
                result.push_back(v1[i] - v2[i]);
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector subtraction");
        }
    }

    //============================================================
    template <class T>
    std::vector<T> abs(std::vector<T> v)
    {
        std::vector<T> result;

        for (int i = 0;i < v.size();i++)
        {
            if ((v[i] < 0) || (v[i] == -0.0))
            {
                result.push_back(-1*v[i]);
            }
            else
            {
                result.push_back(v[i]);
            }
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> square(std::vector<T> v)
    {
        std::vector<T> result;

        for (int i = 0;i < v.size();i++)
        {
            result.push_back(v[i]*v[i]);
        }

        return result;
    }


    //============================================================
    template <class T>
    std::vector<T> squareRoot(std::vector<T> v)
    {
        if (isAllPositiveOrZero(v))
        {
            std::vector<T> result;

            for (int i = 0;i < v.size();i++)
            {
                result.push_back((T) std::sqrt((T)v[i]));
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Attempted to compute square root of vector containing negative numbers");
        }
    }

    //============================================================
    template <class T>
    std::vector<T> scale(std::vector<T> v,T lowerLimit,T upperLimit)
    {
        std::vector<T> result;

        T minVal = (T) min(v);
        T maxVal = (T) max(v);
        T outputRange = upperLimit - lowerLimit;
        T inputRange = maxVal - minVal;

        for (int i = 0;i < v.size();i++)
        {
            T value = (T) v[i];
            T scaledValue = ((value - minVal) * outputRange) / inputRange + lowerLimit;

            result.push_back(scaledValue);
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> difference(std::vector<T> v)
    {
        std::vector<T> result;

        for (int i = 1;i < v.size();i++)
        {
            result.push_back(v[i]-v[i-1]);
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> zeros(int N)
    {
        if (N >= 0)
        {
            std::vector<T> result;

            for (int i = 0;i < N;i++)
            {
                result.push_back(0);
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Cannot create vector with negative length");
        }
    }

    //============================================================
    template <class T>
    std::vector<T> ones(int N)
    {
        if (N >= 0)
        {
            std::vector<T> result;

            for (int i = 0;i < N;i++)
            {
                result.push_back(1);
            }

            return result;
        }
        else
        {
            throw std::invalid_argument( "Cannot create vector with negative length");
        }
    }


    //============================================================
    template <class T>
    std::vector<T> range(int limit1,int limit2,int step)
    {
        std::vector<T> result;

        if (step > 0)
        {
            for (T i = limit1;i < limit2;i += step)
            {
                result.push_back(i);
            }
        }
        else if (step < 0)
        {
            for (T i = limit1;i > limit2;i += step)
            {
                result.push_back(i);
            }
        }
        else
        {
            throw std::invalid_argument( "Cannot use a step size of 0 when creating a range of values");
        }

        return result;
    }

    //============================================================
    template <class T>
    std::vector<T> range(int maxValue)
    {
        return SoundWave::range<T>(0, maxValue, 1);
    }

    //============================================================
    template <class T>
    std::vector<T> range(int minValue,int maxValue)
    {
        return SoundWave::range<T>(minValue, maxValue, 1);
    }

    //============================================================
    template <class T>
    T dotProduct(std::vector<T> v1,std::vector<T> v2)
    {
        // if vector size is the same
        if (v1.size() == v2.size())
        {
            T sumVal = 0.0;

            // sum the element-wise product
            for (int i = 0;i < v1.size();i++)
            {
                sumVal += (v1[i]*v2[i]);
            }

            // return the sum as the dot product
            return sumVal;
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in vector dot product");
        }
    }

    //============================================================
    template <class T>
    T euclideanDistance(std::vector<T> v1,std::vector<T> v2)
    {
        // if vector size is the same
        if (v1.size() == v2.size())
        {
            T sumVal = 0.0;

            // sum the squared difference
            for (int i = 0;i < v1.size();i++)
            {
                T diff = (T) (v1[i] - v2[i]);
                sumVal += (diff*diff);
            }

            // if sum is bigger than zero
            if (sumVal > 0)
            {
                // return the square root of the sum as the Euclidean distance
                return std::sqrt(sumVal);
            }
            else // all differences were zero, so report 0.0 as Euclidean distance
            {
                return 0.0;
            }
        }
        else
        {
            throw std::invalid_argument( "Vector lengths differ in Euclidean distance calculation");
        }
    }

    //============================================================
    template <class T>
    T cosineSimilarity(std::vector<T> v1,std::vector<T> v2)
    {
    return dotProduct(v1, v2) / (magnitude(v1) * magnitude(v2));
    }

    //============================================================
    template <class T>
    T cosineDistance(std::vector<T> v1,std::vector<T> v2)
    {
        return 1.0 - cosineSimilarity(v1, v2);
    }


    template<class T>
    std::vector<T> cos(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::cos(v[i]);
        return r;
    }
    template<class T>
    std::vector<T> sin(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::sin(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> tan(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::tan(v[i]);
        return r;
    }

    template<class T>
    std::vector<T> acos(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::acos(v[i]);
        return r;
    }
    template<class T>
    std::vector<T> asin(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::asin(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> atan(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::atan(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> atan2(const std::vector<T> & v, const T value) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::atan2(v[i], value);
        return r;
    }    
    template<class T>
    std::vector<T> atan2(const std::vector<T> & v, const std::vector<T> value) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::atan2(v[i], value[i]);
        return r;
    }    
    template<class T>
    std::vector<T> cosh(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::cosh(v[i]);
        return r;
    }
    template<class T>
    std::vector<T> sinh(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::sinh(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> tanh(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::tanh(v[i]);
        return r;
    }

    template<class T>
    std::vector<T> acosh(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::acosh(v[i]);
        return r;
    }
    template<class T>
    std::vector<T> asinh(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::asinh(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> atanh(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::atanh(v[i]);
        return r;
    }    

    template<class T>
    std::vector<T> exp(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::exp(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> log(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::log(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> log10(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::log10(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> exp2(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::exp2(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> expm1(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::expm1(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> ilogb(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::ilogb(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> log2(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::log2(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> log1p(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::log1p(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> logb(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::logb(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> scalbn(const std::vector<T> & v, const std::vector<int> & x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::scalbn(v[i],x[i]);
        return r;
    }    
    template<class T>
    std::vector<T> scalbn(const std::vector<T> & v, const int x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::scalbn(v[i],x);
        return r;
    }    
    template<class T>
    std::vector<T> scalbln(const std::vector<T> & v, const std::vector<long int> & x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::scalbln(v[i],x[i]);
        return r;
    }    
    template<class T>
    std::vector<T> scalbln(const std::vector<T> & v, const long int x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::scalbln(v[i],x);
        return r;
    }    
    template<class T>
    std::vector<T> pow(const std::vector<T> & v, const std::vector<T> & x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::pow(v[i],x[i]);
        return r;
    }    
    template<class T>
    std::vector<T> pow(const std::vector<T> & v, const T x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::pow(v[i],x);
        return r;
    }    
    template<class T>
    std::vector<T> pow(const T x, const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::pow(x,v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> sqrt(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::sqrt(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> cbrt(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::cbrt(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> hypot(const std::vector<T> & v, const std::vector<T> & x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::hypot(v[i],x[i]);
        return r;
    }    
    template<class T>
    std::vector<T> hypot(const std::vector<T> & v, const T x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::hypot(v[i],x);
        return r;
    }    
    template<class T>
    std::vector<T> hypot(const T x, const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::hypot(x,v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> erf(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::erf(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> erfc(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::erfc(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> tgamma(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::tgamma(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> lgamma(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::lgamma(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> ceil(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::ceil(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> floor(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::floor(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> fmod(const std::vector<T> & v, const std::vector<T> & x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::fmod(v[i],x[i]);
        return r;
    }    
    template<class T>
    std::vector<T> fmod(const std::vector<T> & v, const T x) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::fmod(v[i],x);
        return r;
    }    
    template<class T>
    std::vector<T> fmod(const T x, const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::fmod(x,v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> trunc(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::trunc(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> round(const std::vector<T> & v) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::round(v[i]);
        return r;
    }    
    template<class T>
    std::vector<long int> lround(const std::vector<T> & v) {
        std::vector<long int> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::lround(v[i]);
        return r;
    }    
    template<class T>
    std::vector<long long int> llround(const std::vector<T> & v) {
        std::vector<long long int> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::llround(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> nearbyint(const std::vector<T> & v) {
        std::vector<long long int> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = std::nearbyint(v[i]);
        return r;
    }    
    template<class T>
    std::vector<T> remainder(const std::vector<T> & a, const std::vector<T> & b) {
        std::vector<long long int> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::remainder(a[i],b[i]);
        return r;
    }    
    template<class T>
    std::vector<T> copysign(const std::vector<T> & a, const std::vector<T> & b) {
        std::vector<long long int> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::copysign(a[i],b[i]);
        return r;
    }    
    template<class T>
    std::vector<T> fdim(const std::vector<T> & a, const std::vector<T> & b) {
        std::vector<long long int> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::fdim(a[i],b[i]);
        return r;
    }    
    #undef fmax
    template<class T>
    std::vector<T> fmax(const std::vector<T> & a, const std::vector<T> & b) {
        std::vector<long long int> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::fmax(a[i],b[i]);
        return r;
    }    
    template<class T>
    std::vector<T> fmin(const std::vector<T> & a, const std::vector<T> & b) {
        std::vector<long long int> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::fmin(a[i],b[i]);
        return r;
    }    
    template<class T>
    std::vector<T> fma(const std::vector<T> & a, const std::vector<T> & b, const std::vector<T> & c) {
        std::vector<long long int> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::fma(a[i],b[i],c[i]);
        return r;
    }    
    template<class T>
    std::vector<T> fabs(const std::vector<T> & a) {
        std::vector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::fabs(a[i]);
        return r;
    }    
    
    /*
    template<class T>    
    std::vector<T> abs(const std::vector<T> & a) {
        std::vector<T> r(a.size());
        for(size_t i = 0; i < a.size(); i++) r[i] = std::abs((long la[i]);
        return r;
    } 
    */
       


    template<typename T>
    StereoVector<T> stereo(const std::vector<T> & left, const std::vector<T> & right) {
        StereoVector<T> r(left.size());
        for(size_t i = 0; i < left.size(); i++)
        {
            r[0][i] = left[i];
            r[1][i] = right[i];
        }
    }

    template<typename T>
    std::vector<T> merge(const std::vector<T> & left, const std::vector<T> & right) {
        std::vector<T> r(left.size()*2);
        size_t x = 0;
        for(size_t i = 0; i < left.size(); i++)
        {
            r[x++] = left[i];
            r[x++] = right[i];
        }
    }

    template<typename T>
    void swap(std::vector<T> & left, std::vector<T> & right) {
        std::swap(left,right);
    }
    
    template<typename T>
    bool isin(const std::vector<T> & v, const T val) {
        return std::find(v.begin(),v.end(),val) != v.end();
    }

    template<typename T>
    StereoVector<T> pan(const std::vector<T> & left, const std::vector<T> & right, T amt) {
        StereoVector<T> r(left.size());
        T pan_map = ((amt+1)/2.0) * (M_PI/2.0);
        for(size_t i = 0; i < left.size(); i++)
        {
            r[0][i] = left[i] * sin(pan_map);
            r[1][i] = right[i] * cos(pan_map);
        }
        return r;
    }
    template<typename T>
    StereoVector<T> constant_power_pan(const std::vector<T> & left, const std::vector<T> & right, T pos) {
        StereoVector<T> r(left.size());        
        const T piover2 = 4.0*std::atan(1.0)*0.5;
        const T root2over2 = std::sqrt(2.0)*0.5;
        T thispos = pos * piover2;
        T angle   = thispos * 0.5;
        T pleft   = root2over2 * (std::cos(angle) - std::sin(angle));
        T pright  = root2over2 * (std::cos(angle) + std::sin(angle));
        for(size_t i = 0; i < left.size(); i++)
        {
            r[0][i] = left[i] * pleft;
            r[1][i] = right[i] * pright;
        }
        return r;
    }
    template<typename T>
    std::vector<T> mix(const std::vector<T> & a, const std::vector<T> & b)
    {
        assert(a.size() == b.size());
        std::vector<T> r(a.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = a[i]+b[i];
        T max = std::max_element(r.begin(),r.end());
        if(max > 0) for(size_t i = 0; i < r.size(); i++) r[i] /= max;
        return r;
    }
    template<typename T>
    std::vector<T> normalize(const std::vector<T> & a) {
        std::vector<T> r(a);        
        T max = std::max_element(r.begin(),r.end());
        if(max > 0) for(size_t i = 0; i < r.size(); i++) r[i] /= max;
        return r;
    }
    template<class A, class B>
    std::vector<B> convert(const std::vector<A> & v) {
        std::vector<B> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = B(v[i]);
    }
    template<class T>
    std::vector<T> kernel(const std::vector<T> & v, T (*f)(T value)) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = f(v[i]);
        return r;
    }
    template<class T>
    std::vector<T> kernel(const std::vector<T> & v, std::function<T (T)> func) {
        std::vector<T> r(v.size());
        for(size_t i = 0; i < v.size(); i++) r[i] = func(v[i]);
        return r;
    }
    template<class T>
    void inplace_add(const std::vector<T> & a, std::vector<T> & r, std::function<T (T)> func) {        
        for(size_t i = 0; i < a.size(); i++) r[i] += func(a[i]);        
    }
    template<class T>
    void inplace_sub(const std::vector<T> & a, std::vector<T> & r, std::function<T (T)> func) {        
        for(size_t i = 0; i < a.size(); i++) r[i] -= func(a[i]);        
    }
    template<class T>
    void inplace_mul(const std::vector<T> & a, std::vector<T> & r, std::function<T (T)> func) {        
        for(size_t i = 0; i < a.size(); i++) r[i] *= func(a[i]);        
    }
    template<class T>
    void inplace_div(const std::vector<T> & a, std::vector<T> & r, std::function<T (T)> func) {        
        for(size_t i = 0; i < a.size(); i++) r[i] /= func(a[i]);
    }

    
    template<class T>
    void fill(std::vector<T> & in, T x)
    {
        for(size_t i = 0; i < in.size(); i++) in[i] = x;
    }
    template<class T>
    void zeros(std::vector<T> & in)
    {
        fill(in,T(0));
    }
    template<class T>
    void ones(std::vector<T> & in)
    {
        fill(in,T(1));
    }

    template<typename T>
    void StdMean(T *sig_src_arr, uint32_t blockSize, T * result){
        T sum = T(0);
        uint32_t blkCnt;
        T in1,in2,in3, in4;
        assert(blockSize != 0);
        blkCnt = blockSize>>2U; //Right shifted by 4 so divided by 4

        while(blkCnt > 0){
            in1 = *sig_src_arr++;
            in2 = *sig_src_arr++;
            in3 = *sig_src_arr++;
            in4 = *sig_src_arr++;
            sum += in1;
            sum += in2;
            sum += in3;
            sum += in4;
            blkCnt--;
        }
        blkCnt = blockSize% 0x4;

        while(blkCnt > 0){
            sum += *sig_src_arr++;
            blkCnt--;
        }
        
        *result = sum/(T)blockSize;        
    }
    template<typename T>
    std::vector<T> StdMean(std::vector<T> & in) 
    {
        std::vector<T> r(in.size());
        zeros(r);
        StdMean(in.data(),in.size(),r.data());
        return r;
    }

    template<typename T>
    void StdRMS(T *pSig_src_arr, uint32_t blockSize, T *pResult)
    {
        T sum = 0.0;
        uint32_t blkCnt;
        T in;
        assert(blockSize != 0);
        blkCnt = blockSize >>2;
        while(blkCnt > 0){
            in = *pSig_src_arr++;
            sum += in*in;
            in = *pSig_src_arr++;
            sum += in*in;
            in = *pSig_src_arr++;
            sum += in*in;
            in = *pSig_src_arr++;
            sum += in*in;
            blkCnt--;
        }

        blkCnt = blockSize%0x4;
        while(blkCnt>0)
        {
            in = *pSig_src_arr++;
            sum += in*in;
            blkCnt--;
        }        
        *pResult = std::sqrt(sum/(T)blockSize);
    }

    template<typename T>
    std::vector<T> StdRMS(std::vector<T> & in) 
    {
        std::vector<T> r(in.size());
        zeros(r);
        StdRMS(in.data(),in.size(),r.data());
        return r;
    }

    template<typename T>
    void StdDev(T * pSig_src_arr, uint32_t blockSize, T *pResult)
    {

        T sum = 0.0;
        T sumOfSquares = 0.0;
        T in;

        uint32_t blkCnt;

        T meanOfSquares, mean, squareOfMean;
        T squareOfSum = 0.0;

        T var;

        if(blockSize == 1){
            *pResult = 0;
            return;
        }

        blkCnt = blockSize>>2;

        while(blkCnt>0){
        //perform this operation 4 times
            in = *pSig_src_arr++;
            sum+= in;
            sumOfSquares += in*in;
        //perform this operation 4 times
            in = *pSig_src_arr++;
            sum+= in;
            sumOfSquares += in*in;
        //perform this operation 4 times
            in = *pSig_src_arr++;
            sum+= in;
            sumOfSquares += in*in;
        //perform this operation 4 times
            in = *pSig_src_arr++;
            sum+= in;
            sumOfSquares += in*in;

            blkCnt--;
        }

        blkCnt = blockSize % 0x4;

        while(blkCnt>0){
        //perform this operation 4 times
            in = *pSig_src_arr++;
            sum+= in;
            sumOfSquares += in*in;

            blkCnt--;
        }

        meanOfSquares = sumOfSquares / ((T)blockSize-1.0);
        mean = sum/(T) blockSize;

        squareOfMean = (mean*mean) * ((T)blockSize/(T)(blockSize-1.0));

        *pResult = sqrt((meanOfSquares-squareOfMean));
    }

    template<typename T>
    std::vector<T> StdDev(std::vector<T> & in) 
    {
        std::vector<T> r(in.size());
        zeros(r);
        StdDev(in.data(),in.size(),r.data());
        return r;
    }

    template<typename T>
    void StdVariance(T * pSig_src_arr, uint32_t blockSize, T *pResult)
    {
        T fMean, fValue;
        uint32_t blkCnt;
        T * pInput = pSig_src_arr;

        T sum = 0.0;
        T fSum = 0.0;

        T in1, in2, in3, in4;

        if(blockSize <= 1){
            *pResult = 0;
            return;
        }

        blkCnt = blockSize >>2U;
        while(blkCnt>0){
            in1 = *pInput++;
            in2 = *pInput++;
            in3 = *pInput++;
            in4 = *pInput++;

            sum+= in1;
            sum+= in2;
            sum+= in3;
            sum+= in4;

        blkCnt--;
        }
        blkCnt = blockSize % 0x4;

        while(blkCnt > 0){
            sum += *pInput++;

            blkCnt--;
        }

        fMean = sum/(T) blockSize;
        pInput = pSig_src_arr;
        blkCnt = blockSize>>2;
        while(blkCnt > 0){
            fValue = *pInput++ - fMean;
            fSum += fValue*fValue;
            fValue = *pInput++ - fMean;
            fSum += fValue*fValue;
            fValue = *pInput++ - fMean;
            fSum += fValue*fValue;
            fValue = *pInput++ - fMean;
            fSum += fValue*fValue;

            blkCnt--;
        }
        blkCnt = blockSize % 0x4;

        while(blkCnt>0){
            fValue = *pInput++ - fMean;
            fSum += fValue*fValue;

            blkCnt--;
        }

        *pResult = fSum/(T)(blockSize-1.0);
    }

    template<typename T>
    std::vector<T> StdVariance(std::vector<T> & in) 
    {
        std::vector<T> r(in.size());
        zeros(r);
        StdVariation(in.data(),in.size(),r.data());
        return r;
    }

    template<typename A, typename B>
    std::vector<A> vector_cast(std::vector<B> & in) {
        std::vector<A> r(in.size());
        for(size_t i = 0; i < in.size(); i++)
            r[i] = (A)in[i];
        return r;
    }
    template<typename T>
    std::vector<T> vector_copy(T * ptr, size_t n) {
        std::vector<T> r(n);
        for(size_t i = 0; i < n; i++)
            r[i] = ptr[i];
        return r;
    }

 
    template<typename T>
    std::vector<std::complex<T>> dft_forward(std::vector<std::complex<T>> & in)
    {
        kfr::univector<std::complex<T>> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_dft(input);
        std::vector<std::complex<T>> r(output.size());
        for(size_t i = 0; i < output.size(); i++)
        {
            r[i].real(output[i].real());
            r[i].imag(output[i].imag());
        }
        return r;
    }
    template<typename T>
    std::vector<std::complex<T>> dft_inverse(std::vector<std::complex<T>> & in)
    {
        kfr::univector<std::complex<T>> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_idft(input);
        std::vector<std::complex<T>> r(output.size());
        for(size_t i = 0; i < output.size(); i++)
        {
            r[i].real(output[i].real());
            r[i].imag(output[i].imag());
        }
        return r;
    }
    template<typename T>
    std::vector<std::complex<T>> rdft_forward(std::vector<T> & in)
    {
        kfr::univector<std::complex<T>> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_realdft(input);
        std::vector<std::complex<T>> r(output.size());
        for(size_t i = 0; i < output.size(); i++)
        {
            r[i].real(output[i].real());
            r[i].imag(output[i].imag());
        }
        return r;
    }
    template<typename T>
    std::vector<T> rdft_inverse(std::vector<std::complex<T>> & in)
    {
        kfr::univector<T> output;
        kfr::univector<std::complex<T>> input(in.size());
        output = run_irealdft(input);
        std::vector<std::complex<T>> r(output.size());
        std::copy(output.begin(),output.end(),r.begin());
        return r;
    }

    template<typename T>
    std::vector<T> dct_forward(std::vector<T> in)
    {        
        DSP::DCTPlan<T> dct(in.size());
        kfr::univector<T> out(in.size());
        dct.execute(out,in,false);
        std::vector<T> r(out.size());
        memcpy(r.data(),out.data(),out.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> dct_inverse(std::vector<T> in)
    {        
        DSP::DCTPlan<T> dct(in.size());
        kfr::univector<T> out(in.size());
        dct.execute(out,in,true);
        std::vector<T> r(out.size());
        memcpy(r.data(),out.data(),out.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> convolve(std::vector<T> &a, std::vector<T> & b)
    {        
        kfr::univector<T> A(a.size());
        kfr::univector<T> B(b.size());        
        kfr::univector<T> R = kfr::convolve(A,B);
        std::vector<T> r(R.size());
        memcpy(r.data(),R.data(),R.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> correlate(std::vector<T> &a, std::vector<T> & b)
    {        
        kfr::univector<T> A(a.size());
        kfr::univector<T> B(b.size());        
        kfr::univector<T> R = kfr::correlate(A,B);
        std::vector<T> r(R.size());
        memcpy(r.data(),R.data(),R.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> autocorrelate(std::vector<T> &a)
    {        
        kfr::univector<T> A(a.size());        
        kfr::univector<T> R = kfr::autocorrelate(A);
        std::vector<T> r(R.size());
        memcpy(r.data(),R.data(),R.size()*sizeof(T));
        return r;
    }
    template<typename T>
    std::vector<T> convolve_filter(size_t block_size,std::vector<T> & impulse, std::vector<T> &x)
    {        
        kfr::univector<T> imp(impulse.size());
        std::copy(impulse.begin(),impulse.end(),imp.begin());
        DSP::ConvolveFilter<T> filter(imp,block_size);
        kfr::univector<T> in(x.size());
        std::copy(x.begin(),x.end(),in.begin());
        filter.apply(in);
        std::vector<T> r(in.size());
        std::copy(in.begin(),in.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> dcremove(T cutoff, std::vector<T> & in)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> r = DSP::dcremove(i,cutoff);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }

    template<typename T>
    std::vector<T> sinewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::sinewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }
    template<typename T>
    std::vector<T> squarewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::squarewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }
    template<typename T>
    std::vector<T> trianglewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::trianglewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }
    template<typename T>
    std::vector<T> sawtoothewave(size_t n, T freq, T sample_rate, T phase=(T)0) {        
        kfr::univector<T> r = DSP::squarewave(n,freq,sample_rate,phase);
        std::vector<T> x(r.size());
        std::copy(r.begin(),r.end(),x.begin());
        return x;
    }    

    template<typename T>
    std::vector<T> load_wav(const char * filename)
    {
        kfr::univector<T> out = DSP::load_wav<T>(filename);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> load_mp3(const char * filename)
    {
        kfr::univector<T> out = DSP::load_mp3<T>(filename);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> load_flac(const char * filename)
    {
        kfr::univector<T> out = DSP::load_flac<T>(filename);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    void save_wav(std::vector<T> & v, const char * filename, size_t channels, int sample_type, double sample_rate, bool use_w64=false)
    {
        kfr::univector<T> in(v.size());
        std::copy(v.begin(),v.end(),in.begin());
        DSP::write_wav(in,filename,channels,sample_type,sample_rate,use_w64);
        
    }

    
    template<typename T> 
    std::vector<T> bessel_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselLowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> bessel_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselHighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> bessel_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselBandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> bessel_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::BesselBandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }

    template<typename T> 
    std::vector<T> butterworth_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthLowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> butterworth_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthHighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> butterworth_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthBandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> butterworth_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::ButterworthBandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    
    template<typename T> 
    std::vector<T> chebyshev1_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1LowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev1_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1HighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev1_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1BandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev1_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev1BandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }

    template<typename T> 
    std::vector<T> chebyshev2_lowpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2LowPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev2_highpass(std::vector<T> & in,int order, T cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2HighPassFilter<T> filter(order,cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev2_bandpass(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2BandPassFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    template<typename T> 
    std::vector<T> chebyshev2_bandstop(std::vector<T> & in,int order, T lo_cutoff, T hi_cutoff, T sample_rate)
    {
        kfr::univector<T> x(in.size());
        std::copy(in.begin(),in.end(),x.begin());
        DSP::Chebyshev2BandStopFilter<T> filter(order,lo_cutoff,hi_cutoff,sample_rate);
        filter.apply(x);
        std::vector<T> r(x.size());
        std::copy(x.begin(),x.end(),r.begin());
        return r;
    }
    

    
    template<typename T>
    std::vector<T> lowpass_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::lowpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::lowpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::lowpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::lowpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::lowpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::lowpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::lowpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::highpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::highpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::highpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::highpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::highpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::highpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::highpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::bandpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::bandpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::bandpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::bandpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::bandpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::bandpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> bandpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::bandpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::bandpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::bandpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::bandpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }

    template<typename T>
    std::vector<T> notch1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::notchfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> notch2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::notch_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::notch_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> notch3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::notch_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::notch_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::notch_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> notch4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::notch_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::notch_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::notch_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::notch_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
    template<typename T>
    std::vector<T> allpass1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::allpassfilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> allpass2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> allpass3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::allpass_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> allpass4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::allpass_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::allpass_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }

    template<typename T>
    std::vector<T> peak1_filter(std::vector<T> & in, T freq, T q, T gain)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::peakfilter(i,freq,q,gain);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> peak2_filter(std::vector<T> & in, T freq, T q, T gain)
    {        
        DSP::BiQuadParams<T> f1 = DSP::peak_params(freq,T(0.54119610),gain);
        DSP::BiQuadParams<T> f2 = DSP::peak_params(freq,T(1.3065460),gain);
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> peak3_filter(std::vector<T> & in, T freq, T q, T gain)
    {        
        DSP::BiQuadParams<T> f1 = DSP::allpass_params(freq,T(0.51763809),gain);
        DSP::BiQuadParams<T> f2 = DSP::allpass_params(freq,T(0.70710678),gain);
        DSP::BiQuadParams<T> f3 = DSP::allpass_params(freq,T(1.9318517),gain);
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> peak4_filter(std::vector<T> & in, T freq, T q, T gain)
    {        
        DSP::BiQuadParams<T> f1 = DSP::peak_params(freq,T(0.50979558),gain);
        DSP::BiQuadParams<T> f2 = DSP::peak_params(freq,T(0.60134489),gain);
        DSP::BiQuadParams<T> f3 = DSP::peak_params(freq,T(0.89997622),gain);
        DSP::BiQuadParams<T> f4 = DSP::peak_params(freq,T(2.5629154),gain);
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::lowshelffilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowshelf_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::lowshelf_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowshelf_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::lowshelf_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::lowshelf_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> lowshelf4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::lowshelf_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::lowshelf_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::lowshelf_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::lowshelf_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }

template<typename T>
    std::vector<T> highshelf1_filter(std::vector<T> & in, T freq, T q)
    {
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::highshelffilter(i,freq,q);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highshelf2_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highshelf_params(freq,T(0.54119610));
        DSP::BiQuadParams<T> f2 = DSP::highshelf_params(freq,T(1.3065460));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highshelf3_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highshelf_params(freq,T(0.51763809));
        DSP::BiQuadParams<T> f2 = DSP::highshelf_params(freq,T(0.70710678));
        DSP::BiQuadParams<T> f3 = DSP::highshelf_params(freq,T(1.9318517));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> highshelf4_filter(std::vector<T> & in, T freq, T q)
    {        
        DSP::BiQuadParams<T> f1 = DSP::highshelf_params(freq,T(0.50979558));
        DSP::BiQuadParams<T> f2 = DSP::highshelf_params(freq,T(0.60134489));
        DSP::BiQuadParams<T> f3 = DSP::highshelf_params(freq,T(0.89997622));
        DSP::BiQuadParams<T> f4 = DSP::highshelf_params(freq,T(2.5629154));
        DSP::Biquad<T> filter;
        std::vector<DSP::BiQuadParams<T>> fv = {f1,f2,f3,f4};
        filter.init(fv);
        kfr::univector<T> input(in.size());        
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = filter.apply(input);        
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
    enum class QualityType : int
    {
        draft   = 4,
        low     = 6,
        normal  = 8,
        high    = 10,
        perfect = 12,
    };
    template<typename T> 
    std::vector<T> resample(QualityType quality,std::vector<T> &in, T input_rate, T output_rate)
    {
        kfr::univector<T> input(in.size());
        std::copy(in.begin(),in.end(),input.begin());
        kfr::univector<T> out = DSP::resample(input,(kfr::sample_rate_conversion_quality)quality,output_rate,input_rate);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
   
    enum WindowType 
    {
        rectangular     = 1,
        triangular      = 2,
        bartlett        = 3,
        cosine          = 4,
        hann            = 5,
        bartlett_hann   = 6,
        hamming         = 7,
        bohman          = 8,
        blackman        = 9,
        blackman_harris = 10,
        kaiser          = 11,
        flattop         = 12,
        gaussian        = 13,
        lanczos         = 14,
    };
 
    template<typename T>
    kfr::expression_pointer<T> make_window(WindowType type, size_t s, const T x = -1)
    {
        switch(type)
        {
        case hann: return DSP::make_window_hann_ptr<T>(s);
        case hamming: return DSP::make_window_hamming_ptr<T>(s);
        case blackman: return DSP::make_window_blackman_ptr<T>(s);
        case blackman_harris: return DSP::make_window_blackman_ptr<T>(s);
        case gaussian: return DSP::make_window_gaussian_ptr<T>(s);
        case triangular: return DSP::make_window_triangular_ptr<T>(s);
        case bartlett: return DSP::make_window_bartlett_ptr<T>(s);
        case cosine: return DSP::make_window_cosine_ptr<T>(s);
        case bartlett_hann: return DSP::make_window_bartlett_hann_ptr<T>(s);
        case bohman: return DSP::make_window_bohman_ptr<T>(s);
        case lanczos: return DSP::make_window_lanczos_ptr<T>(s);
        case flattop: return DSP::make_window_flattop_ptr<T>(s);
        case rectangular: return DSP::make_window_rectangular_ptr<T>(s);
        case kaiser: return DSP::make_window_kaiser_ptr<T>(s);
        }
    }
    template<typename T>
    std::vector<T> fir_lowpass(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_lowpass(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> fir_highpass(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_highpass(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> fir_bandpass(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_bandpass(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    template<typename T>
    std::vector<T> fir_bandstop(std::vector<T> & in, size_t num_taps, WindowType type, bool normalize = true)
    {
        kfr::expression_pointer<T> window = make_window<T>(type,num_taps);
        kfr::univector<T> i(in.size());
        std::copy(in.begin(),in.end(),i.begin());
        kfr::univector<T> out = DSP::fir_bandstop(in,num_taps,window,normalize);
        std::vector<T> r(out.size());
        std::copy(out.begin(),out.end(),r.begin());
        return r;
    }
    
    std::vector<std::complex<float>> audio_fft_forward(std::vector<float> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        std::vector<float> temp(s);
        std::vector<float> real(s);
        std::vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        memcpy(temp.data(),in.data(),in.size()*sizeof(float));
        fft.init(in.size());
        fft.fft(temp.data(),real.data(),imag.data());
        std::vector<std::complex<float>> out(s);
        for(size_t i = 0; i < s; i++)
        {
            out[i].real(real[i]);
            out[i].imag(imag[i]);
        }
        return out;
    }

    std::vector<float> audio_fft_inverse(std::vector<std::complex<float>> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        std::vector<float> temp(s);
        std::vector<float> real(s);
        std::vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        for(size_t i = 0; i < in.size(); i++)
        {
            real[i] = in[i].real();
            imag[i] = in[i].imag();
        }
        fft.init(in.size());
        fft.ifft(temp.data(),real.data(),imag.data());        
        return temp;
    }

    std::vector<float> audio_convolve(std::vector<float>& in, size_t block_size, std::vector<float> & ir)
    {
        std::vector<float> out(in.size());
        fftconvolver::FFTConvolver conv;
        conv.init(block_size,ir.data(),ir.size());
        conv.process(in.data(),out.data(),in.size());
        return out;
    }
    template<typename T>
    // r = frac
    // x = [i]
    // y = [i+1]
    T linear_interpolate(T x, T y, T r)
    {        
        return r +*x (1.0-r)*y;
        
    }
    template<typename T>
    T cubic_interpolate(T finpos, T xm1, T x0, T x1, T x2)
    {
        //T xm1 = x [inpos - 1];
        //T x0  = x [inpos + 0];
        //T x1  = x [inpos + 1];
        //T x2  = x [inpos + 2];
        T a = (3 * (x0-x1) - xm1 + x2) / 2;
        T b = 2*x1 + xm1 - (5*x0 + x2) / 2;
        T c = (x1 - xm1) / 2;
        return (((a * finpos) + b) * finpos + c) * finpos + x0;
    }
    // original
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite1(T x, T y0, T y1, T y2, T y3)
    {
        // 4-point, 3rd-order Hermite (x-form)
        T c0 = y1;
        T c1 = 0.5f * (y2 - y0);
        T c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
        T c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // james mccartney
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite2(T x, T y0, T y1, T y2, T y3)
    {
        // 4-point, 3rd-order Hermite (x-form)
        T c0 = y1;
        T c1 = 0.5f * (y2 - y0);
        T c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
        T c2 = y0 - y1 + c1 - c3;
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // james mccartney
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite3(T x, T y0, T y1, T y2, T y3)
    {
            // 4-point, 3rd-order Hermite (x-form)
            T c0 = y1;
            T c1 = 0.5f * (y2 - y0);
            T y0my1 = y0 - y1;
            T c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
            T c2 = y0my1 + c1 - c3;

            return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // laurent de soras
    template<typename T>
    // x[i-1]
    // x[i]
    // x[i+1]
    // x[i+2]    
    inline T hermite4(T frac_pos, T xm1, T x0, T x1, T x2)
    {
        const T    c     = (x1 - xm1) * 0.5f;
        const T    v     = x0 - x1;
        const T    w     = c + v;
        const T    a     = w + v + (x2 - x0) * 0.5f;
        const T    b_neg = w + a;

        return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
    }

    /// Sndfile   
    /// Resampler
    /// SpeexResampler

    template<typename T>
    std::vector<T> upsample2x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()*2);
        zeros(out);
        for(size_t i = 0; i < in.size(); i++)
            out[i*2] = in[i];
        return out;
    }
    template<typename T>
    std::vector<T> upsample4x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()*4);
        zeros(out);
        for(size_t i = 0; i < in.size(); i++)
            out[i*4] = in[i];
        return out;
    }
    template<typename T>
    std::vector<T> downsample2x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()/2);
        zeros(out);
        for(size_t i = 0; i < in.size()/2; i++)
            out[i] = in[i*2];
        return out;
    }
    template<typename T>
    std::vector<T> downsample4x(std::vector<T> & in)
    {
        std::vector<T> out(in.size()/4);
        zeros(out);
        for(size_t i = 0; i < in.size()/4; i++)
            out[i] = in[i*4];
        return out;
    }

    // Resamplers 
    // Interpolators 
    // Decimators 
    // DSPFilters
    // Spuce
    // Filters
    // FFTW3
    // Integrators 
    // Differencers 
    // Autodiff 
    // Numerical Integrator (Runge-Kutta)   
    // RingBuffer 
    // CircularBuffer
    // Splines and Beziers
    // Vector Integrator
    // C++ Random 
    // C++ Std
    
}
