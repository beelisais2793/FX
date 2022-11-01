#pragma once 

#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <random> 

#include <kfr/kfr.h>
#include <kfr/dsp.hpp>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>

typedef double SampleType;

namespace DSP {

    template<typename T> struct SampleVector;
    template<typename T> struct SampleMatrix;

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

        SampleVector<T> operator + (const SampleVector<T> & s);
        SampleVector<T> operator * (const SampleVector<T> & s);

        

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
    

    template<typename T>
    struct Complex
    {
        kfr::complex<T> c;

        Complex() = default;
        Complex(const T r, const T i) : c(r,i) {}
        Complex(const kfr::complex<T> & v) : c(v) {} 

        //std::complex<T> complex() const { std::complex<T> r(real(c),imag(c)); return r; }

        Complex<T>& operator = (const Complex<T>& u) { c = u.c; return *this; }
        Complex<T>& operator = (const kfr::complex<T> & u) { c = u; return *this; }

        Complex<T>  operator + (const Complex<T>& u) { Complex<T> r; r.c=(c+u.c); return r; }
        Complex<T>  operator - (const Complex<T>& u) { Complex<T> r; r.c=(c-u.c); return r; }
        Complex<T>  operator * (const Complex<T>& u) { Complex<T> r; r.c=(c*u.c); return r; }
        Complex<T>  operator / (const Complex<T>& u) { Complex<T> r; r.c=(c/u.c); return r; }
                
        Complex<T> cabs() { return Complex<T>(kfr::cabs(c)); }
        Complex<T> carg() { return Complex<T>(kfr::carg(c)); }
        Complex<T> cartesian() { return Complex<T>(kfr::cartesian(c)); }
        Complex<T> conj() { return Complex<T>(kfr::cconj(c)); }
        Complex<T> ccos() { return Complex<T>(kfr::ccos(c)); }
        Complex<T> csin() { return Complex<T>(kfr::csin(c)); }    
        Complex<T> ccosh() { return Complex<T>(kfr::ccosh(c)); }
        Complex<T> csinh() { return Complex<T>(kfr::csinh(c)); }    
        Complex<T> cexp() { return Complex<T>(kfr::cexp(c)); }
        Complex<T> cexp10() { return Complex<T>(kfr::cexp10(c)); }
        Complex<T> cexp2() { return Complex<T>(kfr::cexp2(c)); }
        Complex<T> clog() { return Complex<T>(kfr::clog(c)); }
        Complex<T> clog10() { return Complex<T>(kfr::clog10(c)); }
        Complex<T> clog2() { return Complex<T>(kfr::clog2(c)); }
        Complex<T> csqr() { return Complex<T>(kfr::csqr(c)); }
        Complex<T> csqrt() { return Complex<T>(kfr::csqrt(c)); }
        Complex<T> polar() { return Complex<T>(kfr::polar(c)); }
        
        T imag() { return kfr::imag(c); }
        T real() { return kfr::real(c); }

        void imag(const T r) { return c.real(r); }
        void real(const T i) { return c.imag(i); }

        bool isreal() { return kfr::isreal(c); }
        
    };

    template<typename T>
    Complex<T>& cabs(const Complex<T> & a) { return a.cabs(); }
    template<typename T>
    Complex<T>& carg(const Complex<T> & a) { return a.carg(); }
    template<typename T>
    Complex<T>& cartesian(const Complex<T> & a) { return a.cartesian(); }
    template<typename T>
    Complex<T>& conj(const Complex<T> & a) { return a.cconj(); }
    template<typename T>
    Complex<T>& ccos(const Complex<T> & a) { return a.ccos(); }
    template<typename T>
    Complex<T>& csin(const Complex<T> & a) { return a.csin(); }    
    template<typename T>
    Complex<T>& ccosh(const Complex<T> & a) { return a.ccosh(); }
    template<typename T>
    Complex<T>& csinh(const Complex<T> & a) { return a.csinh(); }    
    template<typename T>
    Complex<T>& cexp(const Complex<T> & a) { return a.cexp(); }
    template<typename T>
    Complex<T>& cexp10(const Complex<T> & a) { return a.cexp10(); }
    template<typename T>
    Complex<T>& cexp2(const Complex<T> & a) { return a.cexp2(); }
    template<typename T>
    Complex<T>& clog(const Complex<T> & a) { return a.clog(); }
    template<typename T>
    Complex<T>& clog10(const Complex<T> & a) { return a.clog10(); }
    template<typename T>
    Complex<T>& clog2(const Complex<T> & a) { return a.clog2(); }
    template<typename T>
    Complex<T>& csqr(const Complex<T> & a) { return a.csqr(); }
    template<typename T>
    Complex<T>& csqrt(const Complex<T> & a) { return a.csqrt(); }
    template<typename T>
    Complex<T>& polar(const Complex<T> & a) { return a.polar(); }
    template<typename T>
    T& imag(const Complex<T> & a) { return a.imag(); }
    template<typename T>
    T& real(const Complex<T> & a) { return a.real(); }
    template<typename T>
    bool isreal(const Complex<T> & a) { return a.isreal(); }

    template<typename T>
    Complex<T> make_complex(const T r, const T i) {
        return Complex<T>(kfr::make_complex(r,i));
    }
    template<typename T>
    Complex<T> make_complex(const kfr::complex<T> & c) {
        return Complex<T>(c);
    }
        
    template<typename T>
    struct SampleVector
    {
        kfr::univector<T> vector;
        size_t _channels;

        SampleVector() {
            _channels = 1;            
        }
        SampleVector(size_t n, size_t ch = 1) {
            vector.resize(n*ch);
            _channels = ch;
        }
        SampleVector(std::vector<T> & vector, size_t ch=1) {
            vector.resize(vector.size());
            _channels = ch;
            std::copy(vector.begin(),vector.end(),vector.begin());
        }
        SampleVector(T* data, size_t size) { vector = kfr::make_univector(data,size); }        
        SampleVector(const kfr::univector<T> & u) : vector(u) {} 

        size_t size() const { return vector.size(); }
        size_t channels() const { return _channels; }
        size_t samples_per_channel() const { return size() / channels(); }
        void   resize(size_t s) { vector.resize(s); }

        void fill(T value) { std::fill(vector.begin(),vector.end(),value); }
        void zero() { std::fill(vector.begin(),vector.end(),(T)0.0); }
        void copy(SampleVector<T> & v) { std::copy(v.vector.begin(),v.vector.end(),vector.begin()); }

        SampleVector<T> scalar(size_t s, const T & val) { SampleVector<T> r(s); r.fill(val); return r; }

        T& at(size_t pos) { return vector.at(pos); }
        T& front() { return vector.front(); }
        T& back() { return vector.back(); }
        T* data() { return vector.data(); }
        
        void push_back(const T val) { vector.push_back(val); }
        T    pop_back() { T x = vector.back(); vector.pop_back(); return x; }
        
        T  __getitem(size_t i) { return vector[i]; }
        void __setitem(size_t i, const T & val) { vector[i] = val; }
        
        T& operator()(size_t i, size_t ch=1) {
            return vector[i*_channels + ch];
        }
        T& operator[](size_t i) {
            return vector[i];
        }


        const T& operator()(size_t i, size_t ch=1) const {
            return vector[i*_channels + ch];
        }
        const T& operator[](size_t i) const {
            return vector[i];
        }

        SampleVector<T> get_channel(size_t ch) {
            SampleVector<T> r(samples_per_channel(),1);
            size_t x = 0;
            for(size_t i = ch; i < size(); i+=channels())
                r[x++] = vector[i];
            return r;            
        }
        void set_channel(const SampleVector<T> & v, size_t ch) {
            size_t x = 0;
            for(size_t i = ch; i < size(); i+= channels())
                vector[i] = v[x++];
        }

        SampleVector<T>& operator = (const SampleVector<T>& u) { vector = u.vector; return *this; }
        SampleVector<T>& operator = (const std::vector<T>& u) { copy(u); return *this; }

        SampleVector<T>  operator + (const SampleVector<T>& u) { SampleVector<T> r; r.vector=(vector+u.vector); return r; }
        SampleVector<T>  operator - (const SampleVector<T>& u) { SampleVector<T> r; r.vector=(vector-u.vector); return r; }
        SampleVector<T>  operator * (const SampleVector<T>& u) { SampleVector<T> r; r.vector=(vector*u.vector); return r; }
        SampleVector<T>  operator / (const SampleVector<T>& u) { SampleVector<T> r; r.vector=(vector/u.vector); return r; }
        SampleVector<T>  operator % (const SampleVector<T>& u) { SampleVector<T> r; r.vector=(vector%u.vector); return r; }

        SampleVector<T>  operator + (const T& u) { SampleVector<T> r; r.vector=(vector+u); return r; }
        SampleVector<T>  operator - (const T& u) { SampleVector<T> r; r.vector=(vector-u); return r; }
        SampleVector<T>  operator * (const T& u) { SampleVector<T> r; r.vector=(vector*u); return r; }
        SampleVector<T>  operator / (const T& u) { SampleVector<T> r; r.vector=(vector/u); return r; }
        SampleVector<T>  operator % (const T& u) { SampleVector<T> r; r.vector=(vector%u); return r; }

        SampleVector<T>&  operator += (const SampleVector<T>& u) { vector+=u.vector; return *this; }
        SampleVector<T>&  operator -= (const SampleVector<T>& u) { vector-=u.vector; return *this; }
        SampleVector<T>&  operator *= (const SampleVector<T>& u) { vector*=u.vector; return *this; }
        SampleVector<T>&  operator /= (const SampleVector<T>& u) { vector/=u.vector; return *this; }
        SampleVector<T>&  operator %= (const SampleVector<T>& u) { vector%=u.vector; }

        SampleVector<T>&  operator += (const T& u) { vector+=u; return *this; }
        SampleVector<T>&  operator -= (const T& u) { vector-=u; return *this; }
        SampleVector<T>&  operator *= (const T& u) { vector*=u; return *this; }
        SampleVector<T>&  operator /= (const T& u) { vector/=u; return *this; }
        SampleVector<T>&  operator %= (const T& u) { vector%=u; }
        
        void println() { kfr::println(vector); }

        
        void plot_save(const std::string& name="", const std::string& options="") {
            kfr::plot_save(name,vector,options);
        }

        void plot_show(const std::string& name="", const std::string&  options="") {
            kfr::plot_show(name,vector,options);
        }

    };

    template<typename T> 
    struct SampleMatrix
    {
        kfr::univector2d<T> vector; 

        SampleMatrix() {}
        SampleMatrix(size_t samples, size_t channels=2) { vector.resize(channels); for(size_t i = 0; i < channels; i++) vector[i].resize(samples); }    
        SampleMatrix(const kfr::univector2d<T> & u) : vector(u) {} 
        SampleMatrix(const SampleMatrix<T>& v) : vector(v.vector) {}
        //SampleVector(const std::initializer_list<T> & vals) { vector = kfr::make_univector(vals.begin(),vals.end()); }

        size_t size(size_t channel=0) const { return vector[channel].size(); }
        void resize(size_t c, size_t s) { 
            vector.resize(c);
            for(size_t i = 0; i < c; i++) vector[i].resize(s);
         }

        int num_channels() const { return vector.size(); }
        void fill(size_t channel, T value) { std::fill(vector[channel].begin(),vector[channel].end(),value); }
        void fill(T value) {
            for(size_t i = 0; i < num_channels(); i++)
                fill(i,value);
        }
        SampleVector<T> operator[](size_t ch) { return SampleVector<T>(vector[ch]); }

        SampleVector<T> get_channel(size_t ch) { return SampleVector<T>(vector[ch]); }
        void         set_channel(size_t ch, const SampleVector<T> & v) { vector[ch] = v.vector; }
        SampleVector<T>  __getitem(size_t ch)  { return SampleVector<T>(vector[ch]); }
        void __setitem(size_t ch, const SampleVector<T> & val) { vector[ch] = val.vector; }


        SampleMatrix<T>& operator = (const SampleMatrix<T>& u) { vector = u.vector; return *this; }        
        SampleMatrix<T>  operator + (const SampleMatrix<T>& u) { 
            SampleMatrix<T> r; 
            for(size_t i = 0; i < vector.size(); i++)
                r.vector[i] =(vector[i]+u.vector[i]); 
                return r; 
        }
        SampleMatrix<T>  operator - (const SampleMatrix<T>& u) { 
            SampleMatrix<T> r; 
            for(size_t i = 0; i < vector.size(); i++)
                r.vector[i] =(vector[i]-u.vector[i]); 
                return r; 
        }        
        SampleMatrix<T>  operator * (const SampleMatrix<T>& u) { 
            SampleMatrix<T> r; 
            for(size_t i = 0; i < vector.size(); i++)
                r.vector[i] =(vector[i]*u.vector[i]); 
                return r; 
        }
            
        SampleMatrix<T>  operator / (const SampleMatrix<T>& u) { 
            SampleMatrix<T> r; 
            for(size_t i = 0; i < vector.size(); i++)
                r.vector[i] =(vector[i]/u.vector[i]); 
                return r; 
        }

        SampleMatrix<T>  operator % (const SampleMatrix<T>& u) { 
            SampleMatrix<T> r; 
            for(size_t i = 0; i < vector.size(); i++)
                r.vector[i] =(vector[i] % u.vector[i]); 
                return r; 
        }
        
        SampleMatrix<T>&  operator += (const SampleMatrix<T>& u) { vector = vector + u.vector; return *this; }
        SampleMatrix<T>&  operator -= (const SampleMatrix<T>& u) { vector = vector - u.vector; return *this; }
        SampleMatrix<T>&  operator *= (const SampleMatrix<T>& u) { vector = vector * u.vector; return *this; }
        SampleMatrix<T>&  operator /= (const SampleMatrix<T>& u) { vector = vector / u.vector; return *this; }
        SampleMatrix<T>&  operator %= (const SampleMatrix<T>& u) { vector = vector % u.vector; return *this; }        
        
    };
    
    template<typename T>
    SampleMatrix<T> stereo(const SampleVector<T> & left, const SampleVector<T> & right) {
        SampleMatrix<T> vector = left.vector + right.vector;
        return vector;
    }


    template<typename T>
    SampleVector<T> make_univec(const T * data, size_t s) {
        return SampleVector<T>(kfr::make_univector<T>(data,s)); 
    }
    template<typename T>
    SampleVector<T> make_window_hann(size_t s) {
        return SampleVector<T>(kfr::window_hann<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_hamming(size_t s) {
        return SampleVector<T>(kfr::window_hamming<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_blackman(size_t s, const T alpha=T(0.16), window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return SampleVector<T>(kfr::window_blackman<T>(s,alpha,symmetry));
    }
    template<typename T>
    SampleVector<T> make_window_blackman_harris(size_t s, window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return SampleVector<T>(kfr::window_blackman_harris<T>(s,symmetry));
    }
    template<typename T>
    SampleVector<T> make_window_gaussian(size_t s, const T alpha=T(0.25)) {
        return SampleVector<T>(kfr::window_gaussian<T>(s,alpha));
    }
    template<typename T>
    SampleVector<T> make_window_triangular(size_t s) {
        return SampleVector<T>(kfr::window_triangular<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_bartlett(size_t s) {
        return SampleVector<T>(kfr::window_bartlett<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_cosine(size_t s) {
        return SampleVector<T>(kfr::window_cosine<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_bartlett_hann(size_t s) {
        return SampleVector<T>(kfr::window_bartlett_hann<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_bohman(size_t s) {
        return SampleVector<T>(kfr::window_bohman<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_lanczos(size_t s) {
        return SampleVector<T>(kfr::window_lanczos<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_flattop(size_t s) {
        return SampleVector<T>(kfr::window_flattop<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_rectangular(size_t s) {
        return SampleVector<T>(kfr::window_rectangular<T>(s));
    }
    template<typename T>
    SampleVector<T> make_window_kaiser(size_t s, const T beta = T(0.5)) {
        return SampleVector<T>(kfr::window_kaiser<T>(s,beta));
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
    SampleVector<T> bitwiseand(const SampleVector<T> & a, const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::bitwiseand(a.vector,b.vector); return r; }
    template<typename T>
    SampleVector<T> bitwiseandnot(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::bitwiseandnot(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> bitwisenot(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::bitwisenot(a.vector); return r; }
    template<typename T>    
    SampleVector<T> bitwiseor(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::bitwiseor(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> bitwisexor(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::bitwisexor(a.vector,b.vector); return r; }

    template<typename T>    
    SampleVector<T> linspace(T start, T stop, size_t size, bool endpoint=false,bool trunc=false)
    {   
        SampleVector<T> r; 
        r.vector = kfr::linspace(start,stop,size,endpoint,trunc); 
        return r; 
    }

    template<typename T>    
    SampleVector<T> pad(const SampleVector<T> & in, const T & fill_value = T(0))
    { 
        SampleVector<T> r; 
        r.vector = kfr::padded(in.vector,fill_value); 
        return r; 
    }

    template<typename T>    
    SampleVector<T> slice(const SampleVector<T> & v, size_t start, size_t end=kfr::max_size_t)
    {   
        SampleVector<T> r;        
        r.vector = v.vector.slice(start,end);
        return r;
    }

    template<typename T>    
    SampleVector<T> truncate(const SampleVector<T> & v, size_t size)
    {   
        SampleVector<T> r; 
        r.vector = v.vector.truncate();
        return r;
    }

    template<typename T>    
    SampleVector<T> reverse(const SampleVector<T> & v)
    {   
        SampleVector<T> r;         
        r.vector = kfr::reverse(v.vector);
        return r;
    }


    template<typename T>    
    T& ringbuf_read(SampleVector<T> &v,size_t & cursor, T& value) { v.vector.ringbuf_read(cursor,value); return value; }

    template<typename T>    
    void ringbuf_write(SampleVector<T> &v, size_t & cursor, T& value) { v.vector.ringbuf_write(cursor,value); }


    template<typename T>    
    SampleVector<T> abs(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::abs(a.vector); return r; }
    template<typename T>    
    SampleVector<T> add(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::add(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> add(const SampleVector<T> & a,const T & b) { SampleVector<T> r; r.vector = kfr::add(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> absmax(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::absmax(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> absmax(const SampleVector<T> & a,const T & b) { SampleVector<T> r; r.vector = kfr::absmax(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> absmin(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::absmin(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> absmin(const SampleVector<T> & a,const T & b) { SampleVector<T> r; r.vector = kfr::absmin(a.vector,b); return r; }    
    template<typename T>    
    SampleVector<T> clamp(const SampleVector<T> & a,const SampleVector<T> & lo, const SampleVector<T> &hi) { SampleVector<T> r; r.vector = kfr::clamp(a.vector,lo.vector,hi.vector); return r; }
    template<typename T>    
    SampleVector<T> clamp(const SampleVector<T> & a,const T& lo, const T &hi) { SampleVector<T> r; r.vector = kfr::clamp(a.vector,lo,hi); return r; }
    template<typename T>    
    SampleVector<T> cube(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::cub(a.vector); return r; }
    template<typename T>    
    SampleVector<T> div(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::div(a.vector,b.vector); return r; }    
    template<typename T>    
    SampleVector<T> fmadd(const SampleVector<T> & a,const SampleVector<T> & y, const SampleVector<T> & z) { SampleVector<T> r; r.vector = kfr::fmadd(a.vector,y.vector,z.vector); return r; }
    template<typename T>    
    SampleVector<T> fmsub(const SampleVector<T> & a,const SampleVector<T> & y, const SampleVector<T> & z) { SampleVector<T> r; r.vector = kfr::fmsub(a.vector,y.vector,z.vector); return r; }    
    template<typename T>    
    SampleVector<T> max(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::max(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> max(const SampleVector<T> & a, const T & b) { SampleVector<T> r; r.vector = kfr::max(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> min(const SampleVector<T> & a, const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::min(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> min(const SampleVector<T> & a, const T & b) { SampleVector<T> r; r.vector = kfr::min(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> mix(const SampleVector<T> & a, const T& c, const SampleVector<T> & y) { SampleVector<T> r; r.vector = kfr::mix(c,a.vector,y.vector); return r; }
    template<typename T>    
    SampleVector<T> mixs(const SampleVector<T> & a, const T& c, const SampleVector<T> & y) { SampleVector<T> r; r.vector = kfr::mixs(c,a.vector,y.vector); return r; }
    template<typename T>    
    SampleVector<T> mul(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::mul(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> mul(const SampleVector<T> & a, const T & b) { SampleVector<T> r; r.vector = kfr::mul(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> neg(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::neg(a.vector); return r; }        
    template<typename T>    
    SampleVector<T> sqr(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::sqr(a.vector); return r; }
    template<typename T>    
    SampleVector<T> sqrt(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::sqrt(a.vector); return r; }
    template<typename T>    
    SampleVector<T> exp(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::exp(a.vector); return r; }
    template<typename T>    
    SampleVector<T> exp10(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::exp10(a.vector); return r; }
    template<typename T>    
    SampleVector<T> exp2(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::exp2(a.vector); return r; }
    template<typename T>    
    SampleVector<T> exp_fmadd(const SampleVector<T> & a,const SampleVector<T> & y, const SampleVector<T> & z) { SampleVector<T> r; r.vector = kfr::exp_fmadd(a.vector,y.vector,z.vector); return r; }
    template<typename T>    
    SampleVector<T> log(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::log(a.vector); return r; }
    template<typename T>    
    SampleVector<T> log10(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::log10(a.vector); return r; }
    template<typename T>    
    SampleVector<T> log2(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::log2(a.vector); return r; }
    template<typename T>    
    SampleVector<T> log_fmadd(const SampleVector<T> & a,const SampleVector<T> & y, const SampleVector<T> & z) { SampleVector<T> r; r.vector = kfr::log_fmadd(a.vector,y.vector,z.vector); return r; }
    template<typename T>    
    SampleVector<T> logb(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::logb(a.vector); return r; }
    template<typename T>    
    SampleVector<T> logm(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::logm(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> logn(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::logn(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> pow(const SampleVector<T> & a,const T & y) { SampleVector<T> r; r.vector = kfr::pow(a.vector,y); return r; }
    template<typename T>    
    SampleVector<T> pow(const SampleVector<T> & a,const SampleVector<T> & y) { SampleVector<T> r; r.vector = kfr::pow(a.vector,y.vector); return r; }
    template<typename T>    
    SampleVector<T> root(const SampleVector<T> & a,const SampleVector<T> & y) { SampleVector<T> r; r.vector = kfr::root(a.vector,y.vector); return r; }
    template<typename T>    
    SampleVector<T> floor(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::floor(a.vector); return r; }        
    template<typename T>    
    SampleVector<T> acos(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::acos(a.vector); return r; }
    template<typename T>    
    SampleVector<T> asin(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::asin(a.vector); return r; }
    template<typename T>    
    SampleVector<T> atan(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::atan(a.vector); return r; }
    template<typename T>    
    SampleVector<T> atan2(const SampleVector<T> & a,const T & b) { SampleVector<T> r; r.vector = kfr::atan2(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> atan2(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::atan2(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> atan2deg(const SampleVector<T> & a,const T & b) { SampleVector<T> r; r.vector = kfr::atan2deg(a.vector,b); return r; }
    template<typename T>    
    SampleVector<T> atan2deg(const SampleVector<T> & a,const SampleVector<T> & b) { SampleVector<T> r; r.vector = kfr::atan2deg(a.vector,b.vector); return r; }
    template<typename T>    
    SampleVector<T> atandeg(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::atandeg(a.vector); return r; }
    template<typename T>    
    SampleVector<T> cos(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::cos(a.vector); return r; }
    template<typename T>    
    SampleVector<T> sin(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::sin(a.vector); return r; }    
    template<typename T>    
    SampleVector<T> cosdeg(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::cosdeg(a.vector); return r; }        
    template<typename T>    
    SampleVector<T> sindeg(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::cosdeg(a.vector); return r; }    
    template<typename T>    
    SampleVector<T> sinc(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::sinc(a.vector); return r; }
    template<typename T>    
    SampleVector<T> tan(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::tan(a.vector); return r; }        
    template<typename T>    
    SampleVector<T> cosh(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::cosh(a.vector); return r; }
    template<typename T>    
    SampleVector<T> coth(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::coth(a.vector); return r; }    
    template<typename T>    
    SampleVector<T> sinh(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::sinh(a.vector); return r; }    
    template<typename T>    
    SampleVector<T> tanh(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::tanh(a.vector); return r; }
    template<typename T>    
    SampleVector<T> gamma(const SampleVector<T> & a) { SampleVector<T> r; r.vector = kfr::gamma(a.vector); return r; }

    template<typename T>    
    T absmaxof(const SampleVector<T> & a) { return kfr::absmaxof(a.vector); }
    template<typename T>    
    T absminof(const SampleVector<T> & a) { return kfr::absminof(a.vector); }
    template<typename T>    
    T dot(const SampleVector<T> & a,const SampleVector<T> & b) { return kfr::dotproduct(a.vector,b.vector); }
    template<typename T>    
    T maxof(const SampleVector<T> & a) { return kfr::maxof(a.vector); }
    template<typename T>    
    T minof(const SampleVector<T> & a) { return kfr::minof(a.vector); }
    template<typename T>    
    T mean(const SampleVector<T> & a) { return kfr::mean(a.vector); }
    template<typename T>    
    T product(const SampleVector<T> & a) { return kfr::product(a.vector); }
    template<typename T>    
    T rms(const SampleVector<T> & a) { return kfr::rms(a.vector); }
    template<typename T>    
    T sum(const SampleVector<T> & a) { return kfr::sum(a.vector); }
    template<typename T>    
    T sumsqr(const SampleVector<T> & a) { return kfr::sumsqr(a.vector); }

    // doesn't compile
    //template<typename T>    
    //SampleVector<T> div(const SampleVector<T> & a,const T b) { SampleVector<T> r; r.vector = kfr::div<T>(a.vector,b); return r; }

    template<typename T>    
    SampleVector<T> ipow(const SampleVector<T> & v, int base) { SampleVector<T> r; r.vector = kfr::ipow(v.vector,base); return r; }

    template<typename T>    
    T kcos2x(const T s, const T c) {return kfr::cos2x<SampleType>(s,c); }

    template<typename T>    
    T kcos3x(const T & s, const T & c) {return kfr::cos3x(s,c); }

    template<typename T>    
    T ksin2x(const T & s, const T & c) {return kfr::sin2x(s,c); }

    template<typename T>    
    T ksin3x(const T & s, const T & c) {return kfr::sin3x(s,c); }

    template<typename T>    
    SampleVector<T> cossin(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::cossin(v.vector); return r; }

    template<typename T>    
    SampleVector<T> sincos(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::sincos(v.vector); return r; }

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
    SampleVector<T> fastcos(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::fastcos(v.vector); return r; }

    template<typename T>    
    SampleVector<T> fastcosdeg(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::fastcosdeg(v.vector); return r; }

    template<typename T>    
    SampleVector<T> fastsin(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::fastsin(v.vector); return r; }

    template<typename T>    
    SampleVector<T> fastsindeg(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::fastsindeg(v.vector); return r; }        

    template<typename T>    
    SampleVector<T> coshsinh(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::coshsinh(v.vector); return r; }

    template<typename T>    
    SampleVector<T> sinhcosh(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::sinhcosh(v.vector); return r; }

    template<typename T>    
    SampleVector<T> cossindeg(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::cossindeg(v.vector); return r; }    

    template<typename T>    
    SampleVector<T> sincosdeg(const SampleVector<T> & v) { SampleVector<T> r; r.vector = kfr::sincosdeg(v.vector); return r; }    

    template<typename T>
    SampleVector<T> autocorrelate(const SampleVector<T> & v) {
        return SampleVector<T>(kfr::autocorrelate(v.vector));
    }

    template<typename T>
    SampleVector<T> correlate(const SampleVector<T> & v1,const SampleVector<T> & v2) {
        return SampleVector<T>(kfr::correlate(v1.vector,v2.vector));
    }

    template<typename T>
    SampleVector<T> convolve(const SampleVector<T> & v1, const SampleVector<T> & v2) {
        return SampleVector<T>(kfr::convolve(v1.vector,v2.vector));
    }


    template<typename T>    
    SampleVector<T> random(size_t s) 
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distrib(0.0,1.0);
        SampleVector<T> r(s);    
        for(size_t i = 0; i < s; i++)
            r.vector[i] = distrib(generator);
        return r;
    }   

    template<typename T>    
    SampleVector<T> random(size_t s, T min, T max) 
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distrib(min,max);
        SampleVector<T> r(s);    
        for(size_t i = 0; i < s; i++)
            r.vector[i] = distrib(generator);
        return r;
    }  

    template<typename T>
    void plot_save(const SampleVector<T> & v, const std::string& name="", const std::string& options="") {
            kfr::plot_save(name,v.vector,options);
        }
    template<typename T>    
    void plot_show(const SampleVector<T> & v, const std::string& name="", const std::string&  options="") {
        kfr::plot_show(name,v.vector,options);
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

    template<typename T>
    void add_notch(BiQuadParams<T> &bqp, T f, T q) {
        kfr::biquad_params<T> p  = biquad_notch(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_lowpass(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_lowpass(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_highpass(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_highpass(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_peak(BiQuadParams<T> &bqp,T f, T q, T gain) {
        kfr::biquad_params<T> p  = biquad_peak(f,q, gain);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_lowshelf(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_lowshelf(f,q);
        bqp.bq.push_back(p);
    }
    template<typename T>    
    void add_highself(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_highshelf(f,q);
        bqp.bq.push_back(p);            
    }
    template<typename T>    
    void add_bandpass(BiQuadParams<T> &bqp,T f, T q) {
        kfr::biquad_params<T> p  = biquad_bandpass(f,q);
        bqp.bq.push_back(p);
    }

    template<typename T>
    kfr::biquad_params<T> notch_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_notch<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> lowpass_params(T f, T q) {
        kfr::biquad_params<T> params =  kfr::biquad_lowpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> highpass_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_highpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> peak_params(T f, T q, T gain) {
        kfr::biquad_params<T> params = kfr::biquad_peak<T>(cometa::identity<T>(f),cometa::identity<T>(q),cometa::identity<T>(gain));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> lowshelf_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_lowshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> highshelf_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_highshelf<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }
    template<typename T>
    kfr::biquad_params<T> bandpass_params(T f, T q) {
        kfr::biquad_params<T> params = kfr::biquad_bandpass<T>(cometa::identity<T>(f),cometa::identity<T>(q));
        return(params);
    }

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

        void apply(SampleVector<T> & vector) 
        {                 
            filter->apply(vector.vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
        {
            filter->apply(dst.vector,src.vector);
        }

        void reset() { filter->reset(); }
    };

    template<typename T>
    struct NotchFilter {
        Biquad<T> filter;

        NotchFilter( const BiQuadParams<T>& p, T f, T q) {        
            filter.notch(f,q);
        }

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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

        void apply(SampleVector<T> & vector) 
        {                 
            filter.apply(vector);
        }
        void apply(const SampleVector<T> & src, SampleVector<T> & dst) 
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
    SampleVector<T> biquad(const BiQuadParams<T> & bp, SampleVector<T> & input) {
        return SampleVector<T>(kfr::biquad(bp.bq,input.vector));
    }

    // simple biquad filter functions
    template<typename T>
    SampleVector<T> lowpassfilter(const SampleVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.lowpass(freq,q);
        SampleVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    SampleVector<T> highpassfilter(const SampleVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.highpass(freq,q);
        SampleVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    SampleVector<T> notchfilter(const SampleVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.notch(freq,q);
        SampleVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    SampleVector<T> bandpassfilter(const SampleVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.bandpass(freq,q);
        SampleVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    SampleVector<T> peakfilter(const SampleVector<T> & input, T freq, T q, T gain) {
        Biquad<T> filter;
        filter.peak(freq,q,gain);
        SampleVector<T> r(input);
        filter.apply(r,input);
        return r;
    }
    template<typename T>
    SampleVector<T> lowshelffilter(const SampleVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.lowshelf(freq,q);
        SampleVector<T> r(input);
        filter.apply(r,input);
        return r;    
    }
    template<typename T>
    SampleVector<T> highshelffilter(const SampleVector<T> & input, T freq, T q) {
        Biquad<T> filter;
        filter.highshelf(freq,q);
        SampleVector<T> r(input);
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
        size_t read(SampleVector<T> & vector) {
            return reader->read(vector.data(),vector.size());
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
        void write(SampleVector<T> & vector) {
            writer->write(vector.data(),vector.size());
        }
        const audio_format& format() {
            return writer->format();
        }

        size_t size() const { 
            audio_format& fmt = format();
            return fmt.length;
        }
    };

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
        size_t read(SampleVector<T> & vector) {
            return reader->read(vector.data(),vector.size());
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

        size_t size() const { 
            audio_format& fmt = format();
            return fmt.length;
        }
    };

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
        size_t read(SampleVector<T> & vector) {
            return reader->read(vector.data(),vector.size());
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

        size_t size() const { 
            audio_format& fmt = format();
            return fmt.length;
        }
    };

    template<typename T>
    SampleVector<T> load_wav(const char * filename) {
        WavReader<T> r(filename);        
        SampleVector<T> v(size());
        r.read(v);
        return v;
    }

    template<typename T>
    void write_wav(SampleVector<T> & v, const char * filename, size_t channels, int sample_type, double sample_rate, bool use_w64=false) {
        WavWriter<T> w(filename,kfr::audio_format{channels,static_cast<kfr::audio_sample_type>(sample_type),sample_rate,use_w64});
        w.write(v);        
    }

    template<typename T>
    SampleVector<T> load_mp3(const char * filename) {
        MP3Reader<T> r(filename);        
        SampleVector<T> v(size());
        r.read(v);
        return v;
    }

    template<typename T>
    SampleVector<T> load_flac(const char * filename) {
        FlacReader<T> r(filename);
        SampleVector<T> v(size());
        r.read(v);
        return v;
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
        ConvolveFilter(SampleVector<T> & u, size_t block_size=1024) {
            filter = new kfr::convolve_filter<T>(u.vector,block_size);
            assert(filter != nullptr);
        }
        ~ConvolveFilter() {
            if(filter) delete filter;
        }
        void set_data(const SampleVector<T> & u) { filter->set_data(u.vector); }
        void reset() { filter->reset(); }
        size_t input_block_size() const { return filter->input_block_size(); }

        void apply(SampleVector<T> & in) {
            filter->apply(in.vector);
        }
        void apply(const SampleVector<T> & in, SampleVector<T> & out) {
            filter->apply(out.vector,in.vector);
        }
    };  

    
    template<typename T>
    struct DCTPlan 
    {
    private:    
        kfr::dct_plan<T> plan;    
    public:
        DCTPlan(size_t size) : plan(size) {}
        
        void execute(SampleVector<T> & out, const SampleVector<T> & in, bool inverse=false) {
            SampleVector<kfr::u8> temp(in.size());
            plan.execute(out.vector.data(),in.vector.data(),temp.vector.data(),inverse);
        }
    };
    
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
        
        void execute(SampleVector<kfr::complex<T>> & out, const SampleVector<kfr::complex<T>> & in, bool inverse = false) {
            SampleVector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.vector.data(),in.vector.data(),temp.vector.data(),inverse);
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

        void execute_forward(SampleVector<kfr::complex<T>> & out, const SampleVector<T> & in) {
            SampleVector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.vector,in.vector,temp.vector);
        }
        void execute_reverse(SampleVector<T> & out, const SampleVector<kfr::complex<T>> & in) {
            SampleVector<kfr::u8> temp;
            temp.resize(in.size());
            out.resize(in.size());
            plan->execute(out.vector,in.vector,temp.vector);
        }
    };

    template<typename T>
    SampleVector<kfr::complex<T>> run_dft(const SampleVector<kfr::complex<T>> & data) { 
        return SampleVector<kfr::complex<T>>(kfr::dft(data.vector)); 
    }

    // sets imaginary to zero, return complex
    template<typename T>
    SampleVector<kfr::complex<T>> run_realdft(const SampleVector<T> & data) { 
        SampleVector<kfr::complex<T>> tmp;
        tmp.resize(data.size());    
        for(size_t i = 0; i < data.size(); i++)
        {
            tmp[i] = kfr::complex<T>(data[i],(T)0);
        }
        return SampleVector<kfr::complex<T>>(kfr::dft(tmp.vector)); 
    }

    template<typename T>
    SampleVector<kfr::complex<T>> run_idft(const SampleVector<kfr::complex<T>> & data) { return SampleVector<kfr::complex<T>>(kfr::idft(data.vector)); }

    template<typename T>
    SampleVector<T> run_irealdft(const SampleVector<kfr::complex<T>> & data) { return SampleVector<T>(kfr::irealdft(data.vector)); }

    template<typename T>
    SampleVector<T> resample(sample_rate_conversion_quality quality, const SampleVector<T> & input, size_t output_sr, size_t input_sr) {    
        auto r = kfr::resampler<T>(quality,output_sr, input_sr);    
        SampleVector<T> output(input.size() * output_sr/input_sr + r.get_delay());
        r.process(output.vector,input.vector);
        return output;    
    }

    template<typename T>
    SampleVector<T> convert_sample(const SampleVector<T> & input) {
        SampleVector<T> output(input.size());
        kfr::convert(output.vector.data(),input.vector.data(),input.size());
        return output;
    }

    template<typename T>
    SampleVector<T> amp_to_dB(const SampleVector<T> & in) {
        return SampleVector<T>(kfr::amp_to_dB(in.vector));
    }

    template<typename T>
    struct FIRFilter {
    private:
        kfr::filter_fir<SampleType> * filter;
        SampleVector<T> taps;
        
    public:
        
        FIRFilter(size_t num_taps) { 
            taps.resize(num_taps); 
            filter = nullptr;
        }
        ~FIRFilter() {
            if(filter != NULL) delete filter;
        }
        void bandpass(T x, T y, kfr::expression_pointer<T> & window, bool normalize=true ) {        
            kfr::fir_bandpass(taps.vector,x,y,window,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void bandstop(T x, T y, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_bandstop(taps.vector,x,y, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void highpass(T cutoff, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_highpass(taps.vector,cutoff, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        void lowpass(T cutoff, kfr::expression_pointer<T> & window_type, bool normalize=true ) {        
            kfr::fir_lowpass(taps.vector,cutoff, window_type,normalize);
            filter = new kfr::filter_fir<T>(taps);
        }
        
        void apply(SampleVector<T> & data) {
            filter->apply(data.vector);
        }
        void apply(SampleVector<T> & out, const SampleVector<T> & in) {
            filter->apply(out.vector,in.vector);
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
        void apply(SampleVector<T> & data) {
            filter->apply(data);
        }
        void apply(SampleVector<T> & out, const SampleVector<T> & in) {
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
        void apply(SampleVector<T> & data) {
            filter->apply(data);
        }
        void apply(SampleVector<T> & out, const SampleVector<T> & in) {
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
        void apply(SampleVector<T> & data) {
            filter->apply(data);
        }
        void apply(SampleVector<T> & out, const SampleVector<T> & in) {
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
        void apply(SampleVector<T> & data) {
            filter->apply(data);
        }
        void apply(SampleVector<T> & out, const SampleVector<T> & in) {
            filter->apply(out,in);
        }
        void reset() { filter->reset(); }
    };



    template<typename T>
    SampleVector<T> fir_lowpass(const SampleVector<T> & input, size_t num_taps, T cutoff, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.lowpass(cutoff, window_type, normalize);
        SampleVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    SampleVector<T> fir_highpass(const SampleVector<T> & input, size_t num_taps, T cutoff, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.highpass(cutoff, window_type, normalize);
        SampleVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    SampleVector<T> fir_bandpass(const SampleVector<T> & input, size_t num_taps, T x, T y, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.bandpass(x, y, window_type, normalize);
        SampleVector<T> out(input.size());
        f.apply(out,input);
        return out;
    }
    template<typename T>
    SampleVector<T> fir_bandstop(const SampleVector<T> & input, size_t num_taps, T x, T y, kfr::expression_pointer<T> & window_type, bool normalize = true) {
        FIRFilter<T> f(num_taps);
        f.bandstop(x, y, window_type, normalize);
        SampleVector<T> out(input.size());
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

        void apply(SampleVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const SampleVector<T> &in,SampleVector<T> & out)
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
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct BesselHighPassFilter {
        BesselFilter<T> filter;

        BesselHighPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct BesselBandPassFilter {
        BesselFilter<T> filter;

        BesselBandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct BesselBandStopFilter {
        BesselFilter<T> filter;

        BesselBandStopFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    SampleVector<T> bessel_lowpass(int order, T lo, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        BesselLowPassFilter<T> filter(order,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> bessel_highpass(int order, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        BesselHighPassFilter<T> filter(order,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> bessel_bandpass(int order, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        BesselBandPassFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> bessel_bandstop(int order, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
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

        void apply(SampleVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const SampleVector<T> &in, SampleVector<T> & out)
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
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct ButterworthHighPassFilter {
        ButterworthFilter<T> filter;

        ButterworthHighPassFilter(int order, T cutoff, T sample_rate) : filter(order) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct ButterworthBandPassFilter {
        ButterworthFilter<T> filter;

        ButterworthBandPassFilter(int order, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct ButterworthBandStopFilter {
        ButterworthFilter<T> filter;

        ButterworthBandStopFilter(int order, T lo_cutoff, T hi_cutoff ,T sample_rate) : filter(order) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    SampleVector<T> butterworth_lowpass(int order, T lo, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        ButterworthLowPassFilter<T> filter(order,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> butterworth_highpass(int order, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        ButterworthHighPassFilter<T> filter(order,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> butterworth_bandpass(int order, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        ButterworthBandPassFilter<T> filter(order,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> butterworth_bandstop(int order,  T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
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

        void apply(SampleVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const SampleVector<T> &in,SampleVector<T> & out)
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
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct Chebyshev1HighPassFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1HighPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev1BandPassFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1BandPassFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev1BandStopFilter {
        Chebyshev1Filter<T> filter;

        Chebyshev1BandStopFilter(int order, T rp, T lo_cutoff, T hi_cutoff ,T sample_rate) : filter(order,rp) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    SampleVector<T> chebyshev1_lowpass(int order, T rp, T lo, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev1LowPassFilter<T> filter(order,rp,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> chebyshev1_highpass(int order, T rp,T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev1HighPassFilter<T> filter(order,rp,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> chebyshev1_bandpass(int order, T rp, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev1BandPassFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> chebyshev1_bandstop(int order, T rp, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
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

        void apply(SampleVector<T> &in)
        {
            filter.apply(in);
        }
        void apply(const SampleVector<T> &in, SampleVector<T> & out)
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
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };

    template<typename T>
    struct Chebyshev2HighPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2HighPassFilter(int order, T rp, T cutoff, T sample_rate) : filter(order,rp) {
            filter.highpass(cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev2BandPassFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2BandPassFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandpass(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    struct Chebyshev2BandStopFilter {
        Chebyshev2Filter<T> filter;

        Chebyshev2BandStopFilter(int order, T rp, T lo_cutoff, T hi_cutoff, T sample_rate) : filter(order,rp) {
            filter.bandstop(lo_cutoff,hi_cutoff,sample_rate);
        }
        void apply(SampleVector<T> &in) { filter.apply(in);}
        void apply(const SampleVector<T> &in, SampleVector<T> &out) { filter.apply(in,out); }
    };
    
    template<typename T>
    SampleVector<T> chebyshev2_lowpass(int order, T rp, T lo, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev2LowPassFilter<T> filter(order,rp,lo,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> chebyshev2_highpass(int order, T rp,T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev2HighPassFilter<T> filter(order,rp,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> chebyshev2_bandpass(int order, T rp, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev2BandPassFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }
    template<typename T>
    SampleVector<T> chebyshev2_bandstop(int order, T rp, T lo, T hi, T sample_rate, const SampleVector<T> & in) {
        SampleVector<T> out(in.size());
        Chebyshev2BandStopFilter<T> filter(order,rp,lo,hi,sample_rate);
        filter.apply(in,out);
        return out;
    }

    template<typename T>
    SampleVector<T> dcremove(const SampleVector<T> & input, T cutoff) {
        return SampleVector<T>(kfr::CMT_ARCH_NAME::dcremove(input.vector,cutoff));
    }

    template<typename T>
    SampleVector<T> sinewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        SampleVector<T> r(n);                
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::sine(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }

    template<typename T>
    SampleVector<T> squarewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        SampleVector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::square(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }

    template<typename T>
    SampleVector<T> trianglewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        SampleVector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::triangle(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }
    
    template<typename T>
    SampleVector<T> sawtoothwave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        SampleVector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::sawtooth(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }

    template<typename T>
    SampleVector<T> generate_sin(size_t n, T start, T step) {
        SampleVector<T> r(n);
        r.vector = kfr::gen_sin(start,step);
        return r;
    }

    template<typename T>
    SampleVector<T> generate_linear(size_t n, T start, T step) {
        SampleVector<T> r(n);
        r.vector = kfr::gen_linear(start,step);
        return r;
    }

    template<typename T>
    SampleVector<T> generate_exp(size_t n, T start, T step) {
        SampleVector<T> r(n);
        r.vector = kfr::gen_exp(start,step);
        return r;
    }

    template<typename T>
    SampleVector<T> generate_exp2(size_t n, T start, T step) {
        SampleVector<T> r(n);
        r.vector = kfr::gen_exp2(start,step);
        return r;        
    }

    template<typename T>
    SampleVector<T> generate_cossin(size_t n, T start, T step) {
        SampleVector<T> r(n);
        r.vector = kfr::gen_cossin(start,step);
        return r;
    }

    template<typename T>
    void do_deinterleave(SampleMatrix<T> & out, const T * ptr, size_t size, size_t channels) {                
        out.resize(channels,size);
        std::vector<T*> p(out.size());
        for(size_t i = 0; i < out.size(); i++) p[i] = out[i].data();
        kfr::deinterleave(p.data(),ptr,channels,size);
    }
    
    template<typename T>
    SampleVector<T> do_interleave(const SampleMatrix<T> & input) {        
        return SampleVector<T>(kfr::interleave(input.vector));
    }

    template<typename T>
    SampleVector<T> generate_sequence(int n, int start, int step) {
        SampleVector<T> r(n);
        for(size_t i = 0; i < n; i++)
            r[i] = start + step*n;
        return r;
    }


    std::string dB_to_string(const double value, double min=-140.0f) { return kfr::dB_to_string(value,min); }
    std::string dB_to_utf8string(const double value, double min=-140.0f) { return kfr::dB_to_utf8string(value,min); }    
}