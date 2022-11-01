#pragma once 

#include <vector>
#include <complex>
#include <limits>
#include <valarray>
#include <random>
#include <chrono>
#include <iostream>


namespace Std {

    template<class T>
    struct ValArray;

    template<class T>
    struct Scalar : public StdObject
    {
        T val;

        Scalar(const T& v) { val = v; }
        Scalar(const Scalar & s) { val = s.val;}

        Scalar<T>& operator = (const T & v) { val = v; return *this; }
        
        Scalar<T> operator + (const T & v) { return Scalar(val + v); }
        Scalar<T> operator - (const T & v) { return Scalar(val- v);  }
        Scalar<T> operator * (const T & v) { return Scalar(val * v);  }
        Scalar<T> operator / (const T & v) { return Scalar(val / v);  }    


        Scalar<T> operator + (const Scalar<T> & v) { return Scalar(val + v.val); }
        Scalar<T> operator - (const Scalar<T> & v) { return Scalar(val- v.val);  }
        Scalar<T> operator * (const Scalar<T> & v) { return Scalar(val * v.val);  }
        Scalar<T> operator / (const Scalar<T> & v) { return Scalar(val / v.val);  }    

        ValArray<T> operator + (const ValArray<T> & v);
        ValArray<T> operator - (const ValArray<T> & v);
        ValArray<T> operator * (const ValArray<T> & v);
        ValArray<T> operator / (const ValArray<T> & v);    


        operator T() { return val; }
    };

    // you can use this like numpy, but it doesn't have any useful optimization with gcc
    template<class T>
    struct ValArray : public StdObject
    {
        std::valarray<T> a;

        ValArray() {}
        ValArray(size_t n) { a = std::valarray<T>(n); }
        ValArray(const T & val, size_t n) { a = std::valarray<T>(val,n); }
        ValArray(const std::valarray<T> & x) { a = x; }
        ValArray(const std::slice_array<T> & x) { a = std::valarray<T>(x); }
        ValArray(const std::gslice_array<T> & x) { a = std::valarray<T>(x); }
        /*
        ValArray(const Array<T> & x)
        {
            a = std::valarray<T>(x.v.data(), x.v.size());
        }
        */
        ValArray<T>& operator = (const ValArray<T> & x)
        {
            a = x.a;
            return *this;
        }
        ValArray<T>& operator = (const T& val)
        {
            a = val;
            return *this;
        }
        T& operator[](size_t pos) { return a[pos]; }
        T __getitem(size_t pos) { return a[pos]; }
        void __setitem(size_t pos, const T & val) { a[pos] =val; }

        ValArray<T> operator + ()  
        {
            ValArray<T> r;
            r.a = +a;
            return r;
        }
        ValArray<T> operator - ()  
        {
            ValArray<T> r;
            r.a = -a;
            return r;
        }    
        ValArray<T> operator ~ ()  
        {
            ValArray<T> r;
            r.a = ~a;
            return r;
        }
        ValArray<bool> operator ! ()
        {
            ValArray<T> r;
            r.a = !a;
            return r;
        }

        ValArray<T>& operator += ( const ValArray<T> & x)
        {
            a += x.a;
            return *this;
        }
        ValArray<T>& operator -= ( const ValArray<T> & x)
        {
            a -= x.a;
            return *this;
        }
        ValArray<T>& operator *= ( const ValArray<T> & x)
        {
            a *= x.a;
            return *this;
        }
        ValArray<T>& operator /= ( const ValArray<T> & x)
        {
            a /= x.a;
            return *this;
        }
        ValArray<T>& operator %= ( const ValArray<T> & x)
        {
            a %= x.a;
            return *this;
        }

        ValArray<T>& operator += ( const Scalar<T> & x)
        {
            a += x.val;
            return *this;
        }
        ValArray<T>& operator -= ( const Scalar<T> & x)
        {
            a -= x.val;
            return *this;
        }
        ValArray<T>& operator *= ( const Scalar<T> & x)
        {
            a *= x.val;
            return *this;
        }
        ValArray<T>& operator /= ( const Scalar<T> & x)
        {
            a /= x.val;
            return *this;
        }
        ValArray<T>& operator %= ( const Scalar<T> & x)
        {
            a %= x.val;
            return *this;
        }
        
        void swap( ValArray<T> & x) noexcept { a.swap(x.a); }
        size_t size() { return a.size(); }
        void resize(size_t s) { a.resize(s); }
        T sum() { return a.sum(); }
        T min() { return a.min(); }
        T max() { return a.max(); }
        //ValArray<T>& shift(int count) { return ValArray<T>(a.shift(count)); }
        //ValArray<T>& cshift(int count) { return ValArray<T>(a.cshift(count)); }

        // apply function from lua 

        ValArray<T> operator + (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a + b.a;
            return r;
        }
        ValArray<T> operator + (const Scalar<T> & b)  
        {
            ValArray<T> r;
            r.a = a + b.val;
            return r;
        }
        ValArray<T> operator - (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a - b.a;
            return r;
        }
        ValArray<T> operator - (const Scalar<T> & b)  
        {
            ValArray<T> r;
            r.a = a - b.val;
            return r;
        }
        ValArray<T> operator * (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a * b.a;
            return r;
        }
        ValArray<T> operator * (const Scalar<T> & b)  
        {
            ValArray<T> r;
            r.a = a * b.val;
            return r;
        }
        ValArray<T> operator / (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a / b.a;
            return r;
        }
        ValArray<T> operator / (const Scalar<T> & b)  
        {
            ValArray<T> r;
            r.a = a / b.val;
            return r;
        }
        ValArray<T> operator % (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a % b.a;
            return r;
        }
        ValArray<T> operator & (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a & b.a;
            return r;
        }
        ValArray<T> operator | (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a + b.a;
            return r;
        }
        ValArray<T> operator ^ (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a ^ b.a;
            return r;
        }
        ValArray<T> operator << (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a << b.a;
            return r;
        }
        ValArray<T> operator >> (const ValArray<T> & b)  
        {
            ValArray<T> r;
            r.a = a >> b.a;
            return r;
        }
        ValArray<bool> operator && (const ValArray<T> & b)  
        {
            ValArray<bool> r;
            r.a = a && b.a;
            return r;
        }
        ValArray<bool> operator || (const ValArray<T> & b)  
        {
            ValArray<bool> r;
            r.a = a || b.a;
            return r;
        }

        ValArray<T> operator + (const T & b)  
        {
            ValArray<T> r;
            r.a = a + b;
            return r;
        }
        ValArray<T> operator - (const T & b)  
        {
            ValArray<T> r;
            r.a = a - b;
            return r;
        }
        ValArray<T> operator * (const T & b)  
        {
            ValArray<T> r;
            r.a = a * b;
            return r;
        }
        ValArray<T> operator / (const T & b)  
        {
            ValArray<T> r;
            r.a = a / b;
            return r;
        }
        ValArray<T> operator % (const T & b)  
        {
            ValArray<T> r;
            r.a = a % b;
            return r;
        }
        ValArray<T> operator & (const T & b)  
        {
            ValArray<T> r;
            r.a = a & b;
            return r;
        }
        ValArray<T> operator | (const T & b)  
        {
            ValArray<T> r;
            r.a = a + b;
            return r;
        }
        ValArray<T> operator ^ (const T & b)  
        {
            ValArray<T> r;
            r.a = a ^ b;
            return r;
        }
        ValArray<T> operator << (const T & b)  
        {
            ValArray<T> r;
            r.a = a << b;
            return r;
        }
        ValArray<T> operator >> (const T & b)  
        {
            ValArray<T> r;
            r.a = a >> b;
            return r;
        }
        ValArray<bool> operator && (const T & b)  
        {
            ValArray<bool> r;
            r.a = a && b;
            return r;
        }
        ValArray<bool> operator || (const T & b)  
        {
            ValArray<bool> r;
            r.a = a || b;
            return r;
        }

        ValArray<T> abs()
        {
            return ValArray<T>(std::abs(a));
        }
        ValArray<T> exp()
        {
            return ValArray<T>(std::exp(a));
        }
        ValArray<T> log()
        {
            return ValArray<T>(std::log(a));
        }
        ValArray<T> log10()
        {
            return ValArray<T>(std::log10(a));
        }
        ValArray<T> pow(const ValArray<T> & base)
        {
            return ValArray<T>(std::pow(base.a,a));
        }
        ValArray<T> power(const ValArray<T> & power)
        {
            return ValArray<T>(std::pow(a,power.a));
        }
        ValArray<T> sqrt()
        {
            return ValArray<T>(std::sqrt(a));
        }
        ValArray<T> sin()
        {
            return ValArray<T>(std::sin(a));
        }
        ValArray<T> cos()
        {
            return ValArray<T>(std::cos(a));
        }
        ValArray<T> tan()
        {
            return ValArray<T>(std::tan(a));
        }
        ValArray<T> sinh()
        {
            return ValArray<T>(std::sinh(a));
        }
        ValArray<T> cosh()
        {
            return ValArray<T>(std::cosh(a));
        }
        ValArray<T> tanh()
        {
            return ValArray<T>(std::tanh(a));
        }
        ValArray<T> asin()
        {
            return ValArray<T>(std::asin(a));
        }
        ValArray<T> acos()
        {
            return ValArray<T>(std::acos(a));
        }
        ValArray<T> atan()
        {
            return ValArray<T>(std::atan(a));
        }
        ValArray<T> atan2(const ValArray<T> & b)
        {
            return ValArray<T>(std::atan2(a,b.a));
        }
        
        void csv_print()
        {
            for(size_t i=0; i < a.size(); i++)
            {
                std::cout << a[i];
                if(i < (a.size()-1)) std::cout << ",";
            }
            std::cout << std::endl;
        }

        ValArray<T> slice(size_t start, size_t size, size_t stride)
        {
            std::slice_array<T> r = a[std::slice(start,size,stride)];
            return ValArray<T>(r);
        }
        /*
        ValArray<T> gslice(const ValArray<T> & sizes, const ValArray<T> & strides)
        {
            std::slice_array<T> r = a[std::gslice(sizes.a,strides.a)];
            return ValArray<T>(r);
        }
        */
        
    };

    template<class T>
    ValArray<T> Scalar<T>::operator + (const ValArray<T> & v) { return ValArray<T>(v.a + *this); }
    template<class T>
    ValArray<T> Scalar<T>::operator - (const ValArray<T> & v) { return ValArray<T>(v.a -*this);  }
    template<class T>
    ValArray<T> Scalar<T>::operator * (const ValArray<T> & v) { return ValArray<T>(v.a * *this);}
    template<class T>
    ValArray<T> Scalar<T>::operator / (const ValArray<T> & v) { return ValArray<T>(v.a / *this);}
}