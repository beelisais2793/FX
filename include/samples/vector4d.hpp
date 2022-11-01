#pragma once

#include "samples/sample.hpp"
#include "Vc/vectorclass.h"
#include "Vc/vectormath_lib.h"


struct Vector4d : public sample_vector<double>
{
    Vector4d() = default;
    Vector4d(size_t n) : sample_vector<double>(n) {

    }
    Vector4d& operator = (const Vector4d & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }
    Vector4d& operator = (const sample_vector<double> & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }    
    Vector4d operator -() {
        Vector4d x = *this;
        for(size_t i = 0; i < x.size(); i++) x[i] *= -1.0f;
        return x;
    }
};

Vector4d operator + (const Vector4d & v, const double r) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator + (const double r, const Vector4d & v) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator - (const Vector4d & v, const double r) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x - y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator - (const double r, const Vector4d & v) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = y - x;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator * (const Vector4d & v, const double r) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator * (const double r, const Vector4d & v) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator / (const Vector4d & v, const double r) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x / y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4d operator / (const double r, const Vector4d & v) {
    Vec4d x,y(r,r,r,r),z;
    Vector4d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = y / x;;
        z.store_a(out.data()+i);
    }
    return out;
}

template<typename T>
Vector4d operator + (const Vector4d & a, const Vector4d & b) {
    Vec4d x,y,z;
    Vector4d r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=4)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x + y;
        z.store_a(r.data()+i);
    }
    return r;
}


template<typename T>
Vector4d operator - (const Vector4d & a, const Vector4d & b) {
    Vec4d x,y,z;
    Vector4d r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=4)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x - y;
        z.store_a(r.data()+i);
    }
    return r;
}

template<typename T>
Vector4d operator * (const Vector4d & a, const Vector4d & b) {
    Vec4d x,y,z;
    Vector4d r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=4)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x * y;
        z.store_a(r.data()+i);
    }
    return r;
}

Vector4d fabs(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = abs(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d exp(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = exp(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d expm1(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = expm1(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d exp2(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = exp2(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d exp10(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = exp10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d pow(const Vector4d & v, double b) {
    Vector4d r(v.size());    
    Vec4d x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = pow(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d pow(double b, const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = pow(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void myfunc(double * data, int idx) {
    Vec4d r;
    r.load_a(data + idx);
    r = 1+2.0*exp(r);
    r.store_a(data + idx);
}
Vector4d cbrt(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector4d rcbrt(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = invcbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector4d sqrt(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = sqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector4d rsqrt(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = invsqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector4d log(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = log(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d log1p(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = log1p(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d log10(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = log10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d sin(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = sin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d cos(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d tan(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = tan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d asin(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = asin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d acos(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = acos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d atan(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d atan2(const Vector4d & v, double b) {
    Vector4d r(v.size());    
    Vec4d x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atan2(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d atan2(double b, const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atan2(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void sincos(const Vector4d& v, Vector4d & c, Vector4d & s) {
    c.resize(v.size());
    s.resize(v.size());
    Vec4d x,y;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        y.load_a(c.data()+i);
        x = sincos(&y,x);
        x.store_a(s.data()+i);
        y.store_a(c.data()+i);
    }
}
Vector4d sinh(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = sinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d cosh(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d tanh(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = tanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d asinh(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = asinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d acosh(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = acosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d atanh(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d erf(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = erf(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d erfc(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = erfc(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d erfinv(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = erfinv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d cdfnorm(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cdfnorm(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4d cdfnorminv(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cdfnorminv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector4d pow2n(const Vector4d & v) {
    Vector4d r(v.size());    
    Vec4d x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = pow2n(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
void inplace_kernel(Vector4d & v, std::function<void (double * data, int idx)>  f) {
    for(size_t i = 0; i < v.size(); i+=4) f(v.data(),i);
}