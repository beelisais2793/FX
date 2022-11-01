#pragma once 
#include <cmath>

namespace Std {
    template<typename T>
    struct StdScalar 
    {
        T val;

        StdScalar(const T& v) { val = v; }
        StdScalar(const StdScalar<T> & s) { val = s.val; }

        StdScalar<T>& operator = (const StdScalar<T> & s)
        {
            val = s.val;
            return *this;
        }
        StdScalar<T>& operator = (const T & s)
        {
            val = s;
            return *this;
        }
        
        StdScalar<T> operator + (const StdScalar<T> & s)
        {
            return StdScalar<T>(val + s.val);
        }
        StdScalar<T> operator - (const StdScalar<T> & s)
        {
            return StdScalar<T>(val - s.val);
        }
        StdScalar<T> operator * (const StdScalar<T> & s)
        {
            return StdScalar<T>(val * s.val);
        }
        StdScalar<T> operator / (const StdScalar<T> & s)
        {
            return StdScalar<T>(val / s.val);
        }

        StdScalar<T> operator + (const T & s)
        {
            return StdScalar<T>(val + s);
        }
        StdScalar<T> operator - (const T & s)
        {
            return StdScalar<T>(val - s);
        }
        StdScalar<T> operator * (const T & s)
        {
            return StdScalar<T>(val * s);
        }
        StdScalar<T> operator / (const T & s)
        {
            return StdScalar<T>(val / s);
        }

        void aplus(const T & v) { val += v; }
        void aminus(const T & v) { val -= v; }
        void atimes(const T & v) { val *= v; }
        void adiv(const T & v) { val /= v; }

        void aplus(const StdScalar<T> & v) { val += v.val; }
        void aminus(const StdScalar<T> & v) { val -= v.val; }
        void atimes(const StdScalar<T> & v) { val *= v.val; }
        void adiv(const StdScalar<T> & v) { val /= v.val; }

        T    get_value() { return val; }
        void set_value(const T & v) { val = v; }
        
        
        void print() { std::cout << val << std::endl; }  
    };

    template<typename T>
    bool operator > (const StdScalar<T> & a, const StdScalar<T> & b) {
        return a.val > b.val;
    }

    template<typename T>
    StdScalar<T> make_scalar(T val) {
        return StdScalar<T>(val);
    }

    template<typename T>
    StdScalar<T> operator + (const StdScalar<T> & a, const StdScalar<T> & b)
    {
        return a+b;
    }
    template<typename T>
    StdScalar<T> operator - (const StdScalar<T> & a, const StdScalar<T> & b)
    {
        return a-b;
    }
    template<typename T>
    StdScalar<T> operator * (const StdScalar<T> & a, const StdScalar<T> & b)
    {
        return a*b;
    }
    template<typename T>
    StdScalar<T> operator / (const StdScalar<T> & a, const StdScalar<T> & b)
    {
        return a/b;
    }
        

    template<typename T> StdScalar<T> abs(const StdScalar<T> & s)  { 
                return StdScalar<T>(std::abs((double)s.val));
        }    
    template<typename T> StdScalar<T> fabs(const StdScalar<T> & s) { 
            return StdScalar<T>(std::fabs((double)s.val));         
        }

    template<typename T>
    StdScalar<T> acos(const StdScalar<T> & scalar) { return std::acos(scalar.val); }
    template<typename T>
    StdScalar<T> acosh(const StdScalar<T> & scalar) { return std::acosh(scalar.val); }
    template<typename T>
    StdScalar<T> asinh(const StdScalar<T> & scalar) { return std::asinh(scalar.val); }
    template<typename T>
    StdScalar<T> atan(const StdScalar<T> & scalar) { return std::atan(scalar.val); }
    template<typename T>
    StdScalar<T> atan2(const StdScalar<T> & scalar,const StdScalar<T>& y) { return std::atan2(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> atanh(const StdScalar<T> & scalar) { return std::atanh(scalar.val); }
    template<typename T>
    StdScalar<T> cbrt(const StdScalar<T> & scalar) { return std::cbrt(scalar.val); }
    template<typename T>
    StdScalar<T> ceil(const StdScalar<T> & scalar) { return std::ceil(scalar.val); }    
    template<typename T>
    StdScalar<T> copysign(const StdScalar<T> & scalar,const StdScalar<T>& y) { return std::copysign(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> cos(const StdScalar<T> & scalar) { return std::cos(scalar.val); }
    template<typename T>
    StdScalar<T> cosh(const StdScalar<T> & scalar) { return std::cosh(scalar.val); }
    template<typename T>
    StdScalar<T> erf(const StdScalar<T> & scalar) { return std::erf(scalar.val); }
    template<typename T>
    StdScalar<T> erfc(const StdScalar<T> & scalar) { return std::erfc(scalar.val); }
    template<typename T>
    StdScalar<T> exp(const StdScalar<T> & scalar) { return std::exp(scalar.val); }
    template<typename T>
    StdScalar<T> exp2(const StdScalar<T> & scalar) { return std::exp2(scalar.val); }
    template<typename T>
    StdScalar<T> expm1(const StdScalar<T> & scalar) { return std::expm1(scalar.val); }
    template<typename T>
    StdScalar<T> fdim(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::fdim(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> floor(const StdScalar<T> & scalar) { return std::floor(scalar.val); }
    template<typename T>
    StdScalar<T> fma(const StdScalar<T> & scalar,const StdScalar<T> & y, const StdScalar<T>& z) { return std::fma(scalar.val,y.val,z.val); }
    template<typename T>
    StdScalar<T> fmax(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::fmax(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> fmin(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::fmin(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> fmod(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::fmod(scalar.val,y.val); }
    template<typename T>
    int fpclassify(const StdScalar<T> & scalar) { return std::fpclassify(scalar.val); }
    template<typename T>
    StdScalar<T> hypot(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::hypot(scalar.val,y.val); }
    template<typename T>
    int ilogb(const StdScalar<T> & scalar) { return std::ilogb(scalar.val); }
    template<typename T>
    bool isfinite(const StdScalar<T> & scalar) { return std::isfinite(scalar.val); }
    template<typename T>
    bool isgreater(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::isgreater(scalar.val,y.val); }
    template<typename T>
    bool isgreaterequal(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::isgreaterequal(scalar.val,y.val); }
    template<typename T>
    bool isinf(const StdScalar<T> & scalar) { return std::isinf(scalar.val); }
    template<typename T>
    bool isless(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::isless(scalar.val,y.val); }
    template<typename T>
    bool islessequal(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::islessequal(scalar.val,y.val); }
    template<typename T>
    bool islessgreater(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::islessgreater(scalar.val,y.val); }
    template<typename T>
    bool isnan(const StdScalar<T> & scalar) { return std::isnan(scalar.val); }
    template<typename T>
    bool isnormal(const StdScalar<T> & scalar) { return std::isnormal(scalar.val); }
    template<typename T>
    bool isunordered(const StdScalar<T> & scalar,const StdScalar<T>& y) { return std::isunordered(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> ldexp( const StdScalar<T> & scalar,int exp) { return std::ldexp(scalar.val,exp); }
    template<typename T>
    StdScalar<T> lgamma(const StdScalar<T> & scalar) { return std::lgamma(scalar.val); }
    template<typename T>
    StdScalar<T> llrint(const StdScalar<T> & scalar) { return std::llrint(scalar.val); }
    template<typename T>
    StdScalar<T> llround(const StdScalar<T> & scalar) { return std::llround(scalar.val); }
    template<typename T>
    StdScalar<T> log(const StdScalar<T> & scalar) { return std::log(scalar.val); }
    template<typename T>
    StdScalar<T> log10(const StdScalar<T> & scalar) { return std::log10(scalar.val); }
    template<typename T>
    StdScalar<T> log1p(const StdScalar<T> & scalar) { return std::log1p(scalar.val); }
    template<typename T>
    StdScalar<T> log2(const StdScalar<T> & scalar) { return std::log2(scalar.val); }
    template<typename T>
    StdScalar<T> logb(const StdScalar<T> & scalar) { return std::logb(scalar.val); }
    template<typename T>
    StdScalar<T> lrint(const StdScalar<T> & scalar) { return std::lrint(scalar.val); }
    template<typename T>
    StdScalar<T> lround(const StdScalar<T> & scalar) { return std::lround(scalar.val); }
    template<typename T>
    StdScalar<T> nan(const char *tagp) { return std::nan(tagp);}
    template<typename T>
    StdScalar<T> nanf(const char *tagp) { return std::nanf(tagp);}
    template<typename T>
    StdScalar<T> nanl(const char *tagp) { return std::nanl(tagp);}
    template<typename T>
    StdScalar<T> nearbyint(const StdScalar<T> & scalar) { return std::nearbyint(scalar.val); }
    template<typename T>
    StdScalar<T> nextafter(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::nextafter(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> nexttoward(const StdScalar<T> & scalar,const StdScalar<T> & y) { return std::nexttoward(scalar.val,y.val); }
    template<typename T>
    StdScalar<T> pow(const StdScalar<T> & scalar,const StdScalar<T> & e) { return std::pow(scalar.val,e.val); }
    template<typename T>
    StdScalar<T> remainder(const StdScalar<T> & scalar,const StdScalar<T> & d) { return std::remainder(scalar.val,d.val); }
    template<typename T>
    StdScalar<T> rint(const StdScalar<T> & scalar) { return std::rint(scalar.val); }
    template<typename T>
    StdScalar<T> round(const StdScalar<T> & scalar) { return std::round(scalar.val); }
    template<typename T>
    StdScalar<T> scalbln(const StdScalar<T> & scalar,long int n) { return std::scalbln(scalar.val,n);}
    template<typename T>
    StdScalar<T> scalbn(const StdScalar<T> & scalar,int n) { return std::scalbln(scalar.val,n);}
    template<typename T>
    StdScalar<T> sin(const StdScalar<T> & scalar) { return std::sin(scalar.val); }
    template<typename T>
    StdScalar<T> sinh(const StdScalar<T> & scalar) { return std::sinh(scalar.val); }    
    template<typename T>
    StdScalar<T> sqrt(const StdScalar<T> & scalar) { return std::sqrt(scalar.val); }
    template<typename T>
    StdScalar<T> square(const StdScalar<T> & scalar) { return scalar.val*scalar.val; }
    template<typename T>
    StdScalar<T> cube(const StdScalar<T> & scalar) { return scalar.val*scalar.val*scalar.val; }
    template<typename T>
    StdScalar<T> tan(const StdScalar<T> & scalar) { return std::tan(scalar.val); }
    template<typename T>
    StdScalar<T> tanh(const StdScalar<T> & scalar) { return std::tanh(scalar.val); }        
    template<typename T>
    StdScalar<T> tgamma(const StdScalar<T> & scalar) { return std::tgamma(scalar.val); }    
    template<typename T>
    StdScalar<T> trunc(const StdScalar<T> & scalar) { return std::trunc(scalar.val); }
}