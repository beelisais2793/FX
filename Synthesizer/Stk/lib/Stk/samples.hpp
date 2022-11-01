#pragma once

#include "SampleVector.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace SoundWave
{
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
    void set_channel(size_t ch, const SoundWave::SampleVector<T> & in, std::vector<T> & out) {
        size_t x = 0;
        for(size_t i = ch; i < out.size(); i+=2) out[i] = in[x++];
    }
    template<typename T>
    void set_channel(size_t ch, const SoundWave::SampleVector<T> & in, SoundWave::SampleVector<T> & out) {
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
    std::vector<T> interleave(size_t n, size_t channels, const T** & in) {
        std::vector<T> r(n*channels);
        for(size_t i = 0; i < channels; i++)
            for(size_t j = 0; j < n; j++)
                r[j*channels + i] = in[i][j];
        return r;
    }

    template<typename T>
    SoundWave::SampleVector<T> get_left_channel(const SoundWave::SampleVector<T> & in) {
        SoundWave::SampleVector<T> r(in.size()/2);
        size_t x = 0;
        for(size_t i = 0; i < in.size(); i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    SoundWave::SampleVector<T> get_right_channel(const SoundWave::SampleVector<T> & in) {
        SoundWave::SampleVector<T> r(in.size()/2);
        size_t x = 0;
        for(size_t i = 1; i < in.size(); i+=2) r[x++] = in[i];
        return r;
    }
    template<typename T>
    SoundWave::SampleVector<T> get_channel(size_t ch, const SoundWave::SampleVector<T> & in) {
        SoundWave::SampleVector<T> r(in.size()/2);
        size_t x = 0;
        for(size_t i = ch; i < in.size(); i+=2) r[x++] = in[i];
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



    template<typename sample>
    sample dbToLinear(const sample v) {
        return std::pow(10, v / 20.0);
    }
    template<typename sample>
    sample scaledToNormalized(const sample v, const sample min=-1.0, const sample max=1.0)  {
        return ((v - min) / (max - min));
    }
    template<typename sample>
    sample normalizedToScaled(const sample v, const sample min=-1.0, const sample max=1.0)  {
        return min + v * (max - min);
    }
    template<typename sample>
    sample normalizedToExp(const sample v, const sample mAdd, const sample mMul)  {
        return std::exp(mAdd + v * mMul);
    }
    template<typename sample>
    sample expToNormalized(const sample v, const sample mAdd, const sample mMul)  {
        return (std::log(v) - mAdd) / mMul;
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
double product(std::vector<T> v)
{
    if (!isEmpty(v))
    {
        double prod = (double) v[0];

        for (int i = 1;i < v.size();i++)
        {
            prod *= ((double) v[i]);
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
double mean(std::vector<T> v)
{
    // if vector is not empty
    if (!isEmpty(v))
    {
        // store the length of the vector as a double
        double L = (double) v.size();

        // stor the sum of the vector as a double
        double sumVal = (double) sum(v);

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
double median(std::vector<T> v)
{
    // if vector isn't empty
    if (!isEmpty(v))
    {
        double median;
        size_t L = v.size(); // store the size

        // sort the vector
        std::sort(v.begin(), v.end());

        // if the length is even
        if (L  % 2 == 0)
        {
            // take the average of the middle two elements
            median = ((double)(v[L / 2 - 1] + v[L / 2])) / 2.0;
        }
        else // if the length is odd
        {
            // take the middle element
            median = (double) v[(L-1) / 2];
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
double variance(std::vector<T> v)
{
    if (!isEmpty(v))
    {
        // calculate the mean of the vector
        double mu = mean(v);

        double sumVal = 0.0;

        // sum the product of all differences from the mean
        for (int i = 0;i < v.size();i++)
        {
            double diff = v[i]-mu;
            sumVal += diff*diff;
        }

        // return the average of the squared differences
        return sumVal / ((double)v.size());
    }
    else
    {
        throw std::invalid_argument( "Received empty vector when calculating variance" );
    }
}

//============================================================
template <class T>
double standardDeviation(std::vector<T> v)
{
    // if vector is not empty
    if (!isEmpty(v))
    {
        // calculate the variance
        double var = variance(v);

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
double norm1(std::vector<T> v)
{
    double sumVal = 0.0;

    // sum absolute values
    for (int i = 0;i < v.size();i++)
    {
        if (v[i] > 0)
        {
            sumVal += (double) v[i];
        }
        else
        {
            sumVal += (double) (-1*v[i]);
        }
    }

    return sumVal;
}

//============================================================
template <class T>
double norm2(std::vector<T> v)
{
    double sumVal = 0.0;

    // sum squares
    for (int i = 0;i < v.size();i++)
    {
        sumVal += (double) (v[i]*v[i]);
    }

    return std::sqrt(sumVal);
}

//============================================================
template <class T>
double magnitude(std::vector<T> v)
{
    // just another name for L2-norm
    return norm2(v);
}

//============================================================
template <class T>
double normP(std::vector<T> v,double p)
{
    double sumVal = 0.0;

    for (int i = 0;i < v.size();i++)
    {
        double val;

        if (v[i] > 0)
        {
            val = (double) v[i];
        }
        else
        {
            val = (double) (-1*v[i]);
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
            v[i] = (T) std::sqrt((double)v[i]);
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
            result.push_back((T) std::sqrt((double)v[i]));
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
std::vector<double> scale(std::vector<T> v,double lowerLimit,double upperLimit)
{
    std::vector<double> result;

    double minVal = (double) min(v);
    double maxVal = (double) max(v);
    double outputRange = upperLimit - lowerLimit;
    double inputRange = maxVal - minVal;

    for (int i = 0;i < v.size();i++)
    {
        double value = (double) v[i];
        double scaledValue = ((value - minVal) * outputRange) / inputRange + lowerLimit;

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
double dotProduct(std::vector<T> v1,std::vector<T> v2)
{
    // if vector size is the same
    if (v1.size() == v2.size())
    {
        double sumVal = 0.0;

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
double euclideanDistance(std::vector<T> v1,std::vector<T> v2)
{
    // if vector size is the same
    if (v1.size() == v2.size())
    {
        double sumVal = 0.0;

        // sum the squared difference
        for (int i = 0;i < v1.size();i++)
        {
            double diff = (double) (v1[i] - v2[i]);
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
double cosineSimilarity(std::vector<T> v1,std::vector<T> v2)
{
   return dotProduct(v1, v2) / (magnitude(v1) * magnitude(v2));
}

//============================================================
template <class T>
double cosineDistance(std::vector<T> v1,std::vector<T> v2)
{
    return 1.0 - cosineSimilarity(v1, v2);
}

}
