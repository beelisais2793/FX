#pragma once 

namespace SimpleEigen {

template<typename T>
using EigenColVector = Eigen::Matrix<T,Eigen::Dynamic,1,Eigen::ColMajor>;

template<typename T>
using EigenArray1 = Eigen::Array<T,1,Eigen::Dynamic>;

template<typename T>
using EigenColArray = Eigen::Array<T,1,Eigen::Dynamic, Eigen::ColMajor>;

template<class T>
struct ColVector : public Base
{    
    EigenColVector<T> vector;
    
    ColVector() {}
    ColVector(size_t i) { vector = EigenColVector<T>(i); }
    ColVector(size_t i, const T & v) { vector = EigenColVector<T>(i); vector.fill(v); }
    ColVector(const ColVector<T> & v) { vector = v.vector;  }
    ColVector(const EigenColVector<T> & v) { vector = v; }
    
    
    
    T get(int i) { return vector(i); }
    void  set(int i,T v) { vector(i) = v; }
    
    void random(int i) { vector = EigenColVector<T>::Random(i); }
    void random() { vector.setRandom(vector.size()); }
    void zero() { vector.setZero(vector.size()); }    
    void ones() { vector.setOnes(vector.size()); }

    size_t  innerStride() { return vector.innerStride(); }
    T*      data() { return vector.data(); }
    void    resize(size_t size) { vector.resize(size); }
    void    resizeLike(const ColVector<T> & v) { vector.resizeLike(v.vector); }
    void    conservativeResize(size_t size) { vector.conservativeResize(size); }

    T coeff(size_t i) { return vector.coeff(i); }
    T& coeffRef(size_t i) { return vector.coeffRef(i); }

    void print()     {        std::cout << vector << std::endl;    }
    size_t cols() { return vector.cols(); }
    T dot(const ColVector<T> & b) { return vector.dot(b.vector);  }        
    size_t size() const { return vector.size(); }
    void fill(T v) { vector.fill(v); }
    T norm() { return vector.norm(); }
    //T lpNorm1() { return vector.lpNorm<1>(); }
    //T lpNorm() { return vector.lpNorm<Eigen::Infinity>(); }
    T squaredNorm() { return vector.squaredNorm(); }

    ColVector<T>& eval() { vector.eval(); return *this;  }

    void normalize()
    {
       vector.normalize();
    }
    ColVector<T> normalized() { ColVector<T> r; r.vector = vector.normalized(); return r; }    

    void map(std::vector<T> & a)
    {
        vector.resize(a.size());
        for(size_t i = 0; i < a.size(); i++)
            vector(i) = a[i];
    }

    ColVector<T>& head(ColVector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
    ColVector<T>& tail(ColVector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
    ColVector<T>& segment(ColVector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
    ColVector<T>& LinSpaced(T num, T a, T b)
    {        
        vector = EigenColVector<T>::LinSpaced(num,a,b);
        return *this;
    }

    T operator()(size_t i) { return vector(i); }
    T operator[](size_t i) { return vector(i); }
    T __getitem(size_t i) { return vector(i); }
    void __setitem(size_t i, T val) { vector(i) = val; }

    /*
    T inner_product(const ColVector<T> & v)
    { return vector.adjoint() * v.vector; }
    */

    ColVector<T> operator + (const ColVector<T>& b) 
    { 
        ColVector<T> r(*this);
        r.vector = r.vector + b.vector;
        return r;
    }
    ColVector<T> operator - (const ColVector<T>& b) 
    { 
        ColVector<T> r(*this);
        r.vector = r.vector - b.vector;
        return r;
    }    
    
    T operator * (const ColVector<T>& b) 
    { 
        return vector.dot(b.vector);        
    }

    
    ColVector<T> operator * (const T b) 
    { 
        ColVector<T> r(*this);
        r.vector = r.vector * b;
        return r;
    }

    ColVector<T> operator / (const T b)  
    { 
        ColVector<T> r(*this);
        r.vector = r.vector / b;
        return r;
    }

    
    void aplus(const ColVector<T>& b) { vector += b.vector; }
    void aminus(const ColVector<T>& b) { vector -= b.vector; }
    void adot(const ColVector<T>& b) { vector.dot(b.vector); }
    
    void atimes(const T& b) { vector = vector * b; }
    void adiv(const T& b) { vector = vector / b; }


    ColVector<T> t() { return ColVector<T>(vector.transpose().eval());}    
    ColVector<T> abs() {  return ColVector<T>(vector.cwiseAbs());  }
    ColVector<T> inverse() { return ColVector<T>(vector.cwiseInverse()); }    
    ColVector<T> sqrt() { return ColVector<T>(vector.cwiseSqrt()); }
    ColVector<T> abs2() { return ColVector<T>(vector.cwiseAbs2()); }
    ColVector<T> operator * (Matrix<T> & m);

    ColVector<T> operator = (const ColVector<T> & v) { vector = v.vector; return *this; }
    ColVector<T> operator = (const EigenColVector<T> & v) { vector = v; return *this; }

    // add to each col
    Matrix<T> operator + (Matrix<T> & m);
    // subtract from each col 
    Matrix<T> operator - (Matrix<T> & m);
    // divide from each col
    Matrix<T> operator / (Matrix<T> & m);

    Matrix<T> matrix();    
};

template<typename T> ColVector <T>  abs(ColVector <T> & vector ) { ColVector <T>  r; r = vector.abs(); return r;}
template<typename T> ColVector <T>  inverse(ColVector <T> & vector ) { ColVector <T>  r; r = vector.inverse(); return r; }
template<typename T> ColVector <T>  exp(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().exp(); return r;  }
template<typename T> ColVector <T>  log(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().log(); return r;  }
template<typename T> ColVector <T>  log1p(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().log1p(); return r; }
template<typename T> ColVector <T>  log10(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().log10(); return r; }
template<typename T> ColVector <T>  pow(ColVector <T> & vector ,const T& b) { ColVector <T>  r; r = vector.vector.array().pow(b); return r;}
template<typename T> ColVector <T>  sqrt(ColVector <T> & vector ) { ColVector <T>  r; r = vector.sqrt(); return r;}
template<typename T> ColVector <T>  rsqrt(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().rsqrt(); return r; }
template<typename T> ColVector <T>  square(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().square(); return r;}
template<typename T> ColVector <T>  cube(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().cube(); return r; }
template<typename T> ColVector <T>  abs2(ColVector <T> & vector ) { ColVector <T>  r; r = vector.abs2(); return r; }
template<typename T> ColVector <T>  sin(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().sin(); return r;}
template<typename T> ColVector <T>  cos(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().cos(); return r; }
template<typename T> ColVector <T>  tan(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().tan(); return r;}
template<typename T> ColVector <T>  asin(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().asin(); return r; }
template<typename T> ColVector <T>  acos(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().acos(); return r; }
template<typename T> ColVector <T>  atan(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().atan(); return r; }
template<typename T> ColVector <T>  sinh(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().sinh(); return r; }
template<typename T> ColVector <T>  cosh(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().cosh(); return r; }
template<typename T> ColVector <T>  tanh(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().tanh(); return r; }
template<typename T> ColVector <T>  ceil(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().ceil(); return r; }
template<typename T> ColVector <T>  floor(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().floor(); return r; }
template<typename T> ColVector <T>  round(ColVector <T> & vector ) { ColVector <T>  r; r = vector.vector.array().round(); return r; }

template<typename T> ColVector<T> asinh(const ColVector<T> & vector) { return ColVector<T>(vector.vector.array().asinh().matrix()); }
template<typename T> ColVector<T> acosh(const ColVector<T> & vector) { return ColVector<T>(vector.vector.array().acosh().matrix()); }
template<typename T> ColVector<T> atanh(const ColVector<T> & vector) { return ColVector<T>(vector.vector.array().atanh().matrix()); }
template<typename T> ColVector<T> rint(const ColVector<T> & vector)  { return ColVector<T>(vector.vector.array().rint().matrix());  }

template<typename T>
void random(ColVector<T> & v, int i) { v.random(i); }

template<typename T>
void random(ColVector<T> & vector) { vector.random(); }

template<typename T>
void zero(ColVector<T> & vector) { vector.zero(); }    

template<typename T>
void ones(ColVector<T> & vector) { vector.ones(); }

template<typename T>
size_t  innerStride(ColVector<T> & vector) { return vector.innerStride(); }

template<typename T>
void    resize(ColVector<T> & vector, size_t size) { vector.resize(size); }

template<typename T>
void    resizeLike(ColVector<T> & vector, const ColVector<T> & v) { vector.resizeLike(v.vector); }

template<typename T>
void    conservativeResize(ColVector<T> & vector,size_t size) { vector.conservativeResize(size); }

template<typename T>
T coeff(ColVector<T> & vector,size_t i) { return vector.coeff(i); }

template<typename T>
T& coeffRef(ColVector<T> & vector, size_t i) { return vector.coeffRef(i); }

template<typename T>
void print(ColVector<T> & vector)     {    vector.print();  }

template<typename T>
size_t cols(ColVector<T> & vector) { return vector.cols(); }

template<typename T>
T dot(ColVector<T> & vector, const ColVector<T> & b) { return vector.dot(b.vector);  }        

template<typename T>
size_t size(ColVector<T> & vector) { return vector.size(); }

template<typename T>
void fill(ColVector<T> & vector,T v) { vector.fill(v); }

template<typename T>
T norm(ColVector<T> & vector) { return vector.norm(); }

template<typename T>
T squaredNorm(ColVector<T> & vector) { return vector.squaredNorm(); }

template<typename T>
ColVector<T>& eval(ColVector<T> & vector) { return vector.eval(); }

template<typename T>
void normalize(ColVector<T> & vector) { vector.normalize(); }

template<typename T>
ColVector<T> normalized(ColVector<T> & vector) { return vector.normalized();  }    

template<typename T>
ColVector<T> transpose(ColVector<T> & vector) { return ColVector<T>(vector.transpose().eval());}    
template<typename T>
ColVector<T> cwiseAbs(ColVector<T> & vector) {  return ColVector<T>(vector.cwiseAbs());  }
template<typename T>
ColVector<T> cwiseInverse(ColVector<T> & vector) { return ColVector<T>(vector.cwiseInverse()); }    
template<typename T>
ColVector<T> cwiseSqrt(ColVector<T> & vector) { return ColVector<T>(vector.cwiseSqrt()); }
template<typename T>
ColVector<T> cwiseAbs2(ColVector<T> & vector) { return ColVector<T>(vector.cwiseAbs2()); }

}