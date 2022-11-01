#pragma once 
#include <kfr/kfr.h>
#include <kfr/dsp.hpp>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>

#include <iostream>
#include <random> 


typedef double SampleType;

namespace DSP {
    template<typename T> struct UniVector;
    template<typename T> using UniComplex = UniVector<kfr::complex<T>>;

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

        UniVector<T> operator + (const UniVector<T> & s);
        UniVector<T> operator * (const UniVector<T> & s);

        

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
        
        
        void println() { std::cout << val << std::endl; }  
    };

    template<typename T>
    Scalar<T> abs(const Scalar<T> & scalar)  { 
        return Scalar<T>(std::abs(scalar.val));
    }    
    template<typename T>     
    Scalar<T> fabs(const Scalar<T> & scalar) { 
        return Scalar<T>(std::fabs(scalar.val));         
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
    Scalar<T> fmin(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmax(scalar.val,y.val); }
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
    Scalar<T> operator + (const Scalar<T> & a, const Scalar<T> & b) {
        return a + b;
    }
    template<typename T>
    Scalar<T> operator * (const Scalar<T> & a, const Scalar<T> & b) {
        return a * b;
    }
    template<typename T>
    Scalar<T> operator - (const Scalar<T> & a, const Scalar<T> & b) {
        return a - b;
    }
    template<typename T>
    Scalar<T> operator / (const Scalar<T> & a, const Scalar<T> & b) {
        return a / b;
    }
    
    
    // single channel vector (SampleVector)
    template<typename T> 
    struct UniVector
    {
        kfr::univector<T> vec; 

        UniVector() {}
        UniVector(size_t s) { vec.resize(s); }
        UniVector(T* data, size_t size) { vec = kfr::make_univector(data,size); }
        UniVector(std::vector<T> & data) { vec = kfr::make_univector(data.data(),data.size()); }
        UniVector(const kfr::univector<T> & u) : vec(u) {} 
        UniVector(const UniVector<T>& v) : vec(v.vec) {}
        //UniVector(const std::initializer_list<T> & vals) { vec = kfr::make_univector(vals.begin(),vals.end()); }

        size_t size() const { return vec.size(); }
        void resize(size_t s) { vec.resize(s); }
        T& operator[](size_t i) { return vec[i]; }
        T  operator[](size_t i) const { return vec[i]; }
        T  __getitem(size_t i) { return vec[i]; }
        void __setitem(size_t i, const T & val) { vec[i] = val; }

        void fill(T value) { std::fill(vec.begin(),vec.end(),value); }
        
        UniVector<T> scalar(size_t s, const T & val)
        {        UniVector<T> r(s); r.fill(val); return r;    }

        T& at(size_t pos) { return vec.at(pos); }
        T& front() { return vec.front(); }
        T& back() { return vec.back(); }
        T* data() { return vec.data(); }
            
        UniVector<T>& operator = (const UniVector<T>& u) { vec = u.vec; return *this; }
        UniVector<T>  operator + (const UniVector<T>& u) { UniVector<T> r; r.vec=(vec+u.vec); return r; }
        UniVector<T>  operator - (const UniVector<T>& u) { UniVector<T> r; r.vec=(vec-u.vec); return r; }
        UniVector<T>  operator * (const UniVector<T>& u) { UniVector<T> r; r.vec=(vec*u.vec); return r; }
        UniVector<T>  operator / (const UniVector<T>& u) { UniVector<T> r; r.vec=(vec/u.vec); return r; }
        //UniVector<T>  operator % (const UniVector<T>& u) { UniVector<T> r; r.vec=(vec%u.vec); return r; }

        UniVector<T>  operator + (const T& u) { UniVector<T> r; r.vec=(vec+u); return r; }
        UniVector<T>  operator - (const T& u) { UniVector<T> r; r.vec=(vec-u); return r; }
        UniVector<T>  operator * (const T& u) { UniVector<T> r; r.vec=(vec*u); return r; }
        UniVector<T>  operator / (const T& u) { UniVector<T> r; r.vec=(vec/u); return r; }
        //UniVector<T>  operator % (const T& u) { UniVector<T> r; r.vec=(vec%u); return r; }

        UniVector<T>  operator + (const Scalar<T>& u) { UniVector<T> r; r.vec=(vec+u.val); return r; }
        UniVector<T>  operator - (const Scalar<T>& u) { UniVector<T> r; r.vec=(vec-u.val); return r; }
        UniVector<T>  operator * (const Scalar<T>& u) { UniVector<T> r; r.vec=(vec*u.val); return r; }
        UniVector<T>  operator / (const Scalar<T>& u) { UniVector<T> r; r.vec=(vec/u.val); return r; }
        //UniVector<T>  operator % (const Scalar<T>& u) { UniVector<T> r; r.vec=(vec%u.val); return r; }

        UniVector<T>&  operator += (const UniVector<T>& u) { vec+=u.vec; return *this; }
        UniVector<T>&  operator -= (const UniVector<T>& u) { vec-=u.vec; return *this; }
        UniVector<T>&  operator *= (const UniVector<T>& u) { vec*=u.vec; return *this; }
        UniVector<T>&  operator /= (const UniVector<T>& u) { vec/=u.vec; return *this; }
        //UniVector<T>&  operator %= (const UniVector<T>& u) { vec%=u.vec; }

        UniVector<T>&  operator += (const T& u) { vec+=u; return *this; }
        UniVector<T>&  operator -= (const T& u) { vec-=u; return *this; }
        UniVector<T>&  operator *= (const T& u) { vec*=u; return *this; }
        UniVector<T>&  operator /= (const T& u) { vec/=u; return *this; }
        //UniVector<T>&  operator %= (const T& u) { vec%=u; }
        
        void println() {
            kfr::println(vec);
        }
    };

    template<typename T>
    UniVector<T> operator + (const UniVector<T> & a, const UniVector<T> & b) {
        return a + b;
    }
    template<typename T>
    UniVector<T> operator - (const UniVector<T> & a, const UniVector<T> & b) {
        return a - b;
    }
    template<typename T>
    UniVector<T> operator * (const UniVector<T> & a, const UniVector<T> & b) {
        return a * b;
    }
    template<typename T>
    UniVector<T> operator / (const UniVector<T> & a, const UniVector<T> & b) {
        return a / b;
    }
    /*
    template<typename T>
    UniVector<T> operator % (const UniVector<T> & a, const UniVector<T> & b) {
        return a % b;
    }
    */
    template<typename T>
    UniVector<T> operator + (const UniVector<T> & a, const Scalar<T> & b) {
        return a + b;
    }
    template<typename T>
    UniVector<T> operator - (const UniVector<T> & a, const Scalar<T> & b) {
        return a - b;
    }
    template<typename T>
    UniVector<T> operator * (const UniVector<T> & a, const Scalar<T> & b) {
        return a * b;
    }
    template<typename T>
    UniVector<T> operator / (const UniVector<T> & a, const Scalar<T> & b) {
        return a / b;
    }
    /*
    template<typename T>
    UniVector<T> operator % (const UniVector<T> & a, const Scalar<T> & b) {
        return a % b;
    }
    */


    template<typename T>
    UniVector<T> Scalar<T>::operator + (const UniVector<T> & s)
    {
        return UniVector<T>(s.vec + val);
    }
    template<typename T>
    UniVector<T> Scalar<T>::operator * (const UniVector<T> & s)
    {
        return UniVector<T>(s.vec * val);
    }

    
    
    template<typename T>
    T energy_to_loudness(T energy) {
        return kfr::energy_to_loudness(energy);
    }
    template<typename T>
    T loudness_to_energy(T loudness) {
        return kfr::loudness_to_energy(loudness);
    }

    template<typename T>
    using SampleVector = UniVector<T>;

    template<typename T> 
    struct UniVector2D
    {
        kfr::univector2d<T> vec; 

        UniVector2D() {}
        UniVector2D(size_t samples, size_t channels=2) { vec.resize(channels); for(size_t i = 0; i < channels; i++) vec[i].resize(samples); }    
        UniVector2D(const kfr::univector2d<T> & u) : vec(u) {} 
        UniVector2D(const UniVector2D<T>& v) : vec(v.vec) {}
        //UniVector(const std::initializer_list<T> & vals) { vec = kfr::make_univector(vals.begin(),vals.end()); }

        size_t size(size_t channel=0) const { return vec[channel].size(); }
        void resize(size_t c, size_t s) { 
            vec.resize(c);
            for(size_t i = 0; i < c; i++) vec[i].resize(s);
         }

        int num_channels() const { return vec.size(); }
        void fill(size_t channel, T value) { std::fill(vec[channel].begin(),vec[channel].end(),value); }
        void fill(T value) {
            for(size_t i = 0; i < num_channels(); i++)
                fill(i,value);
        }
        UniVector<T> operator[](size_t ch) { return UniVector<T>(vec[ch]); }

        UniVector<T> get_channel(size_t ch) { return UniVector<T>(vec[ch]); }
        void         set_channel(size_t ch, const UniVector<T> & v) { vec[ch] = v.vec; }
        UniVector<T>  __getitem(size_t ch)  { return UniVector<T>(vec[ch]); }
        void __setitem(size_t ch, const UniVector<T> & val) { vec[ch] = val.vec; }


        UniVector2D<T>& operator = (const UniVector2D<T>& u) { vec = u.vec; return *this; }

        /*
        UniVector2D<T>  operator + (const UniVector2D<T>& u) { 
            UniVector2D<T> r; 
            for(size_t i = 0; i < vec.size(); i++)
                r.vec[i] =(vec[i]+u.vec[i]); 
                return r; 
        }
        UniVector2D<T>  operator - (const UniVector2D<T>& u) { 
            UniVector2D<T> r; 
            for(size_t i = 0; i < vec.size(); i++)
                r.vec[i] =(vec[i]-u.vec[i]); 
                return r; 
        }        
        UniVector2D<T>  operator * (const UniVector2D<T>& u) { 
            UniVector2D<T> r; 
            for(size_t i = 0; i < vec.size(); i++)
                r.vec[i] =(vec[i]*u.vec[i]); 
                return r; 
        }
            
        UniVector2D<T>  operator / (const UniVector2D<T>& u) { 
            UniVector2D<T> r; 
            for(size_t i = 0; i < vec.size(); i++)
                r.vec[i] =(vec[i]/u.vec[i]); 
                return r; 
        }

        UniVector2D<T>  operator % (const UniVector2D<T>& u) { 
            UniVector2D<T> r; 
            for(size_t i = 0; i < vec.size(); i++)
                r.vec[i] =(vec[i] % u.vec[i]); 
                return r; 
        }
        
        UniVector2D<T>&  operator += (const UniVector2D<T>& u) { vec = vec + u.vec; return *this; }
        UniVector2D<T>&  operator -= (const UniVector2D<T>& u) { vec = vec - u.vec; return *this; }
        UniVector2D<T>&  operator *= (const UniVector2D<T>& u) { vec = vec * u.vec; return *this; }
        UniVector2D<T>&  operator /= (const UniVector2D<T>& u) { vec = vec / u.vec; return *this; }
        UniVector2D<T>&  operator %= (const UniVector2D<T>& u) { vec = vec % u.vec; return *this; }        
        */
    };
    
    template<typename T>
    UniVector2D<T> stereo(const UniVector<T> & left, const UniVector<T> & right) {
        UniVector2D<T> vec = left.vec + right.vec;
        return vec;
    }

    using window_symmetry = kfr::window_symmetry;

    
    template<typename T>
    UniVector<T> make_univec(const T * data, size_t s) {
        return UniVector<T>(kfr::make_univector<T>(data,s)); 
    }

    template<typename T>
    UniVector<T> make_window_hann(size_t s) {
        return UniVector<T>(kfr::window_hann<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_hamming(size_t s) {
        return UniVector<T>(kfr::window_hamming<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_blackman(size_t s, const T alpha=T(0.16), window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return UniVector<T>(kfr::window_blackman<T>(s,alpha,symmetry));
    }
    template<typename T>
    UniVector<T> make_window_blackman_harris(size_t s, window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return UniVector<T>(kfr::window_blackman_harris<T>(s,symmetry));
    }
    template<typename T>
    UniVector<T> make_window_gaussian(size_t s, const T alpha=T(0.25)) {
        return UniVector<T>(kfr::window_gaussian<T>(s,alpha));
    }
    template<typename T>
    UniVector<T> make_window_triangular(size_t s) {
        return UniVector<T>(kfr::window_triangular<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_bartlett(size_t s) {
        return UniVector<T>(kfr::window_bartlett<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_cosine(size_t s) {
        return UniVector<T>(kfr::window_cosine<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_bartlett_hann(size_t s) {
        return UniVector<T>(kfr::window_bartlett_hann<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_bohman(size_t s) {
        return UniVector<T>(kfr::window_bohman<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_lanczos(size_t s) {
        return UniVector<T>(kfr::window_lanczos<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_flattop(size_t s) {
        return UniVector<T>(kfr::window_flattop<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_rectangular(size_t s) {
        return UniVector<T>(kfr::window_rectangular<T>(s));
    }
    template<typename T>
    UniVector<T> make_window_kaiser(size_t s, const T beta = T(0.5)) {
        return UniVector<T>(kfr::window_kaiser<T>(s,beta));
    }

    template<typename T>
    kfr::expression_pointer<T> make_window_hann_ptr(size_t s) {
        return to_pointer(kfr::window_hann<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_hamming_ptr(size_t s) {
        return to_pointer(kfr::window_hamming<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_blackman_ptr(size_t s, const T alpha=T(0.16), window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return to_pointer(kfr::window_blackman<T>(s,alpha,symmetry));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_blackman_harris_ptr(size_t s, window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return to_pointer(kfr::window_blackman_harris<T>(s,symmetry));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_gaussian_ptr(size_t s, const T alpha=T(0.25)) {
        return to_pointer(kfr::window_gaussian<T>(s,alpha));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_triangular_ptr(size_t s) {
        return to_pointer(kfr::window_triangular<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_bartlett_ptr(size_t s) {
        return to_pointer(kfr::window_bartlett<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_cosine_ptr(size_t s) {
        return to_pointer(kfr::window_cosine<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_bartlett_hann_ptr(size_t s) {
        return to_pointer(kfr::window_bartlett_hann<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_bohman_ptr(size_t s) {
        return to_pointer(kfr::window_bohman<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_lanczos_ptr(size_t s) {
        return to_pointer(kfr::window_lanczos<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_flattop_ptr(size_t s) {
        return to_pointer(kfr::window_flattop<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_rectangular_ptr(size_t s) {
        return to_pointer(kfr::window_rectangular<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_kaiser_ptr(size_t s, const T beta = T(0.5)) {
        return to_pointer(kfr::window_kaiser<T>(s,beta));
    }

    template<typename X>
    X rol(X x, X y) { return kfr::rol(x,y); }
    template<typename X>
    X ror(X x, X y) { return kfr::rol(x,y); }
    template<typename X>
    X shl(X x, X y) { return kfr::rol(x,y); }
    template<typename X>
    X shr(X x, X y) { return kfr::rol(x,y); }

    template<typename T>
    UniVector<T> bitwiseand(const UniVector<T> & a, const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::bitwiseand(a.vec,b.vec); return r; }
    template<typename T>
    UniVector<T> bitwiseandnot(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::bitwiseandnot(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> bitwisenot(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::bitwisenot(a.vec); return r; }
    template<typename T>    
    UniVector<T> bitwiseor(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::bitwiseor(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> bitwisexor(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::bitwisexor(a.vec,b.vec); return r; }

    template<typename T>    
    UniVector<T> linspace(T start, T stop, size_t size, bool endpoint=false,bool trunc=false)
    {   
        UniVector<T> r; 
        r.vec = kfr::linspace(start,stop,size,endpoint,trunc); 
        return r; 
    }

    template<typename T>    
    UniVector<T> pad(const UniVector<T> & in, const T & fill_value = T(0))
    { 
        UniVector<T> r; 
        r.vec = kfr::padded(in.vec,fill_value); 
        return r; 
    }

    template<typename T>    
    UniVector<T> slice(const UniVector<T> & v, size_t start, size_t end=kfr::max_size_t)
    {   
        UniVector<T> r;        
        r.vec = v.vec.slice(start,end);
        return r;
    }

    template<typename T>    
    UniVector<T> truncate(const UniVector<T> & v, size_t size)
    {   
        UniVector<T> r; 
        r.vec = v.vec.truncate();
        return r;
    }

    template<typename T>    
    UniVector<T> reverse(const UniVector<T> & v)
    {   
        UniVector<T> r;         
        r.vec = kfr::reverse(v.vec);
        return r;
    }


    template<typename T>    
    T& ringbuf_read(UniVector<T> &v,size_t & cursor, T& value) { v.vec.ringbuf_read(cursor,value); return value; }

    template<typename T>    
    void ringbuf_write(UniVector<T> &v, size_t & cursor, T& value) { v.vec.ringbuf_write(cursor,value); }


    template<typename T>    
    UniVector<T> abs(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::abs(a.vec); return r; }
    template<typename T>    
    UniVector<T> add(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::add(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> add(const UniVector<T> & a,const T & b) { UniVector<T> r; r.vec = kfr::add(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> absmax(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::absmax(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> absmax(const UniVector<T> & a,const T & b) { UniVector<T> r; r.vec = kfr::absmax(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> absmin(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::absmin(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> absmin(const UniVector<T> & a,const T & b) { UniVector<T> r; r.vec = kfr::absmin(a.vec,b); return r; }    
    template<typename T>    
    UniVector<T> clamp(const UniVector<T> & a,const UniVector<T> & lo, const UniVector<T> &hi) { UniVector<T> r; r.vec = kfr::clamp(a.vec,lo.vec,hi.vec); return r; }
    template<typename T>    
    UniVector<T> clamp(const UniVector<T> & a,const T& lo, const T &hi) { UniVector<T> r; r.vec = kfr::clamp(a.vec,lo,hi); return r; }
    template<typename T>    
    UniVector<T> cube(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::cub(a.vec); return r; }
    template<typename T>    
    UniVector<T> div(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::div(a.vec,b.vec); return r; }    
    template<typename T>    
    UniVector<T> fmadd(const UniVector<T> & a,const UniVector<T> & y, const UniVector<T> & z) { UniVector<T> r; r.vec = kfr::fmadd(a.vec,y.vec,z.vec); return r; }
    template<typename T>    
    UniVector<T> fmsub(const UniVector<T> & a,const UniVector<T> & y, const UniVector<T> & z) { UniVector<T> r; r.vec = kfr::fmsub(a.vec,y.vec,z.vec); return r; }    
    template<typename T>    
    UniVector<T> max(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::max(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> max(const UniVector<T> & a, const T & b) { UniVector<T> r; r.vec = kfr::max(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> min(const UniVector<T> & a, const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::min(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> min(const UniVector<T> & a, const T & b) { UniVector<T> r; r.vec = kfr::min(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> mix(const UniVector<T> & a, const T& c, const UniVector<T> & y) { UniVector<T> r; r.vec = kfr::mix(c,a.vec,y.vec); return r; }
    template<typename T>    
    UniVector<T> mixs(const UniVector<T> & a, const T& c, const UniVector<T> & y) { UniVector<T> r; r.vec = kfr::mixs(c,a.vec,y.vec); return r; }
    template<typename T>    
    UniVector<T> mul(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::mul(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> mul(const UniVector<T> & a, const T & b) { UniVector<T> r; r.vec = kfr::mul(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> neg(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::neg(a.vec); return r; }        
    template<typename T>    
    UniVector<T> sqr(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::sqr(a.vec); return r; }
    template<typename T>    
    UniVector<T> sqrt(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::sqrt(a.vec); return r; }
    template<typename T>    
    UniVector<T> exp(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::exp(a.vec); return r; }
    template<typename T>    
    UniVector<T> exp10(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::exp10(a.vec); return r; }
    template<typename T>    
    UniVector<T> exp2(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::exp2(a.vec); return r; }
    template<typename T>    
    UniVector<T> exp_fmadd(const UniVector<T> & a,const UniVector<T> & y, const UniVector<T> & z) { UniVector<T> r; r.vec = kfr::exp_fmadd(a.vec,y.vec,z.vec); return r; }
    template<typename T>    
    UniVector<T> log(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::log(a.vec); return r; }
    template<typename T>    
    UniVector<T> log10(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::log10(a.vec); return r; }
    template<typename T>    
    UniVector<T> log2(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::log2(a.vec); return r; }
    template<typename T>    
    UniVector<T> log_fmadd(const UniVector<T> & a,const UniVector<T> & y, const UniVector<T> & z) { UniVector<T> r; r.vec = kfr::log_fmadd(a.vec,y.vec,z.vec); return r; }
    template<typename T>    
    UniVector<T> logb(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::logb(a.vec); return r; }
    template<typename T>    
    UniVector<T> logm(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::logm(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> logn(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::logn(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> pow(const UniVector<T> & a,const T & y) { UniVector<T> r; r.vec = kfr::pow(a.vec,y); return r; }
    template<typename T>    
    UniVector<T> pow(const UniVector<T> & a,const UniVector<T> & y) { UniVector<T> r; r.vec = kfr::pow(a.vec,y.vec); return r; }
    template<typename T>    
    UniVector<T> root(const UniVector<T> & a,const UniVector<T> & y) { UniVector<T> r; r.vec = kfr::root(a.vec,y.vec); return r; }
    template<typename T>    
    UniVector<T> floor(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::floor(a.vec); return r; }        
    template<typename T>    
    UniVector<T> acos(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::acos(a.vec); return r; }
    template<typename T>    
    UniVector<T> asin(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::asin(a.vec); return r; }
    template<typename T>    
    UniVector<T> atan(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::atan(a.vec); return r; }
    template<typename T>    
    UniVector<T> atan2(const UniVector<T> & a,const T & b) { UniVector<T> r; r.vec = kfr::atan2(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> atan2(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::atan2(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> atan2deg(const UniVector<T> & a,const T & b) { UniVector<T> r; r.vec = kfr::atan2deg(a.vec,b); return r; }
    template<typename T>    
    UniVector<T> atan2deg(const UniVector<T> & a,const UniVector<T> & b) { UniVector<T> r; r.vec = kfr::atan2deg(a.vec,b.vec); return r; }
    template<typename T>    
    UniVector<T> atandeg(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::atandeg(a.vec); return r; }
    template<typename T>    
    UniVector<T> cos(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::cos(a.vec); return r; }
    template<typename T>    
    UniVector<T> sin(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::sin(a.vec); return r; }    
    template<typename T>    
    UniVector<T> cosdeg(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::cosdeg(a.vec); return r; }        
    template<typename T>    
    UniVector<T> sindeg(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::cosdeg(a.vec); return r; }    
    template<typename T>    
    UniVector<T> sinc(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::sinc(a.vec); return r; }
    template<typename T>    
    UniVector<T> tan(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::tan(a.vec); return r; }        
    template<typename T>    
    UniVector<T> cosh(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::cosh(a.vec); return r; }
    template<typename T>    
    UniVector<T> coth(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::coth(a.vec); return r; }    
    template<typename T>    
    UniVector<T> sinh(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::sinh(a.vec); return r; }    
    template<typename T>    
    UniVector<T> tanh(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::tanh(a.vec); return r; }
    template<typename T>    
    UniVector<T> gamma(const UniVector<T> & a) { UniVector<T> r; r.vec = kfr::gamma(a.vec); return r; }

    template<typename T>    
    T absmaxof(const UniVector<T> & a) { return kfr::absmaxof(a.vec); }
    template<typename T>    
    T absminof(const UniVector<T> & a) { return kfr::absminof(a.vec); }
    template<typename T>    
    T dot(const UniVector<T> & a,const UniVector<T> & b) { return kfr::dotproduct(a.vec,b.vec); }
    template<typename T>    
    T maxof(const UniVector<T> & a) { return kfr::maxof(a.vec); }
    template<typename T>    
    T minof(const UniVector<T> & a) { return kfr::minof(a.vec); }
    template<typename T>    
    T mean(const UniVector<T> & a) { return kfr::mean(a.vec); }
    template<typename T>    
    T product(const UniVector<T> & a) { return kfr::product(a.vec); }
    template<typename T>    
    T rms(const UniVector<T> & a) { return kfr::rms(a.vec); }
    template<typename T>    
    T sum(const UniVector<T> & a) { return kfr::sum(a.vec); }
    template<typename T>    
    T sumsqr(const UniVector<T> & a) { return kfr::sumsqr(a.vec); }

    // doesn't compile
    //template<typename T>    
    //UniVector<T> div(const UniVector<T> & a,const T b) { UniVector<T> r; r.vec = kfr::div<T>(a.vec,b); return r; }

    template<typename T>    
    UniVector<T> ipow(const UniVector<T> & v, int base) { UniVector<T> r; r.vec = kfr::ipow(v.vec,base); return r; }

    template<typename T>    
    T kcos2x(const T s, const T c) {return kfr::cos2x<SampleType>(s,c); }

    template<typename T>    
    T kcos3x(const T & s, const T & c) {return kfr::cos3x(s,c); }

    template<typename T>    
    T ksin2x(const T & s, const T & c) {return kfr::sin2x(s,c); }

    template<typename T>    
    T ksin3x(const T & s, const T & c) {return kfr::sin3x(s,c); }

    template<typename T>    
    UniVector<T> cossin(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::cossin(v.vec); return r; }

    template<typename T>    
    UniVector<T> sincos(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::sincos(v.vec); return r; }

    template<typename T>    
    T kreciprocal(const T & v) { return kfr::reciprocal(v); }

    template<typename T>    
    T rem(const T v,const T b) { return kfr::rem(v,b); }    

    template<typename T>    
    T satadd(const T v,const T y) { return kfr::satadd(v,y); }

    template<typename T>    
    T satsub(const T v,const T  y) { return kfr::satsub(v,y); }

    //? dont know how to make these work yet.
    template<typename T>    
    UniVector<T> fastcos(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::fastcos(v.vec); return r; }

    template<typename T>    
    UniVector<T> fastcosdeg(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::fastcosdeg(v.vec); return r; }

    template<typename T>    
    UniVector<T> fastsin(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::fastsin(v.vec); return r; }

    template<typename T>    
    UniVector<T> fastsindeg(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::fastsindeg(v.vec); return r; }        

    template<typename T>    
    UniVector<T> coshsinh(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::coshsinh(v.vec); return r; }

    template<typename T>    
    UniVector<T> sinhcosh(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::sinhcosh(v.vec); return r; }

    template<typename T>    
    UniVector<T> cossindeg(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::cossindeg(v.vec); return r; }    

    template<typename T>    
    UniVector<T> sincosdeg(const UniVector<T> & v) { UniVector<T> r; r.vec = kfr::sincosdeg(v.vec); return r; }    


    template<typename T>    
    UniVector<T> random(size_t s) 
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distrib(0.0,1.0);
        UniVector<T> r(s);    
        for(size_t i = 0; i < s; i++)
            r.vec[i] = distrib(generator);
        return r;
    }   

    template<typename T>    
    UniVector<T> random(size_t s, T min, T max) 
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distrib(min,max);
        UniVector<T> r(s);    
        for(size_t i = 0; i < s; i++)
            r.vec[i] = distrib(generator);
        return r;
    }  

    template<typename T>
    void plot_save(const UniVector<T> & v, const std::string& name="", const std::string& options="") {
            kfr::plot_save(name,v.vec,options);
        }
    template<typename T>    
    void plot_show(const UniVector<T> & v, const std::string& name="", const std::string&  options="") {
        kfr::plot_show(name,v.vec,options);
    }


    /* running expression filter
    {
            // filter initialization
            biquad_params<fbase> bq[]       = { biquad_lowpass(0.2, 0.9) };
            expression_filter<fbase> filter = to_filter(biquad(bq, placeholder<fbase>()));

            // prepare data
            output = unitimpulse();

            // apply filter
            filter.apply(output);
        }
    */

    template<typename T>
    struct BiQuadParams
    {
        std::vector<kfr::biquad_params<T>>  bq;

        BiQuadParams() = default;
        ~BiQuadParams() = default;

        kfr::biquad_params<T>* get_params() { return bq.data(); }

        kfr::biquad_params<T> biquad_notch(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_lowpass(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_highpass(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_peak(T f, T q, T gain) {
            return(kfr::biquad_params<T>(kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain))));
        }
        kfr::biquad_params<T> biquad_lowshelf(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_highshelf(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }
        kfr::biquad_params<T> biquad_bandpass(T f, T q) {
            return(kfr::biquad_params<T>(kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q))));
        }

        void add_notch(T f, T q) {
            kfr::biquad_params<T> p  = biquad_notch(f,q);
            bq.push_back(p);
        }
        void add_lowpass(T f, T q) {
            kfr::biquad_params<T> p  = biquad_lowpass(f,q);
            bq.push_back(p);
        }
        void add_highpass(T f, T q) {
            kfr::biquad_params<T> p  = biquad_highpass(f,q);
            bq.push_back(p);
        }
        void add_peak(T f, T q, T gain) {
            kfr::biquad_params<T> p  = biquad_peak(f,q, gain);
            bq.push_back(p);
        }
        void add_lowshelf(T f, T q) {
            kfr::biquad_params<T> p  = biquad_lowshelf(f,q);
            bq.push_back(p);
        }
        void add_highself(T f, T q) {
            kfr::biquad_params<T> p  = biquad_highshelf(f,q);
            bq.push_back(p);            
        }
        void add_bandpass(T f, T q) {
            kfr::biquad_params<T> p  = biquad_bandpass(f,q);
            bq.push_back(p);
        }
    };


    // it is kind of a mess right now and will remain that way FOREVER
    template<typename T>
    struct Biquad
    {
        kfr::biquad_filter<T> * filter;

        void init(const BiQuadParams<T> & params)
        {                       
            filter = new kfr::biquad_filter<T>(params.bq.data(),1);
        }
        void init(const kfr::biquad_params<T> & params)
        {                       
            filter = new kfr::biquad_filter<T>(&params,1);    
        }
        void init(const std::vector<kfr::biquad_params<T>> & params)
        {                       
            filter = new kfr::biquad_filter<T>(params.data(),1);    
        }

        Biquad() = default;

        Biquad(const kfr::biquad_params<T> & params) {         
            init(params); 
        }        
        Biquad(const BiQuadParams<T> & params) {         
            init(params); 
        }        
        ~Biquad() { if(filter) delete filter; }

        
        void notch(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void lowpass(T f, T q) {
            kfr::biquad_params<T> params =  kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void highpass(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void peak(T f, T q, T gain) {
            kfr::biquad_params<T> params = kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain));
            init(params);
        }
        void lowshelf(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void highshelf(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }
        void bandpass(T f, T q) {
            kfr::biquad_params<T> params = kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
            init(params);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter->apply(vector.vec);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter->apply(dst.vec,src.vec);
        }

        void reset() { filter->reset(); }
    };

    
    template<typename T>
    kfr::biquad_params<T> notch(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> lowpass(T f, T q) {
        kfr::biquad_params<T> params =  kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> highpass(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> peak(T f, T q, T gain) {
        kfr::biquad_params<T> params = kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> lowshelf(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> highshelf(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> bandpass(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }

    
    template<typename T>
    struct NotchFilter {
        Biquad<T> filter;

        NotchFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.notch(f,q);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct LowPassFilter {
        Biquad<T> filter;

        LowPassFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.lowpass(f,q);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct HighPassFilter {
        Biquad<T> filter;

        HighPassFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.highpass(f,q);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct BandPassFilter {
        Biquad<T> filter;

        BandPassFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.bandpass(f,q);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct PeakFilter {
        Biquad<T> filter;

        PeakFilter( const BiQuadParams<T>& p, T f, T q, T gain) {        
            filter.peak(f,q,gain);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct LowShelfFilter {
        Biquad<T> filter;

        LowShelfFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.lowshelf(f,q);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T>
    struct HighShelfFilter {
        Biquad<T> filter;

        HighShelfFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.highshelf(f,q);
        }

        void apply(UniVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const UniVector<T> & src, UniVector<T> & dst) 
        {
            filter.apply(src,dst);
        }

        void reset() { filter.reset(); }
    };

    template<typename T> T normalize_frequency(T f, T sample_rate) {
        return f/sample_rate;
    }
    // simple apply biquad functions
    template<typename T>
    UniVector<T> biquad(const BiQuadParams<T> & bp, UniVector<T> & input) {
        return UniVector<T>(kfr::biquad(bp.bq,input.vec));
    }

    // simple biquad filter functions
    template<typename T>
    UniVector<T> lowpassfilter(const UniVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.lowpass(freq,q);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    UniVector<T> highpassfilter(const UniVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.highpass(freq,q);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    UniVector<T> notchfilter(const UniVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.notch(freq,q);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    UniVector<T> bandpassfilter(const UniVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.bandpass(freq,q);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    UniVector<T> peakfilter(const UniVector<T> & input, T freq, T q, T gain) {
        Biquad<T> filter;
        filter.peak(freq,q,gain);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    UniVector<T> lowshelffilter(const UniVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.lowshelf(freq,q);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;    
    }
    template<typename T>
    UniVector<T> highshelffilter(const UniVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.highshelf(freq,q);
        UniVector<T> r(input);
        filter.apply(r,input);
        return r;
    }




    ////////////////////////////////////////////////////////////////////////////////
    // Sound File I/O
    // KFR and libnyquist
    ////////////////////////////////////////////////////////////////////////////////

    using seek_origin = kfr::seek_origin;
    using audio_format = kfr::audio_format_and_length;

    
    template<typename T>
    struct WavReader {
    private:
        kfr::audio_reader_wav<T> *reader;
                
    public:
        WavReader(const char * filename) {
                    
            reader = new kfr::audio_reader_wav<T>(kfr::open_file_for_reading(filename));    
            assert(reader != NULL);
        }
        ~WavReader() {
            if(reader) {                
                delete reader;                
            }
        }
        size_t read(UniVector<T> & vec) {
            return reader->read(vec.data(),vec.size());
        }

        size_t tell() const {
            return reader->tell();
        }

        bool seek(size_t offset, seek_origin origin ) {
            return reader->seek(offset,origin);
        }
        
        const audio_format& format() {
            return reader->format();
        }
    };

    template<typename T>
    UniVector<T> load_wav(const char * filename) {
        WavReader<T> r(filename);
        r.seek(0,kfr::seek_origin::end);
        size_t size = r.tell();
        r.seek(0,kfr::seek_origin::begin);
        UniVector<T> v(size);
        r.read(v);
        return v;
    }

    template<typename T>
    struct WavWriter {
        kfr::audio_writer_wav<T> * writer;
        
        
        WavWriter(const char * filename, const audio_format & format) {            
            writer = new kfr::audio_writer_wav<T>(kfr::open_file_for_writing(filename),format);    
            assert(writer != NULL);
        }
        ~WavWriter() {
            if(writer) {                                
                delete writer;
            }
        }
        void write(UniVector<T> & vec) {
            writer->write(vec.data(),vec.size());
        }
        const audio_format& format() {
            return writer->format();
        }
    };

    template<typename T>
    void write_wav(UniVector<T> & v, const char * filename, size_t channels, int sample_type, double sample_rate, bool use_w64=false) {
        WavWriter<T> w(filename,kfr::audio_format{channels,static_cast<kfr::audio_sample_type>(sample_type),sample_rate,use_w64});
        w.write(v);        
    }

    template<typename T>
    struct MP3Reader {
        kfr::audio_reader_mp3<T> * reader;
        
        MP3Reader(const char * filename) {            
            reader = new kfr::audio_reader_mp3<T>(kfr::open_file_for_reading(filename));    
            assert(reader != NULL);
        }
        ~MP3Reader() {
            if(reader != NULL) {                
                delete reader;
            }
        }
        size_t read(UniVector<T> & vec) {
            return reader->read(vec.data(),vec.size());
        }

        size_t tell() const {
            return reader->tell();
        }

        bool seek(size_t offset, seek_origin origin ) {
            return reader->seek(offset,origin);
        }
        
        const audio_format& format() {
            return reader->format();
        }
    };

    template<typename T>
    UniVector<T> load_mp3(const char * filename) {
        MP3Reader<T> r(filename);
        r.seek(0,kfr::seek_origin::end);
        size_t size = r.tell();
        r.seek(0,kfr::seek_origin::begin);
        UniVector<T> v(size);
        r.read(v);
        return v;
    }
    template<typename T>
    struct FlacReader {
        kfr::audio_reader_flac<T> *reader;
                
        FlacReader(const char * filename) {                    
            reader = new kfr::audio_reader_flac<T>(kfr::open_file_for_reading(filename));    
            assert(reader != NULL);
        }
        ~FlacReader() {
            if(reader != NULL) {
                delete reader;
            }
        }
        size_t read(UniVector<T> & vec) {
            return reader->read(vec.data(),vec.size());
        }

        size_t tell() const {
            return reader->tell();
        }

        bool seek(size_t offset, seek_origin origin ) {
            return reader->seek(offset,origin);
        }
        
        const audio_format& format() {
            return reader->format();
        }
    };

    template<typename T>
    UniVector<T> load_flac(const char * filename) {
        FlacReader<T> r(filename);
        r.seek(0,kfr::seek_origin::end);
        size_t size = r.tell();
        r.seek(0,kfr::seek_origin::begin);
        UniVector<T> v(size);
        r.read(v);
        return v;
    }
    
    template<typename T>
    UniVector<T> autocorrelate(const UniVector<T> & v) {
        return UniVector<T>(kfr::autocorrelate(v.vec));
    }

    template<typename T>
    UniVector<T> convolve(const UniVector<T> & v1, const UniVector<T> & v2) {
        return UniVector<T>(kfr::convolve(v1.vec,v2.vec));
    }

    template<typename T>
    struct ConvolveFilter {
    private:
        kfr::convolve_filter<T> *filter;
    
    public:
        ConvolveFilter(size_t size, size_t block_size=1024) {
            filter = new kfr::convolve_filter<T>(size,block_size);
            assert(filter != nullptr);
        }
        ConvolveFilter(UniVector<T> & u, size_t block_size=1024) {
            filter = new kfr::convolve_filter<T>(u.vec,block_size);
            assert(filter != nullptr);
        }
        ~ConvolveFilter() {
            if(filter) delete filter;
        }
        void set_data(const UniVector<T> & u) { filter->set_data(u.vec); }
        void reset() { filter->reset(); }
        size_t input_block_size() const { return filter->input_block_size(); }

        void apply(UniVector<T> & in) {
            filter->apply(in.vec);
        }
        void apply(const UniVector<T> & in, UniVector<T> & out) {
            filter->apply(out.vec,in.vec);
        }
    };  

    

    template<typename T>
    UniVector<T> correlate(const UniVector<T> & v1,const UniVector<T> & v2) {
        return UniVector<T>(kfr::correlate(v1.vec,v2.vec));
    }

    template<typename T>
    struct DCTPlan 
    {
    private:    
        kfr::dct_plan<T> plan;    
    public:
        DCTPlan(size_t size) : plan(size) {}
        
        void execute(UniVector<T> & out, const UniVector<T> & in, bool inverse=false) {
            UniVector<kfr::u8> temp(in.size());
            plan.execute(out.vec.data(),in.vec.data(),temp.vec.data(),inverse);
        }
    };


    using dft_order = kfr::dft_order;

    template<typename T>
    struct DFTPlan 
    {
    private:
        kfr::dft_plan<T> *plan;
    public:
        DFTPlan(size_t size) {
            plan = new kfr::dft_plan<T>(size);
        }
        ~DFTPlan() {
            if(plan) delete plan;
        }
        void dump() const { plan->dump(); }
        
        void execute(UniVector<kfr::complex<T>> & out, const UniVector<kfr::complex<T>> & in, bool inverse = false) {
            UniVector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.vec.data(),in.vec.data(),temp.vec.data(),inverse);
        }
    };

    template<typename T>
    struct DFTRealPlan
    {
    private:
        kfr::dft_plan_real<T> *plan;
    public:
        DFTRealPlan(size_t size) {
            plan = new kfr::dft_plan_real<T>(size);
        }
        ~DFTRealPlan() {
            if(plan) delete plan;
        }
        void dump() const { plan->dump(); }

        void execute_forward(UniVector<kfr::complex<T>> & out, const UniVector<T> & in) {
            UniVector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.vec,in.vec,temp.vec);
        }
        void execute_reverse(UniVector<T> & out, const UniVector<kfr::complex<T>> & in) {
            UniVector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.vec,in.vec,temp.vec);
        }
    };

    template<typename T>
    UniVector<kfr::complex<T>> run_dft(const UniVector<kfr::complex<T>> & data) { 
        return UniVector<kfr::complex<T>>(kfr::dft(data.vec)); 
    }

    // sets imaginary to zero, return complex
    template<typename T>
    UniVector<kfr::complex<T>> run_realdft(const UniVector<T> & data) { 
        UniVector<kfr::complex<T>> tmp;
        tmp.resize(data.size());    
        for(size_t i = 0; i < data.size(); i++)
        {
            tmp[i] = kfr::complex<T>(data[i],(T)0);
        }
        return UniVector<kfr::complex<T>>(kfr::dft(tmp.vec)); 
    }

    template<typename T>
    UniVector<kfr::complex<T>> run_idft(const UniVector<kfr::complex<T>> & data) { return UniVector<kfr::complex<T>>(kfr::idft(data.vec)); }

    template<typename T>
    UniVector<T> run_irealdft(const UniVector<kfr::complex<T>> & data) { return UniVector<T>(kfr::irealdft(data.vec)); }


    using sample_rate_conversion_quality = kfr::sample_rate_conversion_quality;


    template<typename T>
    UniVector<T> resample(sample_rate_conversion_quality quality, const UniVector<T> & input, size_t output_sr, size_t input_sr) {    
        auto r = kfr::resampler<T>(quality,output_sr, input_sr);    
        UniVector<T> output(input.size() * output_sr/input_sr + r.get_delay());
        r.process(output.vec,input.vec);
        return output;    
    }

    template<typename T>
    UniVector<T> convert_sample(const UniVector<T> & input) {
        UniVector<T> output(input.size());
        kfr::convert(output.vec.data(),input.vec.data(),input.size());
        return output;
    }

    template<typename T>
    UniVector<T> amp_to_dB(const UniVector<T> & in) {
        return UniVector<T>(kfr::amp_to_dB(in.vec));
    }

    
    template<typename T>
    struct FIRFilter {
    private:
        kfr::filter_fir<SampleType> * filter;
        UniVector<T> taps;
        
    public:
        
        FIRFilter(size_t num_taps) { 
            taps.resize(num_taps); 
            filter = nullptr;
        }
        ~FIRFilter() {
            if(filter != NULL) delete filter;
        }
        void bandpass(T x, T y, kfr::expression_pointer<T> & window, bool normalize=true ) {        
            kfr::fir_bandpass(taps.vec,x,y,window,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void bandstop(T x, T y, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_bandstop(taps.vec,x,y, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void highpass(T cutoff, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_highpass(taps.vec,cutoff, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void lowpass(T cutoff, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_lowpass(taps.vec,cutoff, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        
        void apply(UniVector<T> & data) {
            filter->apply(data.vec);
        }
        void apply(UniVector<T> & out, const UniVector<T> & in) {
            filter->apply(out.vec,in.vec);
        }
        void reset() { filter->reset(); }

    };

    template<typename T>
    struct FIRBandpassFilter
    {
        FIRFilter<T> * filter;

        FIRBandpassFilter(size_t num_taps, T x, T y, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->bandpass(x,y,window,normalize);
        }
        ~FIRBandpassFilter() {
            if(filter) delete filter;
        }
        void apply(UniVector<T> & data) {
            filter->apply(data);
        }
        void apply(UniVector<T> & out, const UniVector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };

    template<typename T>
    struct FIRBandstopFilter
    {
        FIRFilter<T> * filter;

        FIRBandstopFilter(size_t num_taps, T x, T y, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->bandstop(x,y,window,normalize);
        }
        ~FIRBandstopFilter() {
            if(filter) delete filter;
        }
        void apply(UniVector<T> & data) {
            filter->apply(data);
        }
        void apply(UniVector<T> & out, const UniVector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };

    template<typename T>
    struct FIRLowpassFilter
    {
        FIRFilter<T> * filter;

        FIRLowpassFilter(size_t num_taps, T x, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->lowpass(x,window,normalize);
        }
        ~FIRLowpassFilter() {
            if(filter) delete filter;
        }
        void apply(UniVector<T> & data) {
            filter->apply(data);
        }
        void apply(UniVector<T> & out, const UniVector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };

    template<typename T>
    struct FIRHighpassFilter
    {
        FIRFilter<T> * filter;

        FIRHighpassFilter(size_t num_taps, T x, kfr::expression_pointer<T> & window, bool normalize = true) {
            filter = new FIRFilter<T>(num_taps);
            assert(filter != NULL);
            filter->highpass(x,window,normalize);
        }
        ~FIRHighpassFilter() {
            if(filter) delete filter;
        }
        void apply(UniVector<T> & data) {
            filter->apply(data);
        }
        void apply(UniVector<T> & out, const UniVector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };



    template<typename T>
    UniVector<T> fir_lowpass(const UniVector<T> & input, size_t num_taps, T cutoff, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.lowpass(cutoff, window_type, normalize);
        UniVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    UniVector<T> fir_highpass(const UniVector<T> & input, size_t num_taps, T cutoff, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.highpass(cutoff, window_type, normalize);
        UniVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    UniVector<T> fir_bandpass(const UniVector<T> & input, size_t num_taps, T x, T y, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.bandpass(x, y, window_type, normalize);
        UniVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    UniVector<T> fir_bandstop(const UniVector<T> & input, size_t num_taps, T x, T y, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.bandstop(x, y, window_type, normalize);
        UniVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }


        
    template<typename T>
    struct BesselFilter 
    {        
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;

        int order;
        static constexpr size_t maxorder=32;

        BesselFilter() = default;
        BesselFilter(int _order) : order(_order) {} 
        
        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::bessel<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::bessel<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::bessel<T>(order),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::bessel<T>(order),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }

        void apply(UniVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const UniVector<T> &in,UniVector<T> & out)
        {
            filter.apply(in,out);
        }
    };

    template<typename T>
    struct BesselLowPassFilter {
        BesselFilter<T> filter;

        BesselLowPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct BesselHighPassFilter {
        BesselFilter<T> filter;

        BesselHighPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct BesselBandPassFilter {
        BesselFilter<T> filter;

        BesselBandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct BesselBandStopFilter {
        BesselFilter<T> filter;

        BesselBandStopFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    UniVector<T> bessel_lowpass(int order, T lo, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        BesselLowPassFilter<T> filter(order,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> bessel_highpass(int order, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        BesselHighPassFilter<T> filter(order,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> bessel_bandpass(int order, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        BesselBandPassFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> bessel_bandstop(int order, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        BesselBandStopFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    

    template<typename T>
    struct ButterworthFilter {
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;
        int order;
        static constexpr size_t maxorder=32;

        ButterworthFilter(int _order) : order(_order) {} 

        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::butterworth<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::butterworth<T>(order),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::butterworth<T>(order),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::butterworth<T>(order),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }

        void apply(UniVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const UniVector<T> &in, UniVector<T> & out)
        {
            filter.apply(in,out);
        }
    };

    template<typename T>
    struct ButterworthLowPassFilter {
        ButterworthFilter<T> filter;

        ButterworthLowPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct ButterworthHighPassFilter {
        ButterworthFilter<T> filter;

        ButterworthHighPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct ButterworthBandPassFilter {
        ButterworthFilter<T> filter;

        ButterworthBandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct ButterworthBandStopFilter {
        ButterworthFilter<T> filter;

        ButterworthBandStopFilter(int order, T lo_cutoff, T hi_cutoff ,T sample_rate) : filter(order) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    UniVector<T> butterworth_lowpass(int order, T lo, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        ButterworthLowPassFilter<T> filter(order,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> butterworth_highpass(int order, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        ButterworthHighPassFilter<T> filter(order,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> butterworth_bandpass(int order, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        ButterworthBandPassFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> butterworth_bandstop(int order,  T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        ButterworthBandStopFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    


    template<typename T>
    struct Chebyshev1Filter {
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;
        int order;
        T rp;
        static constexpr size_t maxorder=32;

        Chebyshev1Filter() = default;
        Chebyshev1Filter(int _order, T _rp) : order(_order), rp(_rp) {} 

        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::chebyshev1<T>(order,rp),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::chebyshev1<T>(order,rp),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev1<T>(order,rp),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev1<T>(order,rp),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }

        void apply(UniVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const UniVector<T> &in,UniVector<T> & out)
        {
            filter.apply(in,out);
        }
    };

    template<typename T>
    struct Chebyshev1LowPassFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1LowPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct Chebyshev1HighPassFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1HighPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev1BandPassFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1BandPassFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev1BandStopFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1BandStopFilter(int order, T rp, T lo_cutoff, T hi_cutoff ,T sample_rate) : filter(order,rp) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    UniVector<T> chebyshev1_lowpass(int order, T rp, T lo, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev1LowPassFilter<T> filter(order,rp,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> chebyshev1_highpass(int order, T rp,T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev1HighPassFilter<T> filter(order,rp,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> chebyshev1_bandpass(int order, T rp, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev1BandPassFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> chebyshev1_bandstop(int order, T rp, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev1BandStopFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    

    template<typename T>
    struct Chebyshev2Filter {
        std::vector<kfr::biquad_params<T>> bqs;
        Biquad<T> filter;
        int order;
        T rs;
        static constexpr size_t maxorder=32;

        Chebyshev2Filter() = default;
        Chebyshev2Filter(int _order, T _rs) : order(_order), rs(_rs) {} 

        void lowpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_lowpass(kfr::chebyshev2<T>(order,rs),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void highpass(T cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_highpass(kfr::chebyshev2<T>(order,rs),cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandpass(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev2<T>(order,rs),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }
        void bandstop(T lo_cutoff, T hi_cutoff, T sample_rate) {
            kfr::zpk<T> filt = kfr::iir_bandpass(kfr::chebyshev2<T>(order,rs),lo_cutoff,hi_cutoff,sample_rate);
            bqs  = to_sos(filt);
            filter.init(bqs);
        }

        void apply(UniVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const UniVector<T> &in, UniVector<T> & out)
        {
            filter.apply(in,out);
        }
    };

    
    template<typename T>
    struct Chebyshev2LowPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2LowPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.lowpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct Chebyshev2HighPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2HighPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev2BandPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2BandPassFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev2BandStopFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2BandStopFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(UniVector<T> &in) { filter.apply(in);}
        void apply(const UniVector<T> &in, UniVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    UniVector<T> chebyshev2_lowpass(int order, T rp, T lo, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev2LowPassFilter<T> filter(order,rp,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> chebyshev2_highpass(int order, T rp,T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev2HighPassFilter<T> filter(order,rp,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> chebyshev2_bandpass(int order, T rp, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev2BandPassFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    UniVector<T> chebyshev2_bandstop(int order, T rp, T lo, T hi, T sample_rate, const UniVector<T> & in) {
        UniVector<T> out(in.size());
        Chebyshev2BandStopFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }

    template<typename T>
    UniVector<T> dcremove(const UniVector<T> & input, T cutoff) {
        return UniVector<T>(kfr::CMT_ARCH_NAME::dcremove(input.vec,cutoff));
    }

    template<typename T>
    UniVector<T> sinewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        UniVector<T> r(n);                
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::sine(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }

    template<typename T>
    UniVector<T> squarewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        UniVector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::square(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }

    template<typename T>
    UniVector<T> trianglewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        UniVector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::triangle(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }
    
    template<typename T>
    UniVector<T> sawtoothwave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        UniVector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::sawtooth(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }

    template<typename T>
    UniVector<T> generate_sin(size_t n, T start, T step) {
        UniVector<T> r(n);
        r.vec = kfr::gen_sin(start,step);
        return r;
    }

    template<typename T>
    UniVector<T> generate_linear(size_t n, T start, T step) {
        UniVector<T> r(n);
        r.vec = kfr::gen_linear(start,step);
        return r;
    }

    template<typename T>
    UniVector<T> generate_exp(size_t n, T start, T step) {
        UniVector<T> r(n);
        r.vec = kfr::gen_exp(start,step);
        return r;
    }

    template<typename T>
    UniVector<T> generate_exp2(size_t n, T start, T step) {
        UniVector<T> r(n);
        r.vec = kfr::gen_exp2(start,step);
        return r;        
    }

    template<typename T>
    UniVector<T> generate_cossin(size_t n, T start, T step) {
        UniVector<T> r(n);
        r.vec = kfr::gen_cossin(start,step);
        return r;
    }

    template<typename T>
    void do_deinterleave(UniVector2D<T> & out, const T * ptr, size_t size, size_t channels) {                
        out.resize(channels,size);
        std::vector<T*> p(out.size());
        for(size_t i = 0; i < out.size(); i++) p[i] = out[i].data();
        kfr::deinterleave(p.data(),ptr,channels,size);
    }
    
    template<typename T>
    UniVector<T> do_interleave(const UniVector2D<T> & input) {        
        return UniVector<T>(kfr::interleave(input.vec));
    }

    template<typename T>
    UniVector<T> generate_sequence(int n, int start, int step) {
        UniVector<T> r(n);
        for(size_t i = 0; i < n; i++)
            r[i] = start + step*n;
        return r;
    }


    std::string dB_to_string(const double value, double min=-140.0f) { return kfr::dB_to_string(value,min); }
    std::string dB_to_utf8string(const double value, double min=-140.0f) { return kfr::dB_to_utf8string(value,min); }

    //constexpr size_t audio_sample_sizeof(audio_sample_type type) { return kfr::audio_sample_sizeof(type); }
    //constexpr size_t audio_sample_bit_depth(audio_sample_type type) { return kfr::audio_sample_bit_depth(type); }

}


