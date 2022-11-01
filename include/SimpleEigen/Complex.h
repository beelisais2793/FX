#pragma once 

namespace SimpleEigen {
template<typename T>
struct Complex : public Base
{
    std::complex<T> c;

    Complex() {}
    Complex(T r, T i) : c(r,i) {} 
    Complex(const Complex<T> & cpx) : c(cpx.c) {}
    Complex(const std::complex<T> & cpx) : c(cpx) {} 

    T r() { return c.real(); }
    T i() { return c.imag(); }
    T real() { return c.real(); }
    T imag() { return c.imag(); }
    void set_real(T r) { c.real(r); }
    void set_imag(T i) { c.imag(i); }

    Complex<T>& operator = (const Complex<T> & cpx) { c = cpx.c; return *this; }

    Complex<T> operator + () { Complex<T> r; r.c=(+c); return r;}
    Complex<T> operator - () { Complex<T> r; r.c=(-c); return r;}

    Complex<T> operator + (const Complex<T> & cpx) { Complex<T> r; r.c=(c + cpx.c); return r;}
    Complex<T> operator - (const Complex<T> & cpx) { Complex<T> r; r.c=(c - cpx.c); return r;}
    Complex<T> operator * (const Complex<T> & cpx) { Complex<T> r; r.c=(c * cpx.c); return r;}
    Complex<T> operator / (const Complex<T> & cpx) { Complex<T> r; r.c=(c / cpx.c); return r;}

    Complex<T> abs() { return Complex<T>(std::abs(c)); }
    Complex<T> arg() { return Complex<T>(std::arg(c)); }
    Complex<T> norm() { return Complex<T>(std::norm(c)); }
    Complex<T> conj() { return Complex<T>(std::conj(c)); }
    Complex<T> proj() { return Complex<T>(std::proj(c)); }
    Complex<T> polar(const T& t, const T& theta=T()) { return Complex<T>(std::polar(t,theta)); }
    Complex<T> exp() { return Complex<T>(std::exp(c)); }
    Complex<T> log() { return Complex<T>(std::log(c)); }
    Complex<T> log10() { return Complex<T>(std::abs(c)); }
    Complex<T> pow(const Complex<T> & e) { return Complex<T>(std::pow(c,e.c)); }
    Complex<T> pow(const T & e) { return Complex<T>(std::pow(c,e)); }
    Complex<T> sqrt() { return Complex<T>(std::sqrt(c)); }
    Complex<T> sin() { return Complex<T>(std::sin(c)); }
    Complex<T> cos() { return Complex<T>(std::cos(c)); }
    Complex<T> tan() { return Complex<T>(std::tan(c)); }
    Complex<T> asin() { return Complex<T>(std::asin(c)); }
    Complex<T> acos() { return Complex<T>(std::acos(c)); }
    Complex<T> atan() { return Complex<T>(std::atan(c)); }
    Complex<T> sinh() { return Complex<T>(std::sinh(c)); }
    Complex<T> cosh() { return Complex<T>(std::cosh(c)); }
    Complex<T> tanh() { return Complex<T>(std::tanh(c)); }
    Complex<T> asinh() { return Complex<T>(std::asinh(c)); }
    Complex<T> acosh() { return Complex<T>(std::acosh(c)); }
    Complex<T> atanh() { return Complex<T>(std::atanh(c)); }    
};
}