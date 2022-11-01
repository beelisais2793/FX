#pragma once

#include "samples/sample.hpp"
#include "Vc/vectorclass.h"
#include "Vc/vectormath_lib.h"


struct Vector8f : public sample_vector<float>
{
    Vector8f() = default;
    Vector8f(size_t n) : sample_vector<float>(n) {

    }
    Vector8f& operator = (const Vector8f & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }
    Vector8f& operator = (const sample_vector<float> & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }    
    Vector8f operator -() {
        Vector8f x = *this;
        for(size_t i = 0; i < x.size(); i++) x[i] *= -1.0f;
        return x;
    }
};

Vector8f operator + (const Vector8f & v, const float r) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator + (const float r, const Vector8f & v) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator - (const Vector8f & v, const float r) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = x - y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator - (const float r, const Vector8f & v) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = y - x;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator * (const Vector8f & v, const float r) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator * (const float r, const Vector8f & v) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator / (const Vector8f & v, const float r) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = x / y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector8f operator / (const float r, const Vector8f & v) {
    Vec8f x,y(r,r,r,r,r,r,r,r),z;
    Vector8f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=8) {
        x.load_a(v.data()+i);
        z = y / x;;
        z.store_a(out.data()+i);
    }
    return out;
}

template<typename T>
Vector8f operator + (const Vector8f & a, const Vector8f & b) {
    Vec8f x,y,z;
    Vector8f r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=8)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x + y;
        z.store_a(r.data()+i);
    }
    return r;
}


template<typename T>
Vector8f operator - (const Vector8f & a, const Vector8f & b) {
    Vec8f x,y,z;
    Vector8f r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=8)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x - y;
        z.store_a(r.data()+i);
    }
    return r;
}

template<typename T>
Vector8f operator * (const Vector8f & a, const Vector8f & b) {
    Vec8f x,y,z;
    Vector8f r(a.size());
    assert(a.size() != b.size());
    for(size_t i = 0; i < a.size(); i+=8)
    {
        x.load_a(a.data()+i);
        y.load_a(b.data()+i);
        z = x * y;
        z.store_a(r.data()+i);
    }
    return r;
}

Vector8f fabs(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = abs(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f exp(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = exp(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f expm1(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = expm1(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f exp2(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = exp2(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f exp10(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = exp10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f pow(const Vector8f & v, float b) {
    Vector8f r(v.size());    
    Vec8f x,y(b,b,b,b,b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = pow(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f pow(float b, const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x,y(b,b,b,b,b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = pow(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void myfunc(float * data, int idx) {
    Vec8f r;
    r.load_a(data + idx);
    r = 1+2.0*exp(r);
    r.store_a(data + idx);
}
Vector8f cbrt(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = cbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector8f rcbrt(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = invcbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector8f sqrt(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = sqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector8f rsqrt(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = invsqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector8f log(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = log(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f log1p(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = log1p(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f log10(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = log10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f sin(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = sin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f cos(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = cos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f tan(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = tan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f asin(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = asin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f acos(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = acos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f atan(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = atan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f atan2(const Vector8f & v, float b) {
    Vector8f r(v.size());    
    Vec8f x,y(b,b,b,b,b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = atan2(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f atan2(float b, const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x,y(b,b,b,b,b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = atan2(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void sincos(const Vector8f& v, Vector8f & c, Vector8f & s) {
    c.resize(v.size());
    s.resize(v.size());
    Vec8f x,y;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        y.load_a(c.data()+i);
        x = sincos(&y,x);
        x.store_a(s.data()+i);
        y.store_a(c.data()+i);
    }
}
Vector8f sinh(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = sinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f cosh(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = cosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f tanh(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = tanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f asinh(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = asinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f acosh(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = acosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f atanh(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = atanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f erf(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = erf(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f erfc(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = erfc(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f erfinv(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = erfinv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f cdfnorm(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = cdfnorm(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector8f cdfnorminv(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = cdfnorminv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector8f pow2n(const Vector8f & v) {
    Vector8f r(v.size());    
    Vec8f x;
    for(size_t i = 0; i < v.size(); i+=8)
    {
        x.load_a(v.data()+i);
        x = pow2n(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
void inplace_kernel(Vector8f & v, std::function<void (float * data, int idx)>  f) {
    for(size_t i = 0; i < v.size(); i+=8) f(v.data(),i);
}