#pragma once
#include "samples/sample.hpp"
#include "Vc/vectorclass.h"
#include "Vc/vectormath_lib.h"


struct Vector4f : public sample_vector<float>
{
    Vector4f() = default;
    Vector4f(size_t n) : sample_vector<float>(n) {

    }
    Vector4f& operator = (const Vector4f & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }
    Vector4f& operator = (const sample_vector<float> & x) {
        std::copy(x.begin(),x.end(),this->begin());
        return *this;
    }    
    Vector4f operator -() {
        Vector4f x = *this;
        for(size_t i = 0; i < x.size(); i++) x[i] *= -1.0f;
        return x;
    }
};

Vector4f operator + (const Vector4f & v, const float r) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator + (const float r, const Vector4f & v) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x + y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator - (const Vector4f & v, const float r) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x - y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator - (const float r, const Vector4f & v) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = y - x;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator * (const Vector4f & v, const float r) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator * (const float r, const Vector4f & v) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x * y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator / (const Vector4f & v, const float r) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = x / y;
        z.store_a(out.data()+i);
    }
    return out;
}
Vector4f operator / (const float r, const Vector4f & v) {
    Vec4f x,y(r,r,r,r),z;
    Vector4f out(v.size());    
    for(size_t i = 0; i < v.size(); i+=4) {
        x.load_a(v.data()+i);
        z = y / x;;
        z.store_a(out.data()+i);
    }
    return out;
}

template<typename T>
Vector4f operator + (const Vector4f & a, const Vector4f & b) {
    Vec4f x,y,z;
    Vector4f r(a.size());
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
Vector4f operator - (const Vector4f & a, const Vector4f & b) {
    Vec4f x,y,z;
    Vector4f r(a.size());
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
Vector4f operator * (const Vector4f & a, const Vector4f & b) {
    Vec4f x,y,z;
    Vector4f r(a.size());
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

Vector4f fabs(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = abs(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f exp(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = exp(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f expm1(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = expm1(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f exp2(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = exp2(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f exp10(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = exp10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f pow(const Vector4f & v, float b) {
    Vector4f r(v.size());    
    Vec4f x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = pow(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f pow(float b, const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = pow(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void myfunc(float * data, int idx) {
    Vec4f r;
    r.load_a(data + idx);
    r = 1+2.0*exp(r);
    r.store_a(data + idx);
}
Vector4f cbrt(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector4f rcbrt(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = invcbrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector4f sqrt(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = sqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector4f rsqrt(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = invsqrt(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
Vector4f log(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = log(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f log1p(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = log1p(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f log10(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = log10(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f sin(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = sin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f cos(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f tan(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = tan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f asin(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = asin(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f acos(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = acos(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f atan(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atan(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f atan2(const Vector4f & v, float b) {
    Vector4f r(v.size());    
    Vec4f x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atan2(x,b);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f atan2(float b, const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x,y(b,b,b,b);
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atan2(b,x);
        x.store_a(r.data()+i);
    }
    return r;
}
void sincos(const Vector4f& v, Vector4f & c, Vector4f & s) {
    c.resize(v.size());
    s.resize(v.size());
    Vec4f x,y;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        y.load_a(c.data()+i);
        x = sincos(&y,x);
        x.store_a(s.data()+i);
        y.store_a(c.data()+i);
    }
}
Vector4f sinh(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = sinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f cosh(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f tanh(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = tanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f asinh(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = asinh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f acosh(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = acosh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f atanh(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = atanh(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f erf(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = erf(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f erfc(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = erfc(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f erfinv(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = erfinv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f cdfnorm(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cdfnorm(x);
        x.store_a(r.data()+i);
    }
    return r;
}
Vector4f cdfnorminv(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = cdfnorminv(x);
        x.store_a(r.data()+i);
    }
    return r;
}
/*
Vector4f pow2n(const Vector4f & v) {
    Vector4f r(v.size());    
    Vec4f x;
    for(size_t i = 0; i < v.size(); i+=4)
    {
        x.load_a(v.data()+i);
        x = pow2n(x);
        x.store_a(r.data()+i);
    }
    return r;
}
*/
void inplace_kernel(Vector4f & v, std::function<void (float * data, int idx)>  f) {
    for(size_t i = 0; i < v.size(); i+=4) f(v.data(),i);
}