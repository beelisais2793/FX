#pragma once 

namespace SimpleEigen {
template<typename T>
struct Scalar : public Base
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


    RowVector<T> rowvector(const RowVector<T> & v);
    ColVector<T> colvector(const ColVector<T> & v);
    Matrix<T>    matrix(Matrix<T> & v);
    Array<T>     array(Array<T> & v);
    Array2D<T>     array2d(Array2D<T> & v);

    RowVector<T> rowvector(size_t size);
    ColVector<T> colvector(size_t size);
    Matrix<T>    matrix(size_t r, size_t c);
    Array<T>     array(size_t size);
    Array2D<T>   array2d(size_t r, size_t c);

    RowVector<T> operator * (const RowVector<T> & v);
    RowVector<T> operator / (const RowVector<T> & v);
    RowVector<T> operator + (const RowVector<T> & v);
    RowVector<T> operator - (const RowVector<T> & v);

    ColVector<T> operator * (const ColVector<T> & v);
    ColVector<T> operator / (const ColVector<T> & v);
    ColVector<T> operator + (const ColVector<T> & v);
    ColVector<T> operator - (const ColVector<T> & v);

    Matrix<T> operator * (Matrix<T> & v);
    Matrix<T> operator / (Matrix<T> & v);
    Matrix<T> operator + (Matrix<T> & v);
    Matrix<T> operator - (Matrix<T> & v);

    Array<T> operator * (Array<T> & v);
    Array<T> operator / (Array<T> & v);
    Array<T> operator + (Array<T> & v);
    Array<T> operator - (Array<T> & v);

    T    get_value() { return val; }
    void set_value(const T & v) { val = v; }
    
    Scalar<T> abs()  { 
            return Scalar<T>(std::abs((double)val));
     }    
    Scalar<T> fabs() { 
        return std::fabs((double)val);         
    }
    
    void print() { std::cout << val << std::endl; }  
};


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
}