#pragma once

#include <vector>
#include <ccomplex>
#include <complex>
#include <limits>
#include <valarray>
#include <random>
#include <chrono>
#include <iostream>


namespace Std {
    template<class T>
    struct StdComplex : StdObject
    {
        std::complex<T> c;

        StdComplex() {}
        StdComplex(const T & r, const T & i) { c = std::complex<T>(r,i); }
        StdComplex(const std::complex<T> & q) { c = q; } 

        T imag() { return c.imag(); }
        T real() { return c.real(); }

        std::string name() const { return "Complex"; }
        
        StdComplex<T>& operator = (const StdComplex& q) { c = q.c; return *this; }
        StdComplex<T>& operator += (const StdComplex& q) { c += q.c; return *this; }
        StdComplex<T>& operator -= (const StdComplex& q) { c -= q.c; return *this; }
        StdComplex<T>& operator *= (const StdComplex& q) { c *= q.c; return *this; }
        StdComplex<T>& operator /= (const StdComplex& q) { c /= q.c; return *this; }

        StdComplex<T> operator + (const StdComplex& q) { return StdComplex<T>(c + q.c); }
        StdComplex<T> operator - (const StdComplex& q) { return StdComplex<T>(c - q.c); }
        StdComplex<T> operator * (const StdComplex& q) { return StdComplex<T>(c * q.c); }
        StdComplex<T> operator / (const StdComplex& q) { return StdComplex<T>(c / q.c); }

        bool operator == (const StdComplex& q) { return c == q.c;  }
        bool operator != (const StdComplex& q) { return c != q.c;  }

        T abs() { return std::abs(c); }
        T arg() { return std::arg(c); }
        T norm() { return std::norm(c); }
        StdComplex<T> conj() { return StdComplex<T>(std::conj(c)); }
        StdComplex<T> polar(const T& rho, const T &theta) { return StdComplex<T>(std::polar(rho,theta));}
        StdComplex<T> proj() { return StdComplex<T>(std::proj(c));}

        StdComplex<T> cos() { return StdComplex<T>(std::cos(c));}
        StdComplex<T> cosh() { return StdComplex<T>(std::cosh(c));}
        StdComplex<T> exp() { return StdComplex<T>(std::exp(c));}
        StdComplex<T> log() { return StdComplex<T>(std::log(c));}
        StdComplex<T> log10() { return StdComplex<T>(std::log10(c));}
        
        StdComplex<T> pow(const StdComplex<T> & p) { return StdComplex<T>(std::pow(c,p.c));}
        StdComplex<T> sin() { return StdComplex<T>(std::sin(c));}
        StdComplex<T> sinh() { return StdComplex<T>(std::sinh(c));}
        StdComplex<T> tan() { return StdComplex<T>(std::tan(c));}
        StdComplex<T> tanh() { return StdComplex<T>(std::tanh(c));}
        StdComplex<T> acos() { return StdComplex<T>(std::acos(c));}
        StdComplex<T> acosh() { return StdComplex<T>(std::acosh(c));}
        StdComplex<T> asin() { return StdComplex<T>(std::asin(c));}    
        StdComplex<T> asinh() { return StdComplex<T>(std::asinh(c));}    
        StdComplex<T> atan() { return StdComplex<T>(std::atan(c));}
        StdComplex<T> atanh() { return StdComplex<T>(std::atanh(c));}

    };

    template<class T>
    StdComplex<T> operator + (const StdComplex<T> & a, const StdComplex<T> & b)
    {
        StdComplex<T> r;
        r.c = a.c + b.c;
        return r;
    }
    template<class T>
    StdComplex<T> operator - (const StdComplex<T> & a, const StdComplex<T> & b)
    {
        StdComplex<T> r;
        r.c = a.c - b.c;
        return r;
    }
    template<class T>
    StdComplex<T> operator * (const StdComplex<T> & a, const StdComplex<T> & b)
    {
        StdComplex<T> r;
        r.c = a.c * b.c;
        return r;
    }
    template<class T>
    StdComplex<T> operator / (const StdComplex<T> & a, const StdComplex<T> & b)
    {
        StdComplex<T> r;
        r.c = a.c / b.c;
        return r;
    }
    /*
    StdComplex<float> complex(float complex c) {
        StdComplex<float> r(creal(c),cimag(c));
        return r;
    }
    StdComplex<double> complex(double complex c) {
        StdComplex<double> r(creal(c),cimag(c));
        return r;
    }
    float complex complex(StdComplex<float> & c) {
        float complex c = c.real() + c.imag()*I;
        return c;
    }
    double complex complex(StdComplex<double> & c) {
        double complex c = c.real() + c.imag()*I;
        return c;
    }
    */
    template<typename T>
    T abs(const StdComplex<T> & c) { return c.abs(); }
    template<typename T>
    T arg(const StdComplex<T> & c) { return c.arg(); }
    template<typename T>
    T norm(const StdComplex<T> & c) { return c.norm(); }
    template<typename T>
    StdComplex<T> conj(const StdComplex<T> & c) { return c.conj(); }
    template<typename T>
    StdComplex<T> polar(const StdComplex<T> & c,const T& rho, const T &theta) { return c.polar(rho,theta);}
    template<typename T>
    StdComplex<T> proj(const StdComplex<T> & c) { return c.proj();}
    template<typename T>
    StdComplex<T> cos(const StdComplex<T> & c) { return c.cos();}
    template<typename T>
    StdComplex<T> cosh(const StdComplex<T> & c) { return c.cosh();}
    template<typename T>
    StdComplex<T> exp(const StdComplex<T> & c) { return c.exp();}
    template<typename T>
    StdComplex<T> log(const StdComplex<T> & c) { return c.log();}
    template<typename T>
    StdComplex<T> log10(const StdComplex<T> & c) { return c.log10();}
    template<typename T>
    StdComplex<T> pow(const StdComplex<T> & c,const StdComplex<T> & p) { return c.pow(p.c);}
    template<typename T>
    StdComplex<T> sin(const StdComplex<T> & c) { return c.sin();}
    template<typename T>
    StdComplex<T> sinh(const StdComplex<T> & c) { return c.sinh();}
    template<typename T>
    StdComplex<T> tan(const StdComplex<T> & c) { return c.tan();}
    template<typename T>
    StdComplex<T> tanh(const StdComplex<T> & c) { return c.tanh();}
    template<typename T>
    StdComplex<T> acos(const StdComplex<T> & c) { return c.acos();}
    template<typename T>
    StdComplex<T> acosh(const StdComplex<T> & c) { return c.acosh();}
    template<typename T>
    StdComplex<T> asin(const StdComplex<T> & c) { return c.asin();}    
    template<typename T>
    StdComplex<T> asinh(const StdComplex<T> & c) { return c.asinh();}    
    template<typename T>
    StdComplex<T> atan(const StdComplex<T> & c) { return c.atan();}
    template<typename T>
    StdComplex<T> atanh(const StdComplex<T> & c) { return c.atanh();}   
}