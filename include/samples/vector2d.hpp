#pragma once

#include "samples/sample.hpp"
#include "Vc/vectorclass.h"
#include "Vc/vectormath_lib.h"


struct Vector2d : public sample_vector<double>
{
    Vector2d() = default;
    Vector2d(size_t n) : sample_vector<double>(n) {

    }
    Vector2d& operator = (const Vector2d & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }
    Vector2d& operator = (const sample_vector<double> & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }    
    Vector2d operator -() {
        Vector2d x = *this;
        for(size_t i = 0; i < x.size(); i++) x[i] *= -1.0f;
        return x;
    }
};

Vector2d operator + (const Vector2d & v, const double r) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator + (const double r, const Vector2d & v) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator - (const Vector2d & v, const double r) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = x - y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator - (const double r, const Vector2d & v) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = y - x;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator * (const Vector2d & v, const double r) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator * (const double r, const Vector2d & v) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator / (const Vector2d & v, const double r) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = x / y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector2d operator / (const double r, const Vector2d & v) {
    Vec2d x,y(r,r),z;
    Vector2d out(v.size());    
    for(size_t i = 0; i < v.size(); i+=2) {
        x.load_a(v.data()+i);
        z = y / x;;
        z.store_a(out.data()+i);
    }
    return out;
}

template<typename T>
Vector2d operator + (const Vector2d & a, const Vector2d & b) {
    Vec2d x,y,z;
    Vector2d r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=2)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x + y;
        z.store_a(r.data()+i);
    }
    return r;
}


template<typename T>
Vector2d operator - (const Vector2d & a, const Vector2d & b) {
    Vec2d x,y,z;
    Vector2d r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=2)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x - y;
        z.store_a(r.data()+i);
    }
    return r;
}

template<typename T>
Vector2d operator * (const Vector2d & a, const Vector2d & b) {
    Vec2d x,y,z;
    Vector2d r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=2)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x * y;
        z.store_a(r.data()+i);
    }
    return r;
}

Vector2d fabs(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = abs(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d exp(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = exp(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d expm1(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = expm1(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d exp2(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = exp2(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d exp10(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = exp10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d pow(const Vector2d & v, double b) {
    Vector2d r(v.size());    
    Vec2d x,y(b,b);
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = pow(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d pow(double b, const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x,y(b,b);
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = pow(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void myfunc(double * data, int idx) {
    Vec2d r;
    r.load_a(data + idx);
    r = 1+2.0*exp(r);
    r.store_a(data + idx);
}
Vector2d cbrt(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = cbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector2d rcbrt(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = invcbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector2d sqrt(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = sqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector2d rsqrt(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = invsqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector2d log(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = log(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d log1p(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = log1p(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d log10(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = log10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d sin(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = sin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d cos(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = cos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d tan(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = tan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d asin(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = asin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d acos(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = acos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d atan(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = atan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d atan2(const Vector2d & v, double b) {
    Vector2d r(v.size());    
    Vec2d x,y(b,b);
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = atan2(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d atan2(double b, const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x,y(b,b);
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = atan2(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void sincos(const Vector2d& v, Vector2d & c, Vector2d & s) {
    c.resize(v.size());
    s.resize(v.size());
    Vec2d x,y;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        y.load_a(c.data()+i);
        x = sincos(&y,x);
        x.store_a(s.data()+i);
        y.store_a(c.data()+i);
    }
}
Vector2d sinh(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = sinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d cosh(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = cosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d tanh(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = tanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d asinh(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = asinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d acosh(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = acosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d atanh(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = atanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d erf(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = erf(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d erfc(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = erfc(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d erfinv(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = erfinv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d cdfnorm(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = cdfnorm(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector2d cdfnorminv(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = cdfnorminv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector2d pow2n(const Vector2d & v) {
    Vector2d r(v.size());    
    Vec2d x;
    for(size_t i = 0; i < v.size(); i+=2)
    {
        x.load_a(v.data()+i);
        x = pow2n(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
void inplace_kernel(Vector2d & v, std::function<void (double * data, int idx)>  f) {
    for(size_t i = 0; i < v.size(); i+=2) f(v.data(),i);
}