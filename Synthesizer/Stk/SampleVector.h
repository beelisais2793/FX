#pragma once

#include <Eigen/Eigen>
#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <valarray>

namespace SoundWave
{
    
    template<typename T>
    using EigenMatrix  = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;    
    template<typename T>
    using EigenArray   = Eigen::Array<T,1,Eigen::Dynamic,Eigen::RowMajor>;
    template<typename T>
    using EigenArray2D = Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;
    template<typename T>
    using EigenVector  = Eigen::Matrix<T,1,Eigen::Dynamic,Eigen::RowMajor>;
    template<typename T>
    using EigenColMatrix  = Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::ColMajor>;        
    template<typename T>
    using EigenColVector  = Eigen::Matrix<T,Eigen::Dynamic,1,Eigen::ColMajor>;

    typedef float SampleType;

    template<typename T> struct Matrix;
    template<typename T> struct ColMatrix;

    template<typename T>
    struct Scalar 
    {
        T val;

        Scalar(const T& v) { val = v; }
        Scalar(const Scalar<T> & s) { val = s.val; }

        Scalar<T>& operator = (const Scalar<T> & s)
        {
            val = s.val;
            return *this;
        }
        Scalar<T>& operator = (const T & s)
        {
            val = s;
            return *this;
        }
        
        Scalar<T> operator + (const Scalar<T> & s)
        {
            return Scalar<T>(val + s.val);
        }
        Scalar<T> operator - (const Scalar<T> & s)
        {
            return Scalar<T>(val - s.val);
        }
        Scalar<T> operator * (const Scalar<T> & s)
        {
            return Scalar<T>(val * s.val);
        }
        Scalar<T> operator / (const Scalar<T> & s)
        {
            return Scalar<T>(val / s.val);
        }

        Scalar<T> operator + (const T & s)
        {
            return Scalar<T>(val + s);
        }
        Scalar<T> operator - (const T & s)
        {
            return Scalar<T>(val - s);
        }
        Scalar<T> operator * (const T & s)
        {
            return Scalar<T>(val * s);
        }
        Scalar<T> operator / (const T & s)
        {
            return Scalar<T>(val / s);
        }

        void aplus(const T & v) { val += v; }
        void aminus(const T & v) { val -= v; }
        void atimes(const T & v) { val *= v; }
        void adiv(const T & v) { val /= v; }

        void aplus(const Scalar<T> & v) { val += v.val; }
        void aminus(const Scalar<T> & v) { val -= v.val; }
        void atimes(const Scalar<T> & v) { val *= v.val; }
        void adiv(const Scalar<T> & v) { val /= v.val; }

        T    get_value() { return val; }
        void set_value(const T & v) { val = v; }
        
        
        void print() { std::cout << val << std::endl; }  
    };

    template<typename T>
    bool operator > (const Scalar<T> & a, const Scalar<T> & b) {
        return a.val > b.val;
    }

    template<typename T>
    Scalar<T> make_scalar(T val) {
        return Scalar<T>(val);
    }

    template<typename T>
    Scalar<T> operator + (const Scalar<T> & a, const Scalar<T> & b)
    {
        return a+b;
    }
    template<typename T>
    Scalar<T> operator - (const Scalar<T> & a, const Scalar<T> & b)
    {
        return a-b;
    }
    template<typename T>
    Scalar<T> operator * (const Scalar<T> & a, const Scalar<T> & b)
    {
        return a*b;
    }
    template<typename T>
    Scalar<T> operator / (const Scalar<T> & a, const Scalar<T> & b)
    {
        return a/b;
    }
        

    template<typename T> Scalar<T> abs(const Scalar<T> & s)  { 
                return Scalar<T>(std::abs((double)s.val));
        }    
    template<typename T> Scalar<T> fabs(const Scalar<T> & s) { 
            return Scalar<T>(std::fabs((double)s.val));         
        }

    template<typename T>
    Scalar<T> acos(const Scalar<T> & scalar) { return std::acos(scalar.val); }
    template<typename T>
    Scalar<T> acosh(const Scalar<T> & scalar) { return std::acosh(scalar.val); }
    template<typename T>
    Scalar<T> asinh(const Scalar<T> & scalar) { return std::asinh(scalar.val); }
    template<typename T>
    Scalar<T> atan(const Scalar<T> & scalar) { return std::atan(scalar.val); }
    template<typename T>
    Scalar<T> atan2(const Scalar<T> & scalar,const Scalar<T>& y) { return std::atan2(scalar.val,y.val); }
    template<typename T>
    Scalar<T> atanh(const Scalar<T> & scalar) { return std::atanh(scalar.val); }
    template<typename T>
    Scalar<T> cbrt(const Scalar<T> & scalar) { return std::cbrt(scalar.val); }
    template<typename T>
    Scalar<T> ceil(const Scalar<T> & scalar) { return std::ceil(scalar.val); }    
    template<typename T>
    Scalar<T> copysign(const Scalar<T> & scalar,const Scalar<T>& y) { return std::copysign(scalar.val,y.val); }
    template<typename T>
    Scalar<T> cos(const Scalar<T> & scalar) { return std::cos(scalar.val); }
    template<typename T>
    Scalar<T> cosh(const Scalar<T> & scalar) { return std::cosh(scalar.val); }
    template<typename T>
    Scalar<T> erf(const Scalar<T> & scalar) { return std::erf(scalar.val); }
    template<typename T>
    Scalar<T> erfc(const Scalar<T> & scalar) { return std::erfc(scalar.val); }
    template<typename T>
    Scalar<T> exp(const Scalar<T> & scalar) { return std::exp(scalar.val); }
    template<typename T>
    Scalar<T> exp2(const Scalar<T> & scalar) { return std::exp2(scalar.val); }
    template<typename T>
    Scalar<T> expm1(const Scalar<T> & scalar) { return std::expm1(scalar.val); }
    template<typename T>
    Scalar<T> fdim(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fdim(scalar.val,y.val); }
    template<typename T>
    Scalar<T> floor(const Scalar<T> & scalar) { return std::floor(scalar.val); }
    template<typename T>
    Scalar<T> fma(const Scalar<T> & scalar,const Scalar<T> & y, const Scalar<T>& z) { return std::fma(scalar.val,y.val,z.val); }
    template<typename T>
    Scalar<T> fmax(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmax(scalar.val,y.val); }
    template<typename T>
    Scalar<T> fmin(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmin(scalar.val,y.val); }
    template<typename T>
    Scalar<T> fmod(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmod(scalar.val,y.val); }
    template<typename T>
    int fpclassify(const Scalar<T> & scalar) { return std::fpclassify(scalar.val); }
    template<typename T>
    Scalar<T> hypot(const Scalar<T> & scalar,const Scalar<T> & y) { return std::hypot(scalar.val,y.val); }
    template<typename T>
    int ilogb(const Scalar<T> & scalar) { return std::ilogb(scalar.val); }
    template<typename T>
    bool isfinite(const Scalar<T> & scalar) { return std::isfinite(scalar.val); }
    template<typename T>
    bool isgreater(const Scalar<T> & scalar,const Scalar<T> & y) { return std::isgreater(scalar.val,y.val); }
    template<typename T>
    bool isgreaterequal(const Scalar<T> & scalar,const Scalar<T> & y) { return std::isgreaterequal(scalar.val,y.val); }
    template<typename T>
    bool isinf(const Scalar<T> & scalar) { return std::isinf(scalar.val); }
    template<typename T>
    bool isless(const Scalar<T> & scalar,const Scalar<T> & y) { return std::isless(scalar.val,y.val); }
    template<typename T>
    bool islessequal(const Scalar<T> & scalar,const Scalar<T> & y) { return std::islessequal(scalar.val,y.val); }
    template<typename T>
    bool islessgreater(const Scalar<T> & scalar,const Scalar<T> & y) { return std::islessgreater(scalar.val,y.val); }
    template<typename T>
    bool isnan(const Scalar<T> & scalar) { return std::isnan(scalar.val); }
    template<typename T>
    bool isnormal(const Scalar<T> & scalar) { return std::isnormal(scalar.val); }
    template<typename T>
    bool isunordered(const Scalar<T> & scalar,const Scalar<T>& y) { return std::isunordered(scalar.val,y.val); }
    template<typename T>
    Scalar<T> ldexp( const Scalar<T> & scalar,int exp) { return std::ldexp(scalar.val,exp); }
    template<typename T>
    Scalar<T> lgamma(const Scalar<T> & scalar) { return std::lgamma(scalar.val); }
    template<typename T>
    Scalar<T> llrint(const Scalar<T> & scalar) { return std::llrint(scalar.val); }
    template<typename T>
    Scalar<T> llround(const Scalar<T> & scalar) { return std::llround(scalar.val); }
    template<typename T>
    Scalar<T> log(const Scalar<T> & scalar) { return std::log(scalar.val); }
    template<typename T>
    Scalar<T> log10(const Scalar<T> & scalar) { return std::log10(scalar.val); }
    template<typename T>
    Scalar<T> log1p(const Scalar<T> & scalar) { return std::log1p(scalar.val); }
    template<typename T>
    Scalar<T> log2(const Scalar<T> & scalar) { return std::log2(scalar.val); }
    template<typename T>
    Scalar<T> logb(const Scalar<T> & scalar) { return std::logb(scalar.val); }
    template<typename T>
    Scalar<T> lrint(const Scalar<T> & scalar) { return std::lrint(scalar.val); }
    template<typename T>
    Scalar<T> lround(const Scalar<T> & scalar) { return std::lround(scalar.val); }
    template<typename T>
    Scalar<T> nan(const char *tagp) { return std::nan(tagp);}
    template<typename T>
    Scalar<T> nanf(const char *tagp) { return std::nanf(tagp);}
    template<typename T>
    Scalar<T> nanl(const char *tagp) { return std::nanl(tagp);}
    template<typename T>
    Scalar<T> nearbyint(const Scalar<T> & scalar) { return std::nearbyint(scalar.val); }
    template<typename T>
    Scalar<T> nextafter(const Scalar<T> & scalar,const Scalar<T> & y) { return std::nextafter(scalar.val,y.val); }
    template<typename T>
    Scalar<T> nexttoward(const Scalar<T> & scalar,const Scalar<T> & y) { return std::nexttoward(scalar.val,y.val); }
    template<typename T>
    Scalar<T> pow(const Scalar<T> & scalar,const Scalar<T> & e) { return std::pow(scalar.val,e.val); }
    template<typename T>
    Scalar<T> remainder(const Scalar<T> & scalar,const Scalar<T> & d) { return std::remainder(scalar.val,d.val); }
    template<typename T>
    Scalar<T> rint(const Scalar<T> & scalar) { return std::rint(scalar.val); }
    template<typename T>
    Scalar<T> round(const Scalar<T> & scalar) { return std::round(scalar.val); }
    template<typename T>
    Scalar<T> scalbln(const Scalar<T> & scalar,long int n) { return std::scalbln(scalar.val,n);}
    template<typename T>
    Scalar<T> scalbn(const Scalar<T> & scalar,int n) { return std::scalbln(scalar.val,n);}
    template<typename T>
    Scalar<T> sin(const Scalar<T> & scalar) { return std::sin(scalar.val); }
    template<typename T>
    Scalar<T> sinh(const Scalar<T> & scalar) { return std::sinh(scalar.val); }    
    template<typename T>
    Scalar<T> sqrt(const Scalar<T> & scalar) { return std::sqrt(scalar.val); }
    template<typename T>
    Scalar<T> square(const Scalar<T> & scalar) { return scalar.val*scalar.val; }
    template<typename T>
    Scalar<T> cube(const Scalar<T> & scalar) { return scalar.val*scalar.val*scalar.val; }
    template<typename T>
    Scalar<T> tan(const Scalar<T> & scalar) { return std::tan(scalar.val); }
    template<typename T>
    Scalar<T> tanh(const Scalar<T> & scalar) { return std::tanh(scalar.val); }        
    template<typename T>
    Scalar<T> tgamma(const Scalar<T> & scalar) { return std::tgamma(scalar.val); }    
    template<typename T>
    Scalar<T> trunc(const Scalar<T> & scalar) { return std::trunc(scalar.val); }

    template<typename T>
    struct ArrayVector
    {        
        std::valarray<T> vector;
        size_t channels;

        ArrayVector() {
            channels = 1;
        }
        ArrayVector(size_t size, size_t channels = 1) {
            vector.resize(size * channels);
            this->channels = channels;
        }
        ArrayVector<T>(const std::vector<T> & s, size_t chans = 1) {
            vector = s;
            channels = chans;
        }
        ArrayVector<T>(const ArrayVector<T> & s) {
            vector = s.vector;
            channels = s.channels;
        }

        size_t size() const { return vector.size(); }
        size_t num_channels() const { return channels; } 

        ArrayVector<T>& operator = (const ArrayVector<T> & v) {
            vector = v.vector;
            channels  = v.channels;
            return *this;
        }
        T& operator()(size_t i, size_t ch) {
            return vector[i*channels + ch];
        }

        ArrayVector<T> get_channel(size_t channel) {
            //ArrayVector<T> r(*this);
            //if(channels == 1) return r;
            ArrayVector<T> r(*this);
            r.vector = vector[std::slice(0,vector.size(),channel-1)];
            return r;
        }
        void set_channel(const ArrayVector<T> & v, size_t ch) {
            //size_t x = 0;
            //for(size_t i = ch; i < vector.size(); i+=channels) {
            //    vector[i] = v[x++];
            //}
            std::valarray<std::size_t> idx;
            idx.resize(size()/num_channels);
            size_t x = 0;
            for(size_t i = ch-1; i < size(); i+=channels) idx[x++] = i;
            vector[idx] = v.vector;
        }

        bool operator == (const ArrayVector<T> & s) {
            return s.channels == channels && vector.size() == s.vector.size();
        }

        ArrayVector<T> operator + (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector += s.vector;
            return r;
        }
        ArrayVector<T> operator - (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector -= s.vector;
            return r;
        }
        ArrayVector<T> operator * (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector *= s.vector;
            return r;
        }
        ArrayVector<T> operator / (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector /= s.vector;
            return r;
        }
        ArrayVector<T> operator % (const ArrayVector<T> &s) {
            assert(*this == s);
            ArrayVector<T> r(*this);
            r.vector %= s.vector;
            return r;
        }

        ArrayVector<T> operator + (const T s) {    
            ArrayVector<T> r(*this);
            r.vector += s;
            return r;
        }
        ArrayVector<T> operator - (const T s) {        
            ArrayVector<T> r(*this);
            r.vector -= s;
            return r;
        }
        ArrayVector<T> operator / (const T s) {        
            ArrayVector<T> r(*this);
            r.vector /= s;
            return r;
        }
        ArrayVector<T> operator * (const T s) {        
            ArrayVector<T> r(*this);
            r.vector *= s;
            return r;
        }
        ArrayVector<T> operator % (const T s) {        
            ArrayVector<T> r(*this);
            r.vector %= s;
            return r;
        }


        ArrayVector<T>& operator += (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector += s.vector;
            return *this;
        }
        ArrayVector<T>& operator -= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector -= s.vector;
            return *this;
        }
        ArrayVector<T>& operator *= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector *= s.vector;
            return *this;
        }
        ArrayVector<T>& operator /= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector /= s.vector;
            return *this;
        }
        ArrayVector<T>& operator %= (const ArrayVector<T>& s) {        
            assert(*this == s);
            vector %= s.vector;
            return *this;
        }

        ArrayVector<T> operator += (const T s) {
            vector += s;
            return *this;
        }
        ArrayVector<T> operator -= (const T s) {
            vector -= s;
            return *this;
        }
        ArrayVector<T> operator /= (const T s) {
            vector /= s;
            return *this;
        }
        ArrayVector<T> operator *= (const T s) {
            vector *= s;
            return *this;
        }
        ArrayVector<T> operator %= (const T s) {        
            vector %= s;
            return *this;
        }

        T sum() { return vector.sum(); }
        T min() { return vector.min(); }
        T max() { return vector.max(); }

        void resize(size_t s, size_t c) {
            channels = c;
            vector.resize(s * c);
        }

        ArrayVector<T> abs() { 
            ArrayVector<T> r(*this);
            r.vector = std::abs(vector);
            return r;
        }
        ArrayVector<T> exp() { 
            ArrayVector<T> r(*this);
            r.vector = std::exp(vector);
            return r;
        }
        ArrayVector<T> log() { 
            ArrayVector<T> r(*this);
            r.vector = std::log(vector);
            return r;
        }
        ArrayVector<T> log10() { 
            ArrayVector<T> r(*this);
            r.vector = std::log10(vector);
            return r;
        }
        ArrayVector<T> pow(const ArrayVector<T> & s) { 
            ArrayVector<T> r(*this);
            r.vector = std::pow(vector,s.vector);
            return r;
        }
        ArrayVector<T> pow(const T s) { 
            ArrayVector<T> r(*this);
            r.vector = std::pow(vector,s);        
            return r;        
        }
        ArrayVector<T> sqrt() {
            ArrayVector<T> r(*this);
            r.vector = std::sqrt(vector);
            return r;
        }
        ArrayVector<T> sin() {
            ArrayVector<T> r(*this);
            r.vector = std::sin(vector);
            return r;
        }
        ArrayVector<T> cos() {
            ArrayVector<T> r(*this);
            r.vector = std::cos(vector);
            return r;
        }
        ArrayVector<T> tan() {
            ArrayVector<T> r(*this);
            r.vector = std::tan(vector);
            return r;
        }
        ArrayVector<T> asin() {
            ArrayVector<T> r(*this);
            r.vector = std::asin(vector);
            return r;
        }
        ArrayVector<T> acos() {
            ArrayVector<T> r(*this);
            r.vector = std::acos(vector);
            return r;
        }
        ArrayVector<T> atan() {
            ArrayVector<T> r(*this);
            r.vector = std::atan(vector);
            return r;
        }
        ArrayVector<T> atan2(const T s) {
            ArrayVector<T> r(*this);
            r.vector = std::atan2(vector,s);
            return r;
        }
        ArrayVector<T> sinh() {
            ArrayVector<T> r(*this);
            r.vector = std::sinh(vector);
            return r;
        }
        ArrayVector<T> cosh() {
            ArrayVector<T> r(*this);
            r.vector = std::cosh(vector);
            return r;
        }
        ArrayVector<T> tanh() {
            ArrayVector<T> r(*this);
            r.vector = std::tanh(vector);
            return r;
        }
    };

    template<typename T> ArrayVector<T> abs(ArrayVector<T> & a) { return a.abs(); }
    template<typename T> ArrayVector<T> exp(ArrayVector<T> & a) { return a.exp(); }           
    template<typename T> ArrayVector<T> log(ArrayVector<T> & a) { return a.log(); } 
    template<typename T> ArrayVector<T> log10(ArrayVector<T> & a) { return a.log10(); }
    template<typename T> ArrayVector<T> pow(ArrayVector<T> & a, const ArrayVector<T> & s) { return a.pow(s); }
    template<typename T> ArrayVector<T> pow(ArrayVector<T> & a, const T s) { return a.pow(s); }
    template<typename T> ArrayVector<T> sqrt(ArrayVector<T> & a) { return a.sqrt(); }
    template<typename T> ArrayVector<T> sin(ArrayVector<T> & a) { return a.sin(); }
    template<typename T> ArrayVector<T> cos(ArrayVector<T> & a) { return a.cos(); }
    template<typename T> ArrayVector<T> tan(ArrayVector<T> & a) { return a.tan(); }
    template<typename T> ArrayVector<T> asin(ArrayVector<T> & a) { return a.asin(); }
    template<typename T> ArrayVector<T> acos(ArrayVector<T> & a) { return a.acos(); }
    template<typename T> ArrayVector<T> atan(ArrayVector<T> & a) { return a.atan(); }
    template<typename T> ArrayVector<T> atan2(ArrayVector<T> & a, const T s) { return a.atan2(s); }
    template<typename T> ArrayVector<T> sinh(ArrayVector<T> & a) { return a.sinh(); }
    template<typename T> ArrayVector<T> cosh(ArrayVector<T> & a) { return a.cosh(); }
    template<typename T> ArrayVector<T> tanh(ArrayVector<T> & a) { return a.tanh(); }

    template<typename T>
    struct ArrayMatrix {
        std::vector<ArrayVector<T>> channels;
        ArrayMatrix(size_t chan, size_t samps) {
            channels.resize(chan);
            for(size_t i = 0; i < chan; i++)
                channels[i].resize(chan,samps);
        }
        ArrayMatrix(const ArrayVector<T> & v) {
            channels.resize(v.num_channels());
            for(size_t i = 0; i < v.num_channels(); i++) 
                channels[i] = v.get_channel(i);
        }
        ArrayMatrix(const ArrayMatrix<T> & m) {
            channels = m.channels;
        }

        void resize(size_t chan, size_t samps) {
            channels.resize(chan);
            for(size_t i = 0; i < chan; i++)
                channels[i].resize(samps);

        }
        size_t num_channels() const { return channels.size(); }
        size_t samples_per_channel(size_t ch = 0) const { return channels[ch].size()/channels.size(); }
        size_t size(size_t ch=0) const { return channels[ch].size(); }

        void deinterleave(const ArrayVector<T> & s) {            
            resize(s.num_channels(),s.samples_per_channel());                        
            for(size_t i = 0; i < s.num_channels(); i++) {
                channels.row(i) = s.get_channel(i).vector;
            }            
        }    
        ArrayVector<T> interleave() { 
            ArrayVector<T> r;
            r.resize(samples_per_channel(),num_channels());
            for(size_t i = 0; i < channels.size(); i++)
                r.set_channel(channels[i]);
            return r;
        }

        
        ArrayVector<T>& operator[](size_t ch) { return channels[ch]; }
        ArrayVector<T>& get_channel(size_t c) { return channels[c]; }
        void set_channel(size_t c, const ArrayVector<T> & s) { channels[c] = s; }

        ArrayMatrix<T>& operator = (const ArrayMatrix<T> & v) {        
            channels  = v.channels;
            return *this;
        }
        T& operator()(size_t c, size_t n) {
            return channels[c][n];
        }

        bool check(const ArrayMatrix<T> & b) {
            if(channels.size() != b.channels.size()) return false;
            for(size_t i = 0; i < channels.size(); i++)
                if(channels[i].size() != b.channels[i].size()) return false;
            return true;
        }
        bool operator == (const ArrayMatrix<T> & b) {
            return check(b);
        }
        ArrayMatrix<T> operator + (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] +  m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator - (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] -  m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator * (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] *  m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator / (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] / m.channels[i];
            return r;
        }
        ArrayMatrix<T> operator % (const ArrayMatrix<T> & m) {
            assert(*this == m);
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < m.channels.size(); i++)
                r.channels[i] = channels[i] % m.channels[i];
            return r;
        }

        ArrayMatrix<T> operator + (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] + s;
            return r;
        }
        ArrayMatrix<T> operator - (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] - s;
            return r;
        }
        ArrayMatrix<T> operator * (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] * s;
            return r;
        }
        ArrayMatrix<T> operator / (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] / s;
            return r;
        }
        ArrayMatrix<T> operator % (const T s) {            
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i] % s;
            return r;
        }


        ArrayMatrix<T> abs() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].abs();
            return r;
        }
        ArrayMatrix<T> exp() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].exp();
            return r;
        }
        ArrayMatrix<T> log() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].log();
            return r;
        }
        ArrayMatrix<T> log10() { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].log10();
            return r;
        }
        ArrayMatrix<T> pow(const ArrayVector<T> & s) { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].pow(s);
            return r;
        }
        ArrayMatrix<T> pow(const T s) { 
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].pow(s);
            return r;
        }
        ArrayMatrix<T> sqrt() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].sqrt();
            return r;
        }
        ArrayMatrix<T> sin() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].sin();
            return r;
        }
        ArrayMatrix<T> cos() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].cos();
            return r;
        }
        ArrayMatrix<T> tan() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].tan();
            return r;
        }
        ArrayMatrix<T> asin() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].asin();
            return r;
        }
        ArrayMatrix<T> acos() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].acos();
            return r;
        }
        ArrayMatrix<T> atan() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].atan();
            return r;
        }
        ArrayMatrix<T> atan2(const T s) {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].atan2(s);
            return r;
        }
        ArrayMatrix<T> sinh() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].sinh();
            return r;
        }
        ArrayMatrix<T> cosh() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].cosh();
            return r;
        }
        ArrayMatrix<T> tanh() {
            ArrayMatrix<T> r(*this);
            for(size_t i = 0; i < channels.size(); i++)
                r.channels[i] = channels[i].tanh();
            return r;
        }
    };

    template<typename T> ArrayMatrix<T> abs(ArrayMatrix<T> & a) { return a.abs(); }
    template<typename T> ArrayMatrix<T> exp(ArrayMatrix<T> & a) { return a.exp(); }           
    template<typename T> ArrayMatrix<T> log(ArrayMatrix<T> & a) { return a.log(); } 
    template<typename T> ArrayMatrix<T> log10(ArrayMatrix<T> & a) { return a.log10(); }    
    template<typename T> ArrayMatrix<T> pow(ArrayMatrix<T> & a, const T s) { return a.pow(s); }
    template<typename T> ArrayMatrix<T> sqrt(ArrayMatrix<T> & a) { return a.sqrt(); }
    template<typename T> ArrayMatrix<T> sin(ArrayMatrix<T> & a) { return a.sin(); }
    template<typename T> ArrayMatrix<T> cos(ArrayMatrix<T> & a) { return a.cos(); }
    template<typename T> ArrayMatrix<T> tan(ArrayMatrix<T> & a) { return a.tan(); }
    template<typename T> ArrayMatrix<T> asin(ArrayMatrix<T> & a) { return a.asin(); }
    template<typename T> ArrayMatrix<T> acos(ArrayMatrix<T> & a) { return a.acos(); }
    template<typename T> ArrayMatrix<T> atan(ArrayMatrix<T> & a) { return a.atan(); }
    template<typename T> ArrayMatrix<T> atan2(ArrayMatrix<T> & a, const T s) { return a.atan2(s); }
    template<typename T> ArrayMatrix<T> sinh(ArrayMatrix<T> & a) { return a.sinh(); }
    template<typename T> ArrayMatrix<T> cosh(ArrayMatrix<T> & a) { return a.cosh(); }
    template<typename T> ArrayMatrix<T> tanh(ArrayMatrix<T> & a) { return a.tanh(); }

    template<typename T> ArrayMatrix<T> interleave(const std::vector<ArrayVector<T>> & c) {
        ArrayMatrix<T> m(c.size(),c[0].size());
        for(size_t i = 0; i < c.size(); i++)
            m.set_channel(i,c[i]);
        return m;
    }




    template<typename T>
    struct Vector 
    {    
        EigenVector<T> vector;
        
        Vector() {}
        Vector(size_t i) { vector = EigenVector<T>(i); }
        Vector(size_t i, const T v) { vector = EigenVector<T>(i); vector.fill(v); }
        Vector(const Vector<T> & v) { vector = v.vector;  }
        Vector(const EigenVector<T> & v) { vector = v; }
        Vector(const std::vector<T> & v) { 
            vector.resize(v.size());
            memcpy(vector.data(),v.data(),v.size()*sizeof(T));
        }
        T get(int i) { return vector(i); }
        void  set(int i, T v) { vector(i) = v; }
        
        void random(int i) { vector = EigenVector<T>::Random(i); }
        void random() { vector.setRandom(vector.size()); }
        void zero() { vector.setZero(vector.size()); }    
        void ones() { vector.setOnes(vector.size()); }

        size_t  innerStride() { return vector.innerStride(); }
        T*  data() { return vector.data(); }
        void    resize(size_t size) { vector.resize(size); }
        void    resizeLike(const Vector<T> & v) { vector.resizeLike(v.vector); }
        void    conservativeResize(size_t size) { vector.conservativeResize(size); }

        T coeff(size_t i) { return vector.coeff(i); }
        T& coeffRef(size_t i) { return vector.coeffRef(i); }

        void print()     {        std::cout << vector << std::endl;    }
        size_t cols() { return vector.cols(); }
        T dot(const Vector<T> & b) { return vector.dot(b.vector);  }        
        size_t size() const { return vector.size(); }
        void fill(T v) { vector.fill(v); }
        T norm() { return vector.norm(); }
        T squaredNorm() { return vector.squaredNorm(); }

        Vector<T>& eval() { vector.eval(); return *this;  }

        void normalize() {
            vector.normalize();
        }

        Vector<T> normalized() { Vector<T> r; r.vector = vector.normalized(); return r; }            
        Vector<T>& head(Vector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
        Vector<T>& tail(Vector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
        Vector<T>& segment(Vector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
        Vector<T>& LinSpaced(T num, T a, T b)
        {        
            vector = EigenVector<T>::LinSpaced(num,a,b);
            return *this;
        }

        T operator()(size_t i) { return vector(i); }
        T operator[](size_t i) { return vector(i); }

        /*
        T inner_product(const Vector<T> & v)
        { return vector.adjoint() * v.vector; }
        */

        Vector<T> operator + (const Vector<T>& b) 
        { 
            Vector<T> r(*this);
            r.vector = r.vector + b.vector;
            return r;
        }
        Vector<T> operator - (const Vector<T>& b) 
        { 
            Vector<T> r(*this);
            r.vector = r.vector - b.vector;
            return r;
        }    
        
        T operator * (const Vector<T>& b) 
        { 
            return vector.dot(b.vector);        
        }

        
        Vector<T> operator * (const T b) 
        { 
            Vector<T> r(*this);
            r.vector = r.vector * b;
            return r;
        }

        Vector<T> operator / (const T b)  
        { 
            Vector<T> r(*this);
            r.vector = r.vector / b;
            return r;
        }

        
        void aplus(const Vector<T>& b) { vector += b.vector; }
        void aminus(const Vector<T>& b) { vector -= b.vector; }
        void adot(const Vector<T>& b) { vector.dot(b.vector); }
        
        void atimes(const T b) { vector = vector * b; }
        void adiv(const T b) { vector = vector / b; }


        Vector<T> t() { return Vector<T>(vector.transpose().eval());}    
        Vector<T> abs() {  return Vector<T>(vector.cwiseAbs());  }
        Vector<T> inverse() { return Vector<T>(vector.cwiseInverse()); }    
        Vector<T> sqrt() { return Vector<T>(vector.cwiseSqrt()); }
        Vector<T> abs2() { return Vector<T>(vector.cwiseAbs2()); }
        Vector<T> operator * (Matrix<T> & m);

        Vector<T> operator = (const Vector<T> & v) { vector = v.vector; return *this; }
        Vector<T> operator = (const EigenVector<T> & v) { vector = v; return *this; }

        // add to each col
        Matrix<T> operator + (Matrix<T> & m);
        // subtract from each col 
        Matrix<T> operator - (Matrix<T> & m);
        // divide from each col
        Matrix<T> operator / (Matrix<T> & m);

        Matrix<T> matrix();    
    };

    
    template<typename T> Vector<T>  abs(Vector<T> & vector ) { Vector<T>  r; r = vector.abs(); return r;}
    template<typename T> Vector<T>  inverse(Vector<T> & vector ) { Vector<T>  r; r = vector.inverse(); return r; }
    template<typename T> Vector<T>  exp(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().exp(); return r;  }
    template<typename T> Vector<T>  log(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().log(); return r;  }
    template<typename T> Vector<T>  log1p(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().log1p(); return r; }
    template<typename T> Vector<T>  log10(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().log10(); return r; }
    template<typename T> Vector<T>  pow(Vector<T> & vector ,const T b) { Vector<T>  r; r = vector.vector.array().pow(b); return r;}
    template<typename T> Vector<T>  sqrt(Vector<T> & vector ) { Vector<T>  r; r = vector.sqrt(); return r;}
    template<typename T> Vector<T>  rsqrt(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().rsqrt(); return r; }
    template<typename T> Vector<T>  square(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().square(); return r;}
    template<typename T> Vector<T>  cube(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().cube(); return r; }
    template<typename T> Vector<T>  abs2(Vector<T> & vector ) { Vector<T>  r; r = vector.abs2(); return r; }
    template<typename T> Vector<T>  sin(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().sin(); return r;}
    template<typename T> Vector<T>  cos(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().cos(); return r; }
    template<typename T> Vector<T>  tan(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().tan(); return r;}
    template<typename T> Vector<T>  asin(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().asin(); return r; }
    template<typename T> Vector<T>  acos(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().acos(); return r; }
    template<typename T> Vector<T>  atan(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().atan(); return r; }
    template<typename T> Vector<T>  sinh(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().sinh(); return r; }
    template<typename T> Vector<T>  cosh(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().cosh(); return r; }
    template<typename T> Vector<T>  tanh(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().tanh(); return r; }
    template<typename T> Vector<T>  ceil(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().ceil(); return r; }
    template<typename T> Vector<T>  floor(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().floor(); return r; }
    template<typename T> Vector<T>  round(Vector<T> & vector ) { Vector<T>  r; r = vector.vector.array().round(); return r; }

    
    template<typename T>
    class MatrixView 
    {
    private:
        EigenMatrix<T> *matrix;
        size_t row;
        size_t col;
        bool bUseRows;

    public:

        MatrixView(EigenMatrix<T> *m, size_t r)
        {
            matrix = m;
            row = r;
            col = 0;
            bUseRows = true;
        }

        T __getitem(size_t i) { 
            if(bUseRows == true)
                return (*matrix)(row,i); 
            else 
                return (*matrix)(i,col); 
        }
        void  __setitem(size_t i, const T val) { 
            if(bUseRows == true)
                (*matrix)(row,i) = val; 
            else 
                (*matrix)(i,col) = val;
            
        }

        void use_rows() { bUseRows = true; }
        void use_cols() { bUseRows = false; }
        size_t rows() { return matrix->rows(); }
        size_t cols() { return matrix->cols(); }
        void   set_row(size_t r) { row = r; }
        void   set_col(size_t c) { col = c;}
        
    };

    template<typename T>
    struct Matrix
    {
        EigenMatrix<T> matrix;

        Matrix() {}
        Matrix(int x, int y) { matrix = EigenMatrix<T>(x,y); }
        Matrix(int x, int y, const T val) { matrix = EigenMatrix<T>(x,y); matrix.fill(val);  }
        Matrix(const Vector<T> & v) { matrix = v.vector; }        
        Matrix(const Matrix<T>& m) { matrix = m.matrix;  }    
        Matrix(const EigenMatrix<T>& m) { matrix = m; }    
        Matrix(const EigenVector<T> & v) { matrix = v; }    
        Matrix(const EigenArray2D<T> & a) { matrix = a.matrix(); }
        Matrix(const std::vector<T> & data, size_t r, size_t c) {       
            matrix.resize(r,c);
            memcpy(matrix.data(),data.data(),data.size()*sizeof(T));        
        }

        T& operator()(size_t i, size_t j) { return matrix(i,j); }
        
        void random(int x, int y) { matrix =  EigenMatrix<T>::Random(x,y); }
        void random() { matrix.setRandom(matrix.rows(),matrix.cols()); }
        void identity(int x, int y) { matrix = EigenMatrix<T>::Identity(x,y); }
        void identity() { matrix.setIdentity(matrix.rows(),matrix.cols()); }
        void zero(int x, int y) { matrix = EigenMatrix<T>::Zero(x,y); }
        void zero() { matrix.setZero(matrix.rows(),matrix.cols()); }
        void ones(int x, int y) { matrix = EigenMatrix<T>::Ones(x,y); }
        void ones() { matrix.setOnes(matrix.rows(),matrix.cols()); }

        T get(int i, int j) { return matrix(i,j); }
        void set(int i, int j, T v) { matrix(i,j) = v; }        
        T norm() { return matrix.norm(); }
        T squaredNorm() { return matrix.squaredNorm(); }

        bool all() { return matrix.all(); }
        bool allFinite() { return matrix.allFinite(); }
        bool any() { return matrix.any(); }
        bool count() { return matrix.count(); }

        size_t rows() const { return matrix.rows(); }
        size_t cols() const { return matrix.cols(); }
        void resize(int x, int y) { matrix.resize(x,y); }

        T* data() { return matrix.data(); }    
        
        Vector<T> operator[](int index) { 
            if(index < 0) index += rows();
            EigenMatrix<T> r = matrix.row(index);
            return Vector<T>(r);        
        }
        
        void normalize()
        {
            matrix.normalize();
        }
        Matrix<T> normalized() { 
            Matrix<T> r; 
            r.matrix = matrix.normalized(); 
            return r; 
        }    

        void fill(T v) { matrix.fill(v); }
        Matrix<T> eval() { return Matrix<T>(matrix.eval()); }
        bool hasNaN() { return matrix.hasNaN(); }
        size_t innerSize() { return matrix.innerSize(); }
        size_t outerSize() { return matrix.outerSize(); }    
        bool isMuchSmallerThan(const Matrix<T>& n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }
        bool isOnes() { return matrix.isOnes(); }
        bool isZero() { return matrix.isZero(); }
                
        Vector<T> row_vector(size_t row) { Vector<T> r; r.vector = matrix.row(row); return r; }    
        Vector<T> row(size_t row) { 
            Vector<T> r;        
            r.vector = matrix.row(row); 
            return r; 
        }        
        void set_row(size_t row, std::vector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v[i];
        }
        void set_row_vector(size_t row, Vector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v(i);
        }
        void set_row_matrix(size_t row, Matrix<T> & v, size_t src_row)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v.matrix(src_row,i);
        }

                
        void set_col(size_t col, std::vector<T> & v)    {
            assert(rows() == v.size());        
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v[i];
        }
        void set_col_vector(size_t col, Vector<T> & v)    {
            assert(rows() == v.size());        
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v(i);
        }
        void set_col_matrix(size_t col, Matrix<T> & v, size_t row)    {
            assert(rows() == v.cols());
            assert(cols() == v.rows());
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v.matrix(row,i);

        }

        
        Matrix<T> leftCols(Matrix<T>& r, size_t cols) { r.matrix = matrix.leftCols(cols).eval(); return r; }
        Matrix<T> middleCols(Matrix<T>& r, size_t j, size_t cols) { r.matrix = matrix.middleCols(j,cols).eval(); return r; }
        Matrix<T> rightCols(Matrix<T>& r, size_t cols) { r.matrix = matrix.rightCols(cols).eval(); return r; }
        Matrix<T> topRows(Matrix<T>& r, size_t rows) { r.matrix = matrix.topRows(rows).eval(); return r; }
        Matrix<T> middleRows(Matrix<T>& r, size_t j, size_t rows) { r.matrix = matrix.middleRows(j,rows).eval(); return r; }
        Matrix<T> bottomRows(Matrix<T>& r, size_t rows) { r.matrix = matrix.bottomRows(rows).eval(); return r; }
        Matrix<T> topLeftCorner(Matrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topLeftCorner(i,j).eval(); return r; }
        Matrix<T> topRightCorner(Matrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topRightCorner(i,j).eval(); return r; }

        
        Matrix<T> adjoint()  { Matrix<T> m; m.matrix = matrix.adjoint().eval(); return m; }
        Matrix<T> transpose() { Matrix<T> m; m.matrix = matrix.transpose().eval(); return m; }
        Matrix<T> diagonal() { Matrix<T> m; m.matrix = matrix.diagonal().eval(); return m; }        
        Matrix<T> reverse() { Matrix<T> m; m.matrix = matrix.reverse().eval(); return m; }    
        Matrix<T> replicate(size_t i, size_t j) { Matrix<T> m; m.matrix = matrix.replicate(i,j).eval(); return m; }
        
        void adjointInPlace() {  matrix.adjointInPlace(); }    
        void transposeInPlace() { matrix.transposeInPlace(); }
        void reverseInPlace() { matrix.reverseInPlace(); }

        
        Matrix<T> adjointInto(Matrix<T> & m) { m.matrix = matrix.adjoint().eval(); return m; }    
        Matrix<T> transposeInto(Matrix<T> & m) { m.matrix = matrix.transpose().eval();return m; }    
        Matrix<T> diagonalInto(Matrix<T> & m) { m.matrix = matrix.diagonal().eval();return m; }
        Matrix<T> reverseInto(Matrix<T> & m) { m.matrix = matrix.reverse().eval();return m; }
        Matrix<T> replicateInto(Matrix<T> & m,size_t i, size_t j) { m.matrix = matrix.replicate(i,j).eval(); return m; }
        
        /*
        Matrix<T> outer_product(Matrix<T> & m)
        {   return Matrix<T>(matrix * m.t().matrix); }
        */

        T sum()    {        
            return matrix.sum();        
        }
        T prod()    {        
            return matrix.prod();        
        }
        T mean()    {        
            return matrix.mean();        
        }
        T minCoeff()    {        
            return matrix.minCoeff();        
        }
        T maxCoeff()    {        
            return matrix.maxCoeff();        
        }    
        T trace()    {        
            return matrix.trace();        
        }

        
        
        // can't rowwise in Lua at this time.   
        Matrix<T> addToEachRow(Matrix<T> & v)    {
            Matrix<T> r(*this);        
            r.matrix = r.matrix.rowwise() + Vector<T>(v.matrix).vector;
            return r;
        }
        
        Matrix<T> cwiseAbs()    {
            EigenMatrix<T> r = matrix.cwiseAbs();
            return r;
        }
        Matrix<T> cwiseAbs2()    {
            EigenMatrix<T> r = matrix.cwiseAbs2();
            return r;
        }
        Matrix<T> cwiseProduct(const Matrix<T>& q)    {
            EigenMatrix<T> r = matrix.cwiseProduct(q.matrix); 
            return r;
        }
        Matrix<T> cwiseQuotient(const Matrix<T>& q)    {
            EigenMatrix<T> r = matrix.cwiseQuotient(q.matrix); 
            return r;
        }
        Matrix<T> cwiseInverse()    {
            EigenMatrix<T> r = matrix.cwiseInverse();
            return r;
        }
        Matrix<T> cwiseSqrt(Matrix<T>& out)    {
            EigenMatrix<T> r = matrix.cwiseSqrt();
            return r;
        }
        Matrix<T> cwiseMax(Matrix<T>& q)    {
            EigenMatrix<T> r = matrix.cwiseMin(q.matrix);
            return r;        
        }
        Matrix<T> cwiseMin(Matrix<T>& q)    {
            EigenMatrix<T> r = matrix.cwiseMin(q.matrix);
            return r;
        }

        
        bool operator == (const Matrix<T> &m) {
            Matrix<T> r;
            auto x = (matrix == m.matrix);
            return x;
        }
        bool operator != (const Matrix<T> &m) {
            Matrix<T> r;
            auto x = (matrix != m.matrix);
            return x;
        }
        

        Matrix<T>& operator=(const Matrix<T> & m)    {        
            matrix = m.matrix.eval();                    
            return *this;
        }
        Matrix<T>& operator=(const EigenMatrix<T> & m)    {        
            matrix = m;
            return *this;
        }
        Matrix<T> operator * (const Matrix<T> & m)    {
            Matrix<T> r(*this);
            r.matrix = r.matrix * m.matrix;
            return r;
        } 
        Matrix<T> operator + (const Matrix<T> & m)    {
            Matrix<T> r(*this);
            r.matrix = r.matrix + m.matrix;
            return r;
        }
        Matrix<T> operator - (const Matrix<T> & m)    {
            Matrix<T> r(*this);
            r.matrix = r.matrix - m.matrix;
            return r;
        }

        
        Matrix<T> operator - ()     {
            Matrix<T> r(*this);
            r.matrix = -matrix; 
            return r;
        }
        Vector<T> operator * (const Vector<T> & v)    {
            Vector<T> r;
            r.vector = matrix * v.vector;
            return r;
        }
        Vector<T> operator + (const Vector<T> & v)    {
            Vector<T> r;
            r.vector = matrix + v.vector;
            return r;
        }
        Vector<T> operator - (const Vector<T> & v)    {
            Vector<T> r;
            r.vector = matrix - v.vector;
            return r;
        }
        
        Matrix<T> operator * (const T f)    {
            Matrix<T> r(*this);
            r.matrix = r.matrix * f;
            return r;
        }    
        Matrix<T> operator / (const T f)    {
            Matrix<T> r(*this);
            r.matrix = r.matrix / f;
            return r;
        }
            
        void print()     {
            std::cout << matrix << std::endl;
        }
        
        Matrix<T> t()     {
            return transpose();
        }
        

    // slice 

        Matrix<T> slice(int first_r,int first_c, int last_r=-1, int last_c=-1)    {
            if(last_r = -1) last_r = matrix.rows()-1;
            if(last_c = -1) last_c = matrix.cols()-1;
            auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seq(first_c,last_c));
            Matrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        Matrix<T> sliceN1(int first_r,int first_rn, int first_c, int last_c=-1)    {        
            if(last_c = -1) last_c = matrix.cols()-1;
            auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seq(first_c,last_c));
            Matrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        Matrix<T> sliceN2(int first_r,int first_c, int first_cn, int last_r=-1)    {                
            auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seqN(first_c,first_cn));
            Matrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        Matrix<T> slicen(int first_r,int first_rn, int first_c, int first_cn)    {        
            auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seqN(first_c,first_cn));
            Matrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        void rowwise_add(Vector<T> & r) {
            matrix.rowwise() += r.vector;
        }
        void rowwise_sub(Vector<T> & r) {
            matrix.rowwise() -= r.vector;
        }
        void rowwise_mul(Vector<T> & r) {
            matrix = (matrix.array().rowwise() * r.vector.array()).matrix();
        }
        void rowwise_div(Vector<T> & r) {
            matrix = (matrix.array().rowwise() / r.vector.array()).matrix();
        }

        EigenArray2D<T> array() const { return matrix.array(); }
        
    };



    template<typename T> Matrix<T> abs(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().abs().matrix(); return r;}
    template<typename T> Matrix<T> inverse(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().inverse().matrix(); return r; }
    template<typename T> Matrix<T> exp(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().exp().matrix(); return r;  }
    template<typename T> Matrix<T> log(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log().matrix(); return r;  }
    template<typename T> Matrix<T> log1p(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log1p().matrix(); return r; }
    template<typename T> Matrix<T> log10(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().log10().matrix(); return r; }
    template<typename T> Matrix<T> pow(const Matrix<T> & matrix,const T b) { Matrix<T> r; r.matrix = matrix.array().pow(b).matrix(); return r;}
    template<typename T> Matrix<T> sqrt(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().sqrt().matrix(); return r;}
    template<typename T> Matrix<T> rsqrt(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().rsqrt().matrix(); return r; }
    template<typename T> Matrix<T> square(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().square().matrix(); return r;}
    template<typename T> Matrix<T> cube(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().cube().matrix(); return r; }
    template<typename T> Matrix<T> abs2(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().abs2().matrix(); return r; }
    template<typename T> Matrix<T> sin(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().sin().matrix(); return r;}
    template<typename T> Matrix<T> cos(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().cos().matrix(); return r; }
    template<typename T> Matrix<T> tan(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().tan().matrix(); return r;}
    template<typename T> Matrix<T> asin(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().asin().matrix(); return r; }
    template<typename T> Matrix<T> acos(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().acos().matrix(); return r; }
    template<typename T> Matrix<T> atan(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().atan().matrix(); return r; }
    template<typename T> Matrix<T> sinh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().sinh().matrix(); return r; }
    template<typename T> Matrix<T> cosh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().cosh().matrix(); return r; }
    template<typename T> Matrix<T> tanh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().tanh().matrix(); return r; }
    template<typename T> Matrix<T> ceil(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().ceil().matrix(); return r; }
    template<typename T> Matrix<T> floor(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().floor().matrix(); return r; }
    template<typename T> Matrix<T> round(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().round().matrix(); return r; }
    template<typename T> Matrix<T> asinh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().asinh().matrix(); return r; }
    template<typename T> Matrix<T> acosh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().acosh().matrix(); return r; }
    template<typename T> Matrix<T> atanh(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().atanh().matrix(); return r; }
    template<typename T> Matrix<T> rint(const Matrix<T> & matrix) { Matrix<T> r; r.matrix = matrix.array().rint().matrix(); return r; }

    template<typename T> 
    Vector<T> Vector<T>::operator * (Matrix<T> &b) 
    { 
        Vector<T> r;        
        r.vector =  b.matrix * vector;
        return r;
    }
    
    template<typename T> 
    Matrix<T> Vector<T>::operator + (Matrix<T> &b) 
    {     
        Matrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) + vector(j);
        return r;
    }
    
    template<typename T> 
    Matrix<T> Vector<T>::operator - (Matrix<T> &b) 
    {     
        Matrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) - vector(j);
        return r;
    }
    
    template<typename T> 
    Matrix<T> Vector<T>::operator / (Matrix<T> &b) 
    {     
        Matrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) / vector(j);
        return r;
    }

    
    template<typename T> Matrix<T> Vector<T>::matrix() { return Matrix<T>(vector); }

    template<typename T>
    struct ColVector 
    {    
        EigenColVector<T> vector;
        
        ColVector() {}
        ColVector(size_t i) { vector = EigenColVector<T>(i); }
        ColVector(size_t i, const T v) { vector = EigenColVector<T>(i); vector.fill(v); }
        ColVector(const ColVector<T> & v) { vector = v.vector;  }
        ColVector(const EigenColVector<T> & v) { vector = v; }
        ColVector(const std::vector<T> & v) { 
            vector.resize(v.size());
            memcpy(vector.data(),v.data(),v.size()*sizeof(T));
        }
        T get(int i) { return vector(i); }
        void  set(int i, T v) { vector(i) = v; }
        
        void random(int i) { vector = EigenColVector<T>::Random(i); }
        void random() { vector.setRandom(vector.size()); }
        void zero() { vector.setZero(vector.size()); }    
        void ones() { vector.setOnes(vector.size()); }

        size_t  innerStride() { return vector.innerStride(); }
        T*  data() { return vector.data(); }
        void    resize(size_t size) { vector.resize(size); }
        void    resizeLike(const ColVector<T> & v) { vector.resizeLike(v.vector); }
        void    conservativeResize(size_t size) { vector.conservativeResize(size); }

        T coeff(size_t i) { return vector.coeff(i); }
        T& coeffRef(size_t i) { return vector.coeffRef(i); }

        void print()     {        std::cout << vector << std::endl;    }
        size_t cols() { return vector.cols(); }
        T dot(const ColVector<T> & b) { return vector.dot(b.vector);  }        
        size_t size() const { return vector.size(); }
        void fill(T v) { vector.fill(v); }
        T norm() { return vector.norm(); }
        T squaredNorm() { return vector.squaredNorm(); }

        ColVector<T>& eval() { vector.eval(); return *this;  }

        void normalize() {
            vector.normalize();
        }

        ColVector<T> normalized() { ColVector<T> r; r.vector = vector.normalized(); return r; }            
        ColVector<T>& head(ColVector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
        ColVector<T>& tail(ColVector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
        ColVector<T>& segment(ColVector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
        ColVector<T>& LinSpaced(T num, T a, T b)
        {        
            vector = EigenColVector<T>::LinSpaced(num,a,b);
            return *this;
        }

        T operator()(size_t i) { return vector(i); }
        T operator[](size_t i) { return vector(i); }

        /*
        T inner_product(const ColVector<T> & v)
        { return vector.adjoint() * v.vector; }
        */

        ColVector<T> operator + (const ColVector<T>& b) 
        { 
            ColVector<T> r(*this);
            r.vector = r.vector + b.vector;
            return r;
        }
        ColVector<T> operator - (const ColVector<T>& b) 
        { 
            ColVector<T> r(*this);
            r.vector = r.vector - b.vector;
            return r;
        }    
        
        T operator * (const ColVector<T>& b) 
        { 
            return vector.dot(b.vector);        
        }

        
        ColVector<T> operator * (const T b) 
        { 
            ColVector<T> r(*this);
            r.vector = r.vector * b;
            return r;
        }

        ColVector<T> operator / (const T b)  
        { 
            ColVector<T> r(*this);
            r.vector = r.vector / b;
            return r;
        }

        
        void aplus(const ColVector<T>& b) { vector += b.vector; }
        void aminus(const ColVector<T>& b) { vector -= b.vector; }
        void adot(const ColVector<T>& b) { vector.dot(b.vector); }
        
        void atimes(const T b) { vector = vector * b; }
        void adiv(const T b) { vector = vector / b; }


        ColVector<T> t() { return ColVector<T>(vector.transpose().eval());}    
        ColVector<T> abs() {  return ColVector<T>(vector.cwiseAbs());  }
        ColVector<T> inverse() { return ColVector<T>(vector.cwiseInverse()); }    
        ColVector<T> sqrt() { return ColVector<T>(vector.cwiseSqrt()); }
        ColVector<T> abs2() { return ColVector<T>(vector.cwiseAbs2()); }
        ColVector<T> operator * (ColMatrix<T> & m);

        ColVector<T> operator = (const ColVector<T> & v) { vector = v.vector; return *this; }
        ColVector<T> operator = (const EigenColVector<T> & v) { vector = v; return *this; }

        // add to each col
        ColMatrix<T> operator + (ColMatrix<T> & m);
        // subtract from each col 
        ColMatrix<T> operator - (ColMatrix<T> & m);
        // divide from each col
        ColMatrix<T> operator / (ColMatrix<T> & m);

        ColMatrix<T> matrix();    
    };

    
    template<typename T> ColVector<T>  abs(ColVector<T> & vector ) { ColVector<T>  r; r = vector.abs(); return r;}
    template<typename T> ColVector<T>  inverse(ColVector<T> & vector ) { ColVector<T>  r; r = vector.inverse(); return r; }
    template<typename T> ColVector<T>  exp(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().exp(); return r;  }
    template<typename T> ColVector<T>  log(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().log(); return r;  }
    template<typename T> ColVector<T>  log1p(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().log1p(); return r; }
    template<typename T> ColVector<T>  log10(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().log10(); return r; }
    template<typename T> ColVector<T>  pow(ColVector<T> & vector ,const T b) { ColVector<T>  r; r = vector.vector.array().pow(b); return r;}
    template<typename T> ColVector<T>  sqrt(ColVector<T> & vector ) { ColVector<T>  r; r = vector.sqrt(); return r;}
    template<typename T> ColVector<T>  rsqrt(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().rsqrt(); return r; }
    template<typename T> ColVector<T>  square(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().square(); return r;}
    template<typename T> ColVector<T>  cube(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().cube(); return r; }
    template<typename T> ColVector<T>  abs2(ColVector<T> & vector ) { ColVector<T>  r; r = vector.abs2(); return r; }
    template<typename T> ColVector<T>  sin(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().sin(); return r;}
    template<typename T> ColVector<T>  cos(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().cos(); return r; }
    template<typename T> ColVector<T>  tan(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().tan(); return r;}
    template<typename T> ColVector<T>  asin(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().asin(); return r; }
    template<typename T> ColVector<T>  acos(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().acos(); return r; }
    template<typename T> ColVector<T>  atan(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().atan(); return r; }
    template<typename T> ColVector<T>  sinh(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().sinh(); return r; }
    template<typename T> ColVector<T>  cosh(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().cosh(); return r; }
    template<typename T> ColVector<T>  tanh(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().tanh(); return r; }
    template<typename T> ColVector<T>  ceil(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().ceil(); return r; }
    template<typename T> ColVector<T>  floor(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().floor(); return r; }
    template<typename T> ColVector<T>  round(ColVector<T> & vector ) { ColVector<T>  r; r = vector.vector.array().round(); return r; }

    
    template<typename T>
    class ColMatrixView 
    {
    private:
        EigenColMatrix<T> *matrix;
        size_t row;
        size_t col;
        bool bUseRows;

    public:

        ColMatrixView(EigenColMatrix<T> *m, size_t r)
        {
            matrix = m;
            row = r;
            col = 0;
            bUseRows = true;
        }

        T __getitem(size_t i) { 
            if(bUseRows == true)
                return (*matrix)(row,i); 
            else 
                return (*matrix)(i,col); 
        }
        void  __setitem(size_t i, const T val) { 
            if(bUseRows == true)
                (*matrix)(row,i) = val; 
            else 
                (*matrix)(i,col) = val;
            
        }

        void use_rows() { bUseRows = true; }
        void use_cols() { bUseRows = false; }
        size_t rows() { return matrix->rows(); }
        size_t cols() { return matrix->cols(); }
        void   set_row(size_t r) { row = r; }
        void   set_col(size_t c) { col = c;}
        
    };

    template<typename T>
    struct ColMatrix
    {
        EigenColMatrix<T> matrix;

        ColMatrix() {}
        ColMatrix(int x, int y) { matrix = EigenColMatrix<T>(x,y); }
        ColMatrix(int x, int y, const T val) { matrix = EigenColMatrix<T>(x,y); matrix.fill(val);  }
        ColMatrix(const ColVector<T> & v) { matrix = v.vector; }        
        ColMatrix(const ColMatrix<T>& m) { matrix = m.matrix;  }    
        ColMatrix(const EigenColMatrix<T>& m) { matrix = m; }    
        ColMatrix(const EigenColVector<T> & v) { matrix = v; }    
        ColMatrix(const EigenArray2D<T> & a) { matrix = a.matrix(); }
        ColMatrix(const std::vector<T> & data, size_t r, size_t c) {       
            matrix.resize(r,c);
            memcpy(matrix.data(),data.data(),data.size()*sizeof(T));        
        }

        T& operator()(size_t i, size_t j) { return matrix(i,j); }
        
        void random(int x, int y) { matrix =  EigenColMatrix<T>::Random(x,y); }
        void random() { matrix.setRandom(matrix.rows(),matrix.cols()); }
        void identity(int x, int y) { matrix = EigenColMatrix<T>::Identity(x,y); }
        void identity() { matrix.setIdentity(matrix.rows(),matrix.cols()); }
        void zero(int x, int y) { matrix = EigenColMatrix<T>::Zero(x,y); }
        void zero() { matrix.setZero(matrix.rows(),matrix.cols()); }
        void ones(int x, int y) { matrix = EigenColMatrix<T>::Ones(x,y); }
        void ones() { matrix.setOnes(matrix.rows(),matrix.cols()); }

        T get(int i, int j) { return matrix(i,j); }
        void set(int i, int j, T v) { matrix(i,j) = v; }        
        T norm() { return matrix.norm(); }
        T squaredNorm() { return matrix.squaredNorm(); }

        bool all() { return matrix.all(); }
        bool allFinite() { return matrix.allFinite(); }
        bool any() { return matrix.any(); }
        bool count() { return matrix.count(); }

        size_t rows() const { return matrix.rows(); }
        size_t cols() const { return matrix.cols(); }
        void resize(int x, int y) { matrix.resize(x,y); }

        T* data() { return matrix.data(); }    
        
        ColVector<T> operator[](int index) { 
            if(index < 0) index += rows();
            EigenColMatrix<T> r = matrix.row(index);
            return ColVector<T>(r);        
        }
        void normalize()
        {
            matrix.normalize();
        }
        ColMatrix<T> normalized() { 
            ColMatrix<T> r; 
            r.matrix = matrix.normalized(); 
            return r; 
        }    

        void fill(T v) { matrix.fill(v); }
        ColMatrix<T> eval() { return ColMatrix<T>(matrix.eval()); }
        bool hasNaN() { return matrix.hasNaN(); }
        size_t innerSize() { return matrix.innerSize(); }
        size_t outerSize() { return matrix.outerSize(); }    
        bool isMuchSmallerThan(const ColMatrix<T>& n, T v) { return matrix.isMuchSmallerThan(n.matrix,v); }
        bool isOnes() { return matrix.isOnes(); }
        bool isZero() { return matrix.isZero(); }
                
        ColVector<T> col_vector(size_t row) { ColVector<T> r; r.vector = matrix.col(row); return r; }    
        ColVector<T> col(size_t row) { 
            ColVector<T> r;        
            r.vector = matrix.col(row); 
            return r; 
        }        
        void set_row(size_t row, std::vector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v[i];
        }
        void set_row_vector(size_t row, ColVector<T> & v)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v(i);
        }
        void set_row_matrix(size_t row, ColMatrix<T> & v, size_t src_row)    {        
            for(size_t i = 0; i < cols(); i++)
                matrix(row,i) = v.matrix(src_row,i);
        }

                
        void set_col(size_t col, std::vector<T> & v)    {
            assert(rows() == v.size());        
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v[i];
        }
        void set_col_vector(size_t col, ColVector<T> & v)    {
            assert(rows() == v.size());        
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v(i);
        }
        void set_col_matrix(size_t col, ColMatrix<T> & v, size_t row)    {
            assert(rows() == v.cols());
            assert(cols() == v.rows());
            for(size_t i = 0; i < rows(); i++)
                matrix(i,col) = v.matrix(row,i);

        }

        
        ColMatrix<T> leftCols(ColMatrix<T>& r, size_t cols) { r.matrix = matrix.leftCols(cols).eval(); return r; }
        ColMatrix<T> middleCols(ColMatrix<T>& r, size_t j, size_t cols) { r.matrix = matrix.middleCols(j,cols).eval(); return r; }
        ColMatrix<T> rightCols(ColMatrix<T>& r, size_t cols) { r.matrix = matrix.rightCols(cols).eval(); return r; }
        ColMatrix<T> topRows(ColMatrix<T>& r, size_t rows) { r.matrix = matrix.topRows(rows).eval(); return r; }
        ColMatrix<T> middleRows(ColMatrix<T>& r, size_t j, size_t rows) { r.matrix = matrix.middleRows(j,rows).eval(); return r; }
        ColMatrix<T> bottomRows(ColMatrix<T>& r, size_t rows) { r.matrix = matrix.bottomRows(rows).eval(); return r; }
        ColMatrix<T> topLeftCorner(ColMatrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topLeftCorner(i,j).eval(); return r; }
        ColMatrix<T> topRightCorner(ColMatrix<T>& r, size_t i, size_t j) { r.matrix = matrix.topRightCorner(i,j).eval(); return r; }

        
        ColMatrix<T> adjoint()  { ColMatrix<T> m; m.matrix = matrix.adjoint().eval(); return m; }
        ColMatrix<T> transpose() { ColMatrix<T> m; m.matrix = matrix.transpose().eval(); return m; }
        ColMatrix<T> diagonal() { ColMatrix<T> m; m.matrix = matrix.diagonal().eval(); return m; }        
        ColMatrix<T> reverse() { ColMatrix<T> m; m.matrix = matrix.reverse().eval(); return m; }    
        ColMatrix<T> replicate(size_t i, size_t j) { ColMatrix<T> m; m.matrix = matrix.replicate(i,j).eval(); return m; }
        
        void adjointInPlace() {  matrix.adjointInPlace(); }    
        void transposeInPlace() { matrix.transposeInPlace(); }
        void reverseInPlace() { matrix.reverseInPlace(); }

        
        ColMatrix<T> adjointInto(ColMatrix<T> & m) { m.matrix = matrix.adjoint().eval(); return m; }    
        ColMatrix<T> transposeInto(ColMatrix<T> & m) { m.matrix = matrix.transpose().eval();return m; }    
        ColMatrix<T> diagonalInto(ColMatrix<T> & m) { m.matrix = matrix.diagonal().eval();return m; }
        ColMatrix<T> reverseInto(ColMatrix<T> & m) { m.matrix = matrix.reverse().eval();return m; }
        ColMatrix<T> replicateInto(ColMatrix<T> & m,size_t i, size_t j) { m.matrix = matrix.replicate(i,j).eval(); return m; }
        
        /*
        ColMatrix<T> outer_product(ColMatrix<T> & m)
        {   return ColMatrix<T>(matrix * m.t().matrix); }
        */

        T sum()    {        
            return matrix.sum();        
        }
        T prod()    {        
            return matrix.prod();        
        }
        T mean()    {        
            return matrix.mean();        
        }
        T minCoeff()    {        
            return matrix.minCoeff();        
        }
        T maxCoeff()    {        
            return matrix.maxCoeff();        
        }    
        T trace()    {        
            return matrix.trace();        
        }

        
        
        // can't rowwise in Lua at this time.   
        ColMatrix<T> addToEachRow(ColMatrix<T> & v)    {
            ColMatrix<T> r(*this);        
            r.matrix = r.matrix.rowwise() + ColVector<T>(v.matrix).vector;
            return r;
        }
        
        ColMatrix<T> cwiseAbs()    {
            EigenColMatrix<T> r = matrix.cwiseAbs();
            return r;
        }
        ColMatrix<T> cwiseAbs2()    {
            EigenColMatrix<T> r = matrix.cwiseAbs2();
            return r;
        }
        ColMatrix<T> cwiseProduct(const ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseProduct(q.matrix); 
            return r;
        }
        ColMatrix<T> cwiseQuotient(const ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseQuotient(q.matrix); 
            return r;
        }
        ColMatrix<T> cwiseInverse()    {
            EigenColMatrix<T> r = matrix.cwiseInverse();
            return r;
        }
        ColMatrix<T> cwiseSqrt(ColMatrix<T>& out)    {
            EigenColMatrix<T> r = matrix.cwiseSqrt();
            return r;
        }
        ColMatrix<T> cwiseMax(ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseMin(q.matrix);
            return r;        
        }
        ColMatrix<T> cwiseMin(ColMatrix<T>& q)    {
            EigenColMatrix<T> r = matrix.cwiseMin(q.matrix);
            return r;
        }

        
        bool operator == (const ColMatrix<T> &m) {
            ColMatrix<T> r;
            auto x = (matrix == m.matrix);
            return x;
        }
        bool operator != (const ColMatrix<T> &m) {
            ColMatrix<T> r;
            auto x = (matrix != m.matrix);
            return x;
        }
        

        ColMatrix<T>& operator=(const ColMatrix<T> & m)    {        
            matrix = m.matrix.eval();                    
            return *this;
        }
        ColMatrix<T>& operator=(const EigenColMatrix<T> & m)    {        
            matrix = m;
            return *this;
        }
        ColMatrix<T> operator * (const ColMatrix<T> & m)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix * m.matrix;
            return r;
        } 
        ColMatrix<T> operator + (const ColMatrix<T> & m)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix + m.matrix;
            return r;
        }
        ColMatrix<T> operator - (const ColMatrix<T> & m)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix - m.matrix;
            return r;
        }

        
        ColMatrix<T> operator - ()     {
            ColMatrix<T> r(*this);
            r.matrix = -matrix; 
            return r;
        }
        ColVector<T> operator * (const ColVector<T> & v)    {
            ColVector<T> r;
            r.vector = matrix * v.vector;
            return r;
        }
        ColVector<T> operator + (const ColVector<T> & v)    {
            ColVector<T> r;
            r.vector = matrix + v.vector;
            return r;
        }
        ColVector<T> operator - (const ColVector<T> & v)    {
            ColVector<T> r;
            r.vector = matrix - v.vector;
            return r;
        }
        
        ColMatrix<T> operator * (const T f)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix * f;
            return r;
        }    
        ColMatrix<T> operator / (const T f)    {
            ColMatrix<T> r(*this);
            r.matrix = r.matrix / f;
            return r;
        }
            
        void print()     {
            std::cout << matrix << std::endl;
        }
        
        ColMatrix<T> t()     {
            return transpose();
        }
        

    // slice 

        ColMatrix<T> slice(int first_r,int first_c, int last_r=-1, int last_c=-1)    {
            if(last_r = -1) last_r = matrix.rows()-1;
            if(last_c = -1) last_c = matrix.cols()-1;
            auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seq(first_c,last_c));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        ColMatrix<T> sliceN1(int first_r,int first_rn, int first_c, int last_c=-1)    {        
            if(last_c = -1) last_c = matrix.cols()-1;
            auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seq(first_c,last_c));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        ColMatrix<T> sliceN2(int first_r,int first_c, int first_cn, int last_r=-1)    {                
            auto r = matrix(Eigen::seq(first_r,last_r),Eigen::seqN(first_c,first_cn));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        ColMatrix<T> slicen(int first_r,int first_rn, int first_c, int first_cn)    {        
            auto r = matrix(Eigen::seqN(first_r,first_rn),Eigen::seqN(first_c,first_cn));
            ColMatrix<T> ret(r.rows(),r.cols());
            for(size_t i = 0; i < r.rows(); i++)
                for(size_t j = 0; j < r.cols(); j++)
                    ret.matrix(i,j) = r(i,j);
            return ret;
        }

        void rowwise_add(ColVector<T> & r) {
            matrix.rowwise() += r.vector;
        }
        void rowwise_sub(ColVector<T> & r) {
            matrix.rowwise() -= r.vector;
        }
        void rowwise_mul(ColVector<T> & r) {
            matrix = (matrix.array().rowwise() * r.vector.array()).matrix();
        }
        void rowwise_div(ColVector<T> & r) {
            matrix = (matrix.array().rowwise() / r.vector.array()).matrix();
        }

        EigenArray2D<T> array() const { return matrix.array(); }
        
    };



    template<typename T> ColMatrix<T> abs(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().abs().matrix(); return r;}
    template<typename T> ColMatrix<T> inverse(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().inverse().matrix(); return r; }
    template<typename T> ColMatrix<T> exp(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().exp().matrix(); return r;  }
    template<typename T> ColMatrix<T> log(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().log().matrix(); return r;  }
    template<typename T> ColMatrix<T> log1p(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().log1p().matrix(); return r; }
    template<typename T> ColMatrix<T> log10(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().log10().matrix(); return r; }
    template<typename T> ColMatrix<T> pow(const ColMatrix<T> & matrix,const T b) { ColMatrix<T> r; r.matrix = matrix.array().pow(b).matrix(); return r;}
    template<typename T> ColMatrix<T> sqrt(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().sqrt().matrix(); return r;}
    template<typename T> ColMatrix<T> rsqrt(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().rsqrt().matrix(); return r; }
    template<typename T> ColMatrix<T> square(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().square().matrix(); return r;}
    template<typename T> ColMatrix<T> cube(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().cube().matrix(); return r; }
    template<typename T> ColMatrix<T> abs2(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().abs2().matrix(); return r; }
    template<typename T> ColMatrix<T> sin(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().sin().matrix(); return r;}
    template<typename T> ColMatrix<T> cos(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().cos().matrix(); return r; }
    template<typename T> ColMatrix<T> tan(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().tan().matrix(); return r;}
    template<typename T> ColMatrix<T> asin(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().asin().matrix(); return r; }
    template<typename T> ColMatrix<T> acos(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().acos().matrix(); return r; }
    template<typename T> ColMatrix<T> atan(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().atan().matrix(); return r; }
    template<typename T> ColMatrix<T> sinh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().sinh().matrix(); return r; }
    template<typename T> ColMatrix<T> cosh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().cosh().matrix(); return r; }
    template<typename T> ColMatrix<T> tanh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().tanh().matrix(); return r; }
    template<typename T> ColMatrix<T> ceil(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().ceil().matrix(); return r; }
    template<typename T> ColMatrix<T> floor(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().floor().matrix(); return r; }
    template<typename T> ColMatrix<T> round(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().round().matrix(); return r; }
    template<typename T> ColMatrix<T> asinh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().asinh().matrix(); return r; }
    template<typename T> ColMatrix<T> acosh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().acosh().matrix(); return r; }
    template<typename T> ColMatrix<T> atanh(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().atanh().matrix(); return r; }
    template<typename T> ColMatrix<T> rint(const ColMatrix<T> & matrix) { ColMatrix<T> r; r.matrix = matrix.array().rint().matrix(); return r; }

    template<typename T> 
    ColVector<T> ColVector<T>::operator * (ColMatrix<T> &b) 
    { 
        ColVector<T> r;        
        r.vector =  b.matrix * vector;
        return r;
    }
    
    template<typename T> 
    ColMatrix<T> ColVector<T>::operator + (ColMatrix<T> &b) 
    {     
        ColMatrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) + vector(j);
        return r;
    }
    
    template<typename T> 
    ColMatrix<T> ColVector<T>::operator - (ColMatrix<T> &b) 
    {     
        ColMatrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) - vector(j);
        return r;
    }
    
    template<typename T> 
    ColMatrix<T> ColVector<T>::operator / (ColMatrix<T> &b) 
    {     
        ColMatrix<T> r;        
        for(size_t i = 0; i < b.rows(); i++)
            for(size_t j = 0; j < b.cols(); j++)
                r.matrix(i,j) = b.matrix(i,j) / vector(j);
        return r;
    }
    
    template<typename T> ColMatrix<T> ColVector<T>::matrix() { return ColMatrix<T>(vector); }

    template<typename T>
    struct SampleVector
    {        
        EigenArray<T> vector;
        size_t channels;

        SampleVector() {
            channels = 1;
        }
        SampleVector(size_t size, size_t channels = 1) {
            vector.resize(size * channels);
            this->channels = channels;
        }
        SampleVector(const std::vector<T> & s, size_t chans = 1) {
            vector.resize(s.size());
            memcpy(vector.data(),s.data(),s.size()*sizeof(T));
            channels = chans;
        }
        SampleVector(const T * ptr, size_t n, size_t chans = 1) {
            vector.resize(n*chans);
            memcpy(vector.data(),ptr,n*chans*sizeof(T));
            channels = chans;
        }
        SampleVector(const SampleVector<T> & s) {
            vector = s.vector;
            channels = s.channels;
        }    
        SampleVector(const EigenArray<T> & v, size_t ch = 1) {
            vector = v;
            channels = ch;
        }
        SampleVector(size_t channels, const EigenArray<T>& a) {
            vector = a;
            this->channels = channels;
        }

        SampleVector<T> clamp(T min = (T)0.0, T max = (T)1.0) {
            SampleVector<T> r(vector,channels);
            r.vector = vector.cwiseMin(min).cwiseMax(max);
            return r;
        }
        
        T&   get_stride(size_t ch, size_t pos) { return vector[pos*channels + ch]; }
        void set_stride(size_t ch, size_t pos, const T val) { vector[pos*channels + ch] = val; } 
        
        void swap_stereo_channels() {
            assert(channels == 2);
            for(size_t i = 0; i < size(); i+= channels) {
                T temp = vector[i];
                vector[i] = vector[i+1];
                vector[i+1] = temp;
            }
        }

        void set_data(size_t n, size_t channels, T * samples)
        {
            vector.resize(n);
            this->channels = channels;
            memcpy(vector.data(),samples,n*sizeof(T));
        }
        void copy_data(T * samples) {
            memcpy(samples,vector.data(),vector.size()*sizeof(T));
        }
        void set_channel(size_t ch, T * samples) {
            size_t x = 0;
            for(size_t i = ch; i < size(); i+=channels) vector[i] = samples[x++];
        }

        size_t size() const { return vector.size(); }
        size_t num_channels() const { return channels; } 
        void zero() { vector.setZero(); }
        void ones() { vector.setOnes(); }
        void random() { vector.setRandom(); }
        void fill(T s) { vector.fill(s); }
        size_t samples_per_channel() const { return size() / num_channels(); }
        T sum() { return vector.sum(); }
        T min() { return vector.minCoeff(); }
        T max() { return vector.maxCoeff(); }
        size_t min_index() { size_t i; vector.minCoeff(&i); return i; }
        size_t max_index() { size_t i; vector.maxCoeff(&i); return i; }
        T* data() { return vector.data(); }
        
        void normalize() { vector.matrix().normalize(); }
        SampleVector<T> normalized() { return SampleVector<T>(vector.matrix().normalized().array()); }

        SampleVector<T>& operator = (const SampleVector<T> & v) {
            vector = v.vector;
            channels  = v.channels;
            return *this;
        }    
        T& operator()(size_t i, size_t ch) {
            return vector[i*channels + ch];
        }
        T operator()(size_t i, size_t ch) const {
            return vector[i*channels + ch];
        }
        T& operator[](size_t i) {
            return vector[i];
        }
        T operator[](size_t i) const {
            return vector[i];
        }
         
        SampleVector<T> get_channel(size_t channel) {            
            SampleVector<T> r(samples_per_channel());
            size_t x=0;
            for(size_t i = channel; i < size(); i+=channels)
                r[x++] = vector[i];
            return r;
        }
        void get_channel(size_t channel, T * r) {                        
            size_t x=0;
            for(size_t i = channel; i < size(); i+=channels)
                r[x++] = vector[i];            
        }
        SampleVector<T> get_channel(size_t channel) const {            
            SampleVector<T> r(samples_per_channel());
            size_t x=0;
            for(size_t i = channel; i < size(); i+=channels)
                r[x++] = vector[i];
            return r;
        }
        void set_channel(const SampleVector<T> & v, size_t ch) {            
            size_t x = 0;
            for(size_t i = ch; i < size(); i+=channels) vector[i] = v[x++];
        }
        void set_channel(const T* v, size_t ch) {            
            size_t x = 0;
            for(size_t i = ch; i < size(); i+=channels) vector[i] = v[x++];
        }
        void make_stereo(const T * in) {
            set_channel(in,0);
            set_channel(in,1);
        }
        void make_stereo(const T * left, const T * right) {
            set_channel(left,0);
            set_channel(right,1);
        }

        void set_channels(size_t c) {
            channels = c;
            vector.resize(vector.size()*c);
        }
        void resize(size_t n) {
            vector.resize(n);
        }
        void resize(size_t s, size_t c) {
            channels = c;
            vector.resize(s * c);
        }

        SampleVector<T> get_channel_count(size_t ch, size_t pos, size_t n) {
            SampleVector<T> r(n);
            memcpy(r.data(), get_channel(ch).data()+pos, n*sizeof(T));
            return r;
        }

        bool operator == (const SampleVector<T> & s) {
            return s.channels == channels && vector.size() == s.vector.size();
        }

        void copy(const SampleVector<T> & in, size_t block) {
            vector.resize(in.size());
            memcpy(vector.data(),in.vector.data(),block*sizeof(T));
        }
        void copy(const SampleVector<T> & in, size_t pos, size_t n) {
            vector.resize((pos+n));
            memcpy(vector.data(),in.vector.data()+pos,n*sizeof(T));
        }
        void copy_from(const T* in, size_t block) {            
            memcpy(vector.data(),in,block*sizeof(T));
        }
        void copy_to(T* in, size_t block) {            
            memcpy(in,vector.data(),block*sizeof(T));
        }

        SampleVector<T> slice(size_t i, size_t n) {
            SampleVector<T> r(n);
            memcpy(r.data(), vector.data()+i, n * sizeof(T));
            return r;
        }
        
        SampleVector<T> pan(SampleType pan) {
            SampleType pan_mapped = ((pan+1.0)/2.0) * (M_PI/2.0);
            SampleVector<T> r(size(),2);
            for(size_t i = 0; i < size(); i+=channels)
            {
                r[i] = vector[i] * std::sin(pan_mapped);
                if(num_channels() == 2)
                    r[i+1] = vector[i+1] * std::cos(pan_mapped);
                else
                    r[i+1] = vector[i] * std::cos(pan_mapped);
            }
            return r;        
        }
        SampleVector<T> stride_slice(const SampleVector<T> & in, size_t stride, size_t pos, size_t n)
        {
            SampleVector<T> r(n/stride);
            size_t x = pos;
            for(size_t i = 0; i < n; i++) {
                r[i] = vector[x];
                x += stride;
            } 
            return r;
        }
        SampleVector<T> operator + (const SampleVector<T> &s) {
            assert(*this == s);
            SampleVector<T> r(*this);
            r.vector += s.vector;
            return r;
        }
        SampleVector<T> operator - (const SampleVector<T> &s) {
            assert(*this == s);
            SampleVector<T> r(*this);
            r.vector -= s.vector;
            return r;
        }
        SampleVector<T> operator * (const SampleVector<T> &s) {
            assert(*this == s);
            SampleVector<T> r(*this);
            r.vector *= s.vector;
            return r;
        }
        SampleVector<T> operator / (const SampleVector<T> &s) {
            assert(*this == s);
            SampleVector<T> r(*this);
            r.vector /= s.vector;
            return r;
        }
        
        SampleVector<T> operator + (const T s) {    
            SampleVector<T> r(*this);
            r.vector += s;
            return r;
        }
        SampleVector<T> operator - (const T s) {        
            SampleVector<T> r(*this);
            r.vector -= s;
            return r;
        }
        SampleVector<T> operator / (const T s) {        
            SampleVector<T> r(*this);
            r.vector /= s;
            return r;
        }
        SampleVector<T> operator * (const T s) {        
            SampleVector<T> r(*this);
            r.vector *= s;
            return r;
        }
        

        SampleVector<T>& operator += (const SampleVector<T>& s) {        
            assert(*this == s);
            vector += s.vector;
            return *this;
        }
        SampleVector<T>& operator -= (const SampleVector<T>& s) {        
            assert(*this == s);
            vector -= s.vector;
            return *this;
        }
        SampleVector<T>& operator *= (const SampleVector<T>& s) {        
            assert(*this == s);
            vector *= s.vector;
            return *this;
        }
        SampleVector<T>& operator /= (const SampleVector<T>& s) {        
            assert(*this == s);
            vector /= s.vector;
            return *this;
        }
        
        SampleVector<T> operator += (const T s) {
            vector += s;
            return *this;
        }
        SampleVector<T> operator -= (const T s) {
            vector -= s;
            return *this;
        }
        SampleVector<T> operator /= (const T s) {
            vector /= s;
            return *this;
        }
        SampleVector<T> operator *= (const T s) {
            vector *= s;
            return *this;
        }
        
        
        SampleVector<T> abs() { 
            SampleVector<T> r(*this);
            r.vector = vector.abs();
            return r;
        }
        SampleVector<T> abs2() { 
            SampleVector<T> r(*this);
            r.vector = vector.abs2();
            return r;
        }
        SampleVector<T> inverse() { 
            SampleVector<T> r(*this);
            r.vector = vector.inverse();
            return r;
        }
        SampleVector<T> exp() { 
            SampleVector<T> r(*this);
            r.vector = vector.exp();
            return r;
        }
        SampleVector<T> log() { 
            SampleVector<T> r(*this);
            r.vector = vector.log();
            return r;
        }
        SampleVector<T> log1p() { 
            SampleVector<T> r(*this);
            r.vector = vector.log1p();
            return r;
        }
        SampleVector<T> log10() { 
            SampleVector<T> r(*this);
            r.vector = vector.log10();
            return r;
        }
        SampleVector<T> pow(const SampleVector<T> & s) { 
            SampleVector<T> r(*this);
            r.vector = vector.pow(s.vector);
            return r;
        }
        SampleVector<T> pow(const T s) { 
            SampleVector<T> r(*this);
            r.vector = vector.pow(s);        
            return r;        
        }
        SampleVector<T> sqrt() {
            SampleVector<T> r(*this);
            r.vector = vector.sqrt();
            return r;
        }
        SampleVector<T> rsqrt() {
            SampleVector<T> r(*this);
            r.vector = vector.rsqrt();
            return r;
        }
        SampleVector<T> square() {
            SampleVector<T> r(*this);
            r.vector = vector.square();
            return r;
        }
        SampleVector<T> sin() {
            SampleVector<T> r(*this);
            r.vector = vector.sin();
            return r;
        }
        SampleVector<T> cos() {
            SampleVector<T> r(*this);
            r.vector = vector.cos();
            return r;
        }
        SampleVector<T> tan() {
            SampleVector<T> r(*this);
            r.vector = vector.tan();
            return r;
        }
        SampleVector<T> asin() {
            SampleVector<T> r(*this);
            r.vector = vector.asin();
            return r;
        }
        SampleVector<T> acos() {
            SampleVector<T> r(*this);
            r.vector = vector.acos();
            return r;
        }
        SampleVector<T> atan() {
            SampleVector<T> r(*this);
            r.vector = vector.atan();
            return r;
        }        
        SampleVector<T> sinh() {
            SampleVector<T> r(*this);
            r.vector = vector.sinh();
            return r;
        }
        SampleVector<T> cosh() {
            SampleVector<T> r(*this);
            r.vector = vector.cosh();
            return r;
        }
        SampleVector<T> tanh() {
            SampleVector<T> r(*this);
            r.vector = vector.tanh();
            return r;
        }
        SampleVector<T> ceil() { 
            SampleVector<T> r(*this);
            r.vector = vector.ceil();
            return r;        
        }
        SampleVector<T> floor() { 
            SampleVector<T> r(*this);
            r.vector = vector.floor();
            return r;
        }
        SampleVector<T> round() { 
            SampleVector<T> r(*this);
            r.vector = vector.round();
            return r;
        }

        void println() {
            std::cout << "Size=" << size() << std::endl;
            std::cout << "Channel=" << num_channels() << std::endl;
            std::cout << "Samples per channel=" << samples_per_channel() << std::endl;
            for(size_t i = 0; i < size(); i++)
                std:: cout << vector[i] << ",";
            std::cout << std::endl;
        }

    };

    template<typename T>
    struct SampleMatrix {
        EigenArray2D<T> channels;

        SampleMatrix() = default;
        
        SampleMatrix(size_t chan, size_t samps) {
            channels.resize(chan,samps);
        }
        SampleMatrix(const SampleVector<T> & v) {
            channels.resize(v.num_channels(), v.size()/v.num_channels());
            for(size_t i = 0; i < v.num_channels(); i++)             
                channels.row(i) = v.get_channel(i).vector;
        }
        SampleMatrix(const EigenArray2D<T> & c) {
            channels = c;
        }
        SampleMatrix(const SampleMatrix<T> & m) {
            channels = m.channels;
        }

        T * data() { return channels.data(); }
        
        void deinterleave(const SampleVector<T> & s) {
            
            channels.resize(s.num_channels(),s.samples_per_channel());            
            
            for(size_t i = 0; i < s.num_channels(); i++) {
                channels.row(i) = s.get_channel(i).vector;
            }
            
        }    

        SampleVector<T> interleave() { 
            SampleVector<T> r;
            r.resize(channels.cols() * channels.cols());
            for(size_t i = 0; i < channels.rows(); i++)            
            {
                SampleVector<T> tmp(1,channels(i));            
                r.set_channel(tmp,i);
            }
            return r;
        }

        size_t num_channels() const { return channels.rows(); }
        size_t cols() const { return channels.cols(); }
        void resize(size_t r, size_t c) { channels.resize(r,c); }

        SampleVector<T> operator[](size_t ch) { 
            SampleVector<T> tmp(1,channels.row(ch));
            return tmp;
        }
        SampleVector<T> get_channel(size_t c) { 
            SampleVector<T> tmp(1,channels.row(c));
            return tmp;
        }
        void set_channel(size_t c, const SampleVector<T> & s) 
        { 
            channels.row(c) = s.vector;
        }

        SampleMatrix<T>& operator = (const SampleMatrix<T> & v) {        
            channels  = v.channels;
            return *this;
        }
        T& operator()(size_t c, size_t n) {
            return channels(c,n);
        }
        bool operator == (const SampleMatrix<T> & b) {
            return channels.rows() == b.channels.rows() && channels.cols() == b.channels.cols();
        }
        SampleMatrix<T> operator + (const SampleMatrix<T> & m) {        
            SampleMatrix<T> r(*this);
            r.channels = channels + m.channels;
            return r;
        }
        SampleMatrix<T> operator - (const SampleMatrix<T> & m) {
            assert(*this == m);
            SampleMatrix<T> r(*this);
            r.channels = channels - m.channels;
            return r;
        }
        SampleMatrix<T> operator * (const SampleMatrix<T> & m) {
            assert(*this == m);
            SampleMatrix<T> r(*this);
            r.channels = channels * m.channels;
            return r;
        }
        SampleMatrix<T> operator / (const SampleMatrix<T> & m) {
            assert(*this == m);
            SampleMatrix<T> r(*this);
            r.channels = channels / m.channels;
            return r;
        }
        
        SampleMatrix<T> operator + (const T s) {            
            SampleMatrix<T> r(*this);        
            r.channels = channels + s;        
            return r;
        }
        SampleMatrix<T> operator - (const T s) {            
            SampleMatrix<T> r(*this);        
            r.channels = channels - s;
            return r;
        }
        SampleMatrix<T> operator * (const T s) {            
            SampleMatrix<T> r(*this);        
            r.channels = channels * s;
            return r;
        }
        SampleMatrix<T> operator / (const T s) {            
            SampleMatrix<T> r(*this);        
            r.channels = channels / s;
            return r;
        }
        

        SampleMatrix<T> abs() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.abs();
            return r;
        }
        SampleMatrix<T> abs2() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.abs2();
            return r;
        }
        SampleMatrix<T> inverse() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.inverse();
            return r;
        }
        SampleMatrix<T> exp() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.exp();
            return r;
        }
        SampleMatrix<T> log() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.log();
            return r;
        }
        SampleMatrix<T> log1p() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.log1p();
            return r;
        }
        SampleMatrix<T> log10() { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.log10();
            return r;
        }
        SampleMatrix<T> pow(const T s) { 
            SampleMatrix<T> r(*this);        
            r.channels = channels.pow(s);
            return r;
        }
        SampleMatrix<T> sqrt() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.sqrt();
            return r;
        }
        SampleMatrix<T> rsqrt() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.rsqrt();
            return r;
        }
        SampleMatrix<T> square() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.square();
            return r;
        }
        SampleMatrix<T> sin() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.sin();
            return r;
        }
        SampleMatrix<T> cos() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.cos();
            return r;
        }
        SampleMatrix<T> tan() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.tan();
            return r;
        }
        SampleMatrix<T> asin() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.asin();
            return r;
        }
        SampleMatrix<T> acos() {
            SampleMatrix<T> r(*this);
            r.channels = channels.acos();
            return r;
        }
        SampleMatrix<T> atan() {
            SampleMatrix<T> r(*this);
            r.channels = channels.atan();
            return r;
        }
        SampleMatrix<T> sinh() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.sinh();
            return r;
        }
        SampleMatrix<T> cosh() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.cosh();
            return r;
        }
        SampleMatrix<T> tanh() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.tanh();
            return r;
        }
        SampleMatrix<T> ceil() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.ceil();
            return r;
        }
        SampleMatrix<T> floor() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.floor();
            return r;
        }
        SampleMatrix<T> round() {
            SampleMatrix<T> r(*this);        
            r.channels = channels.round();
            return r;
        }
    };

    template<typename T>  SampleVector<T> abs( SampleVector<T> & m) { return m.abs(); }
    template<typename T>  SampleVector<T> abs2( SampleVector<T> & m) { return m.abs2(); }
    template<typename T>  SampleVector<T> inverse( SampleVector<T> & m) { return m.inverse(); }
    template<typename T>  SampleVector<T> exp( SampleVector<T> & m) { return m.exp(); }
    template<typename T>  SampleVector<T> log( SampleVector<T> & m) { return m.log(); }
    template<typename T>  SampleVector<T> log1p( SampleVector<T> & m) { return m.log1p(); }
    template<typename T>  SampleVector<T> log10( SampleVector<T> & m) { return m.log10(); }
    template<typename T>  SampleVector<T> pow( SampleVector<T> & m,  SampleVector<T> & p) { return m.pow(p); }
    template<typename T>  SampleVector<T> pow( SampleVector<T> & m,  T p) { return m.pow(p); }
    template<typename T>  SampleVector<T> sqrt( SampleVector<T> & m) { return m.sqrt(); }
    template<typename T>  SampleVector<T> rsqrt( SampleVector<T> & m) { return m.rsqrt(); }
    template<typename T>  SampleVector<T> square( SampleVector<T> & m) { return m.square(); }
    template<typename T>  SampleVector<T> sin( SampleVector<T> & m) { return m.sin(); }
    template<typename T>  SampleVector<T> cos( SampleVector<T> & m) { return m.cos(); }
    template<typename T>  SampleVector<T> tan( SampleVector<T> & m) { return m.tan(); }
    template<typename T>  SampleVector<T> asin( SampleVector<T> & m) { return m.asin(); }
    template<typename T>  SampleVector<T> acos( SampleVector<T> & m) { return m.acos(); }
    template<typename T>  SampleVector<T> atan( SampleVector<T> & m) { return m.atan(); }
    template<typename T>  SampleVector<T> sinh( SampleVector<T> & m) { return m.sinh(); }
    template<typename T>  SampleVector<T> cosh( SampleVector<T> & m) { return m.cosh(); }
    template<typename T>  SampleVector<T> tanh( SampleVector<T> & m) { return m.tanh(); }
    template<typename T>  SampleVector<T> ceil( SampleVector<T> & m) { return m.ceil(); }
    template<typename T>  SampleVector<T> floor( SampleVector<T> & m) { return m.floor(); }
    template<typename T>  SampleVector<T> round( SampleVector<T> & m) { return m.round(); }

    template<typename T>  SampleMatrix<T> abs( SampleMatrix<T> & m) { return m.abs(); } 
    template<typename T>  SampleMatrix<T> abs2( SampleMatrix<T> & m) { return m.abs2(); }
    template<typename T>  SampleMatrix<T> inverse( SampleMatrix<T> & m) { return m.inverse(); }
    template<typename T>  SampleMatrix<T> exp( SampleMatrix<T> & m) { return m.exp(); }
    template<typename T>  SampleMatrix<T> log( SampleMatrix<T> & m) { return m.log(); }
    template<typename T>  SampleMatrix<T> log1p( SampleMatrix<T> & m) { return m.log1p(); }
    template<typename T>  SampleMatrix<T> log10( SampleMatrix<T> & m) { return m.log10(); }
    template<typename T>  SampleMatrix<T> pow( SampleMatrix<T> & m,  T &p) { return m.pow(p); }
    template<typename T>  SampleMatrix<T> sqrt( SampleMatrix<T> & m) { return m.sqrt(); }
    template<typename T>  SampleMatrix<T> rsqrt( SampleMatrix<T> & m) { return m.rsqrt(); }
    template<typename T>  SampleMatrix<T> square( SampleMatrix<T> & m) { return m.square(); }
    template<typename T>  SampleMatrix<T> sin( SampleMatrix<T> & m) { return m.sin(); }
    template<typename T>  SampleMatrix<T> cos( SampleMatrix<T> & m) { return m.cos(); }
    template<typename T>  SampleMatrix<T> tan( SampleMatrix<T> & m) { return m.tan(); }
    template<typename T>  SampleMatrix<T> asin( SampleMatrix<T> & m) { return m.asin(); }
    template<typename T>  SampleMatrix<T> acos( SampleMatrix<T> & m) { return m.acos(); }
    template<typename T>  SampleMatrix<T> atan( SampleMatrix<T> & m) { return m.atan(); }
    template<typename T>  SampleMatrix<T> sinh( SampleMatrix<T> & m) { return m.sinh(); }
    template<typename T>  SampleMatrix<T> cosh( SampleMatrix<T> & m) { return m.cosh(); }
    template<typename T>  SampleMatrix<T> tanh( SampleMatrix<T> & m) { return m.tanh(); }
    template<typename T>  SampleMatrix<T> ceil( SampleMatrix<T> & m) { return m.ceil(); }
    template<typename T>  SampleMatrix<T> floor( SampleMatrix<T> & m) { return m.floor(); }
    template<typename T>  SampleMatrix<T> round( SampleMatrix<T> & m) { return m.round(); }
    
    template<typename T> 
    SampleMatrix<T> deinterleave(const SampleVector<T> & v) {
        SampleMatrix<T> m;
        m.deinterleave(v);
        return m;
    }

    template<typename T> 
    SampleVector<T> interleave(SampleMatrix<T> & m) {
        return m.interleave();
    }

    
    template<typename T>
    struct Window 
    {   
        SampleVector<T> window;

        Window(size_t i) { window.vector.vector.resize(i); }
        virtual ~Window() = default;

        T& operator[](size_t i) { return window[i]; }

        SampleVector<T> operator * (const SampleVector<T> & v) { return window * v; }
    };

    template<typename T>
    struct Rectangle: public Window<T>
    {
        Rectangle(size_t i) : Window<T>(i) { this->window.fill(1); } 
    };

    template<typename T>
    struct Hamming: public Window<T>
    {
        Hamming(size_t i) : Window<T>(i) {
            T n = this->window.vector.size()-1;
            for(size_t i = 0; i < this->window.size(); i++)
            {
                this->window.vector.vector(i) = 0.54 - (0.46 * std::cos(2*M_PI*i/n));
            }        
        }
    };

    template<typename T>
    struct Hanning: public Window<T>
    {
        Hanning(size_t i) : Window<T>(i) {
            T n = this->window.vector.size()-1;
            for(size_t i = 0; i < this->window.size(); i++)
            {
                this->window.vector.vector(i) = 0.5*(1 - std::cos(2*M_PI*i/n));
            }        
        }
    };

    template<typename T>
    struct Blackman: public Window<T>
    {
        Blackman(size_t i) : Window<T>(i)    
        {
            T n = this->window.vector.size()-1;
            for(size_t i = 0; i < this->window.size(); i++)                    
                this->window.vector.vector(i) = 0.42 - (0.5* std::cos(2*M_PI*i/(n)) + (0.08*std::cos(4*M_PI*i/n)));        
        }
    };

    template<typename T>
    struct BlackmanHarris: public Window<T>
    {
        BlackmanHarris(size_t i) : Window<T>(i)    
        {
            T n = this->window.vector.size()-1;
            for(size_t i = 0; i < this->window.size(); i++)            
                this->window.vector.vector(i) = 0.35875 
                        - 0.48829*std::cos(2*M_PI*(i/n))
                        + 0.14128*std::cos(4.0*M_PI*(i/n)) 
                        - 0.01168*std::cos(6.0*M_PI*(i/n));
        }
    };

    template<typename T>
    struct Gaussian: public Window<T>
    {
        Gaussian(size_t i) : Window<T>(i)
        {
            T a,b,c=0.5;
            for(size_t n = 0; n < this->window.size(); n++)
            {
                a = (n - c*(this->window.vector.size()-1)/(std::sqrt(c)*this->window.vector.size()-1));
                b = -c * std::sqrt(a);
                this->window.vector.vector(n) = std::exp(b);
            }
        }
    };
    template<typename T>
    struct Welch: public Window<T>
    {
        Welch(size_t i) : Window<T>(i)
        {
            for(size_t i = 0; i < this->window.vector.size(); i++)
                this->window.vector.vector(i) = 1.0 - std::sqrt((2*i-this->window.vector.size())/(this->window.vector.size()+1));        
        }
    };
    template<typename T>
    struct Parzen: public Window<T>
    {

        Parzen(size_t i) : Window<T>(i)
        {
            for(size_t i = 0; i < this->window.vector.size(); i++)
                this->window.vector.vector(i) = 1.0 - std::abs((2.0f*i-this->window.vector.size())/(this->window.vector.size()+1.0f));        
        }    
    };
    template<typename T>
    struct Tukey: public Window<T>
    {
        Tukey(size_t i, T alpha) : Window<T>(i)
        {
            size_t num_samples = this->window.vector.size();
            T value = (-1*(num_samples/2)) + 1;
            for(size_t i = 0; i < num_samples; i++)
            {    
                if(value >= 0 && value <= (alpha * (num_samples/2))) 
                    this->window.vector.vector[i] = 1.0; 
                else if(value <= 0 && (value >= (-1*alpha*(num_samples/2)))) 
                    this->vector.vector[i] = 1.0;
                else 
                    this->vector.vector[i] = 0.5 * (1 + std::cos(M_PI *(((2*value)/(alpha*num_samples))-1)))        ;
                value = value + 1;
            }     
        }
    };

    template<typename T>
    void swap(SampleVector<T> & vector,size_t i, size_t j)
    {
        T x = vector.vector(i);
        vector.vector(i) = vector.vector(j);
        vector.vector(j) = x;
    }        

    template<typename T>
    void shift(SampleVector<T> & vector) 
    {
        size_t half = vector.vector.size() / 2; 
        size_t start= half; 
        if(2*half < vector.vector.size()) start++;
        for(size_t i = 0; i < half; i++)
        {
            swap(vector,i, i+start);
            
        }
        if(start != half)
        {
            for(size_t i = 0; i < half; i++)            
            {
                swap(vector,i+start-1,i+start);
            }
        }
    }

    template<typename T>    
    void ishift(SampleVector<T> & vector) 
    {
        size_t half = vector.vector.size() / 2; 
        size_t start= half; 
        if(2*half < vector.vector.size()) start++;
        for(size_t i = 0; i < half; i++)
        {
            swap(vector,i,i+start);
        }
        if(start != half)
        {
            for(size_t i = 0; i < half; i++)            
            {
                swap(vector,half,i);
            }
        }
    }

    template<typename T>    
    T quadratic_peak_pos(SampleVector<T> & vector,size_t pos)
    {
        T s0,s1,s2;
        size_t x0,x2;        
        if(pos == 0 || pos == vector.size()-1) return pos; 
        x0 = (pos < 1)? pos:pos-1;
        x2 = (pos+1 < vector.size()) ? pos+1:pos;
        if(x0 == pos) return vector.vector(pos) <= vector.vector(x2)? pos:x2;
        if(x2 == pos) return vector.vector(pos) <= vector.vector(x0)? pos:x0;
        s0 = vector.vector(x0);
        s1 = vector.vector(pos);
        s2 = vector.vector(x2);
        return pos + 0.5 *(s0-s2) / (s0 - 2*s1 + s2);
    }

    template<typename T>    
    T quadratic_peak_mag(SampleVector<T> & vector,size_t pos)
    {
        T x0,x1,x2;
        size_t index = (size_t)(pos - 0.5) + 1;
        if(pos >= vector.size() || pos < (T)0) return (T)0;
        if((T)index == pos) return vector.vector(index);
        x0 = vector.vector(index-1);
        x1 = vector.vector(index);
        x2 = vector.vector(index+1);
        return x1 - 0.25 * (x0 - x2) * (pos - index);
    }

    template<typename T>    
    T median(SampleVector<T> & vector)
    {        
        size_t n = vector.vector.size();
        size_t high,low;
        size_t median;
        size_t middle,ll,hh;

        low = 0;
        high = n-1;
        median = (low+high)/2;

        for(;;)        
        {
            if(high <= low)
                return vector.vector(median);

            if(high == low+1)
            {
                if(vector.vector(low) > vector.vector(high))
                    swap(vector,low,high);
                return vector.vector(median);
            }
            middle = (low+high)/2;
            if(vector.vector(middle) > vector.vector(high)) swap(vector,middle,high);
            if(vector.vector(low) > vector.vector(high)) swap(vector,low,high);
            if(vector.vector(middle) > vector.vector(low)) swap(vector,middle,low);
            swap(vector,middle,low+1);

            ll=low+1;
            hh=high;
            for(;;)
            {
                do ll++; while( vector.vector(low) > vector.vector(ll));
                do hh--; while( vector.vector(hh) > vector.vector(low));
                if(hh < ll) break;
                swap(vector,ll,hh);
            }
            swap(vector,low,hh);
            if(hh <= median) low = ll;
            if(hh >= median) high = hh-1;
        }
    }

    template<typename T>    
    T moving_threshold(SampleVector<T> & input, size_t post, size_t pre, size_t pos)
    {
        size_t length = input.size();
        size_t win_length = post+pre+1;
        SampleVector<T> vector = input;
        if(pos < post+1)
        {
            for(size_t k = 0; k < post +1 - pos; k++)
                vector.vector(k) = 0;
            for(size_t k = post+1-pos; k < win_length; k++)
                vector.vector(k) = vector.vector(k+pos-post);            
        }
        else if(pos + pre < length)
        {
            for(size_t k = 0; k < win_length; k++)
                vector.vector(k) = vector.vector(k+pos-post);
        }
        else 
        {
            for(size_t k = 0; k < length - pos + post; k++)
                vector.vector(k) = vector.vector(k+pos-post);
            for(size_t k = length - pos + post; k < win_length; k++)
                vector.vector(k) = 0;
        }        
        return median(vector);
    }

    template<typename T> 
    T zero_crossing_rate(SampleVector<T> & vector)
    {
        T zcr = 0;
        for(size_t i = 1; i < vector.vector.size(); i++)
        {
            bool current = vector.vector(i) > 0;
            bool prev    = vector.vector(i-1) > 0;
            if(current != prev) zcr++;
        }    
        return zcr;   
    }
    
    template<typename T> 
    void autocorr(SampleVector<T> & vector,SampleVector<T> & output)
    {
        T tmp;
        output.resize(vector.size());
        for(size_t i = 0; i < vector.size(); i++)
        {
            tmp = (T)0;
            for(size_t j = 0; j < vector.size(); j++)
                tmp += vector.vector(j-i) * vector.vector(j);                
            
            output.vector.vector(i) = tmp / (T)(vector.size()-1);
        }
    }

    
    template<typename T>
    void push(SampleVector<T> & vector,const T& new_elem)
    {
        for(size_t i = 0; i < vector.vector.size()-1; i++) vector.vector(i) = vector.vector(i+1);
        vector.vector(vector.vector.size()-1) = new_elem;
    }

    template<typename T>
    void clamp(SampleVector<T> & vector,T absmax) { 
        for(size_t i = 0; i < vector.size(); i++)
        {
            if( vector.vector(i) > absmax) vector.vector(i) = absmax;
            else if(vector.vector(i) < -absmax) vector.vector(i) = -absmax;
        }
    }

    template<typename T> 
    void  normalize(SampleVector<T> & vector) { 
            Eigen::Matrix<T,1,Eigen::Dynamic> r = vector.vector.matrix();
            r.normalize();
            vector.vector = r.array();         
    }


    template<typename T> 
    bool peakpick(SampleVector<T> & vector,size_t pos)
    {
        bool tmp = false;
        tmp = (vector.vector(pos) > vector.vector(pos-1)) && (vector.vector(pos) > vector.vector(pos+1)) && vector.vector(pos) > 0;
        return tmp;
    }
    
    template<typename T> 
    T RMS(SampleVector<T> & vector)
    {
        T sum = vector.vector.pow(2).sum();        
        return std::sqrt(sum/vector.vector.size());
    }

    template<typename T> 
    T peak_energy(SampleVector<T> & vector)
    {
        T peak = -10000;
        T as;         
        for(size_t i = 0; i < vector.vector.size(); i++)
        {
            T as = std::fabs(vector.vector(i));
            if(as > peak) peak = as;
        }
        return peak;
    }

    template<typename T> 
    T min(SampleVector<T> & vector) { 
        T min = 1e120;
        for(size_t i = 0; i < vector.size(); i++)
            if(vector.vector(i) < min) min = vector.vector(i);
        return min;            
        //return vector.vector(vector.vector.minCoeff()); 
    }

    template<typename T> 
    T max(SampleVector<T> & vector) { 
        T max = -1e120;
        for(size_t i = 0; i < vector.size(); i++)
            if(vector.vector(i) > max) max = vector.vector(i);
        return max;
        
    }

    template<typename T> 
    SampleVector<T>& weighted_copy(SampleVector<T> & vector,const SampleVector<T> & weight, SampleVector<T> & out)
    {
        out.vector.vector = vector.vector * weight.vector.vector;
        return out;
    }    

    template<typename T> 
    T level_lin(SampleVector<T> & vector) 
    {
        T energy = vector.vector.sqrt().sum();
        return energy/vector.size();
    }

    template<typename T> 
    T local_hfc(SampleVector<T> & vector)
    {
        T hfc = 0;
        for(size_t j = 0; j < vector.size(); j++)
            hfc += (j+1) * vector.vector(j);
        return hfc;
    }

    template<typename T> 
    void min_removal(SampleVector<T> & vector)
    {
        T m = min(vector);
        vector.vector -= m;
    }

    template<typename T> 
    T alpha_norm(SampleVector<T> & vector,SampleVector<T> & out, T alpha)
    {
        T tmp = vector.vector.abs().pow(alpha).sum();
        return std::pow(tmp/vector.size(),1.0/alpha);
    }

    template<typename T> 
    void alpha_normalize(SampleVector<T> & vector,SampleVector<T> & out, T alpha)
    {
        T tmp = alpha_norm(out,alpha);
        out.vector.vector = out.vector.vector / tmp;         
    }    

    template<typename T> 
    T unwrap2pi(T phase)
    {
        return phase + 2*M_PI*(1.0 + std::floor(-(phase+M_PI)/(2*M_PI)));
    }

    template<typename T> 
    T quadfrac(T s0, T s1, T s2, T pf)
    {
        T tmp = s0 + (pf/2)*(pf*(s0-2.0*s1+s2)-3.0*s0+4*s1-s2);
        return tmp;
    }

    template<typename T> 
    T freqtomidi(T freq)
    {
        T midi;
        if(freq < 2.0 || freq > 100000) return 0;        
        midi = 12*std::log(midi/6.875) / 0.6931471805599453 -3;
        return midi;
    }

    template<typename T> 
    T miditofreq(T midi)
    {
        if(midi > 140) return 0;
        T freq = std::exp(((midi + 3) / 12)*0.6931471805599453)*6.875;
        return freq;
    }

    template<typename T> 
    T bintofreq(T bin, T sample_rate, T fft_size)
    {
        T freq = sample_rate / fft_size;
        return freq * std::max(bin,(T)0);
    }


    template<typename T> 
    T bintomidi(T bin, T sample_rate, T fft_size)
    {
        T midi = bintofreq(bin,sample_rate,fft_size);
        return freqtomidi(midi);
    }

    template<typename T> 
    T freqtobin(T freq, T sample_rate, T fft_size)
    {
        T bin = fft_size / sample_rate;
        return std::max(freq,(T)0)*bin;
    }

    template<typename T> 
    T miditobin(T midi, T sample_rate, T fft_size)
    {
        T freq = miditofeq(midi);
        return freqtobin(freq,sample_rate, fft_size);
    }
    template<typename T> 
    bool is_power_of_two(uint64_t a)
    {
        if((a & (a-1))==0)
            return true;
        return false;
    }
    template<typename T> 
    uint64_t next_power_of_two(uint64_t a)
    {
        uint64_t i = 1;
        while(i < a) i <<= 1;
        return i;
    }

    template<typename T> 
    T hztomel(T freq)
    {
        T lin_space = (T)3/(T)200;
        T split_hz = (T)1000;
        T split_mel = split_hz * lin_space;
        T log_space = (T)27 / std::log(6.4);
        assert(freq >= (T)0);
        if(freq < split_hz)        
            return freq * lin_space;        
        else
            return split_mel + log_space * std::log(freq/split_hz);
    }
    template<typename T> 
    T hztomel_htk(T freq)
    {        
        T split_hz = (T)700;        
        T log_space = (T)1127;
        assert(freq >= (T)0);
        return log_space * std::log(1 + freq/split_hz);
    }
        
    template<typename T> 
    T meltohz(T mel)
    {
        T lin_space = (T)200/(T)3;
        T split_hz  = (T)1000;
        T split_mel = split_hz / lin_space;
        T log_spacing = std::pow(6.4,1.0/27.0);
        assert(mel >= 0);
        if( mel < split_mel) return lin_space * mel;
        return split_hz * std::pow(log_spacing, mel-split_mel);
    }

    template<typename T> 
    T meltohz_htk(T mel)
    {
        T split_hz = (T)700;
        T log_space = (T)1/(T)1127;
        assert(mel >= 0);
        return split_hz * (std::exp(mel *log_space) -(T)1);
    }

    template<typename T> 
    T db_spl(SampleVector<T> & vector) { return 10 * std::log10(level_lin(vector)); }

    template<typename T> 
    T level_detect(SampleVector<T> & vector,T threshold) { 
        T db = db_spl(vector);
        if(db < threshold) return 1;
        return db;
    }

    template<typename T> 
    size_t size(SampleVector<T> & v) { return v.size(); }
    
    template<typename T> 
    SampleVector<T> random(size_t n, size_t channels=1) { 
        SampleVector<T> r(n,channels);
        r.random();
        return r;
    }

    template<typename T> 
    void fill(SampleVector<T> & v, T value) { v.fill(value); }

    template<typename T> 
    void resize(SampleVector<T> & v, size_t n, size_t channels=1) { v.resize(n,channels); }

    template<typename T> 
    bool    silence_detect(SampleVector<T> & vector,T threshold) { return db_spl(vector) < threshold; }

    template<typename T>
    void set_left_channel(const std::vector<T> & left, SoundWave::SampleVector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = left[x++];
    }
    template<typename T>
    void set_left_channel(const SoundWave::SampleVector<T> & left, SoundWave::SampleVector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = left[x++];
    }
    template<typename T>
    void set_left_channel(const SoundWave::SampleVector<T> & left, std::vector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = left[x++];
    }
    template<typename T>
    void set_right_channel(const std::vector<T> & right, SoundWave::SampleVector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = right[x++];
    }
    template<typename T>
    void set_right_channel(const SoundWave::SampleVector<T> & right, SoundWave::SampleVector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = right[x++];
    }
    template<typename T>
    void set_right_channel(const SoundWave::SampleVector<T> & right, std::vector<T> & out) {
        size_t x = 0;
        for(size_t i = 0; i < out.size(); i+=2) out[i] = right[x++];
    }
    template<typename T>
    void set_channel(size_t ch, const std::vector<T> & in, SoundWave::SampleVector<T> & out) {
        size_t x = 0;
        for(size_t i = ch; i < out.size(); i+=2) out[i] = in[x++];
    }
    template<typename T>
    SoundWave::SampleVector<T> interleave(size_t n, size_t channels, const std::vector<SoundWave::SampleVector<T>> & in) {
        std::vector<T> r(n*channels);
        for(size_t i = 0; i < channels; i++)
            for(size_t j = 0; j < n; j++)
                r[j*channels + i] = in[i][j];
        return r;
    }
    template<typename T>
    std::vector<SoundWave::SampleVector<T>> deinterleave(size_t n, size_t channels, const SoundWave::SampleVector<T> & in) {
        std::vector<SoundWave::SampleVector<T>> r(n);
        for(size_t i = 0; i < channels; i++)
        {
            r[i].resize(n);
            for(size_t j = 0; j < n; j++)
                r[i][j] = in[j*channels + i];
        }
        return r;
    }
};