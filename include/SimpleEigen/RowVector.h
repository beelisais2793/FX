#pragma once 

namespace SimpleEigen {

template<typename T>
using EigenRowVector = Eigen::Matrix<T,1,Eigen::Dynamic,Eigen::RowMajor>;

template<class T>
struct RowVector
{
    EigenRowVector<T> vector;
    
    RowVector() {}
    RowVector(size_t i) { vector = EigenRowVector<T>(i); }
    RowVector(size_t i, const T & v) { vector = EigenRowVector<T>(i); vector.fill(v); }
    RowVector(const RowVector<T> & v) { vector = v.vector;  }
    RowVector(const EigenRowVector<T> & v) { vector = v; }

    T get(int i) { return vector(i); }
    void set(int i,T v) { vector(i) = v; }
    
    void random(int i) { vector = EigenRowVector<T>::Random(i); }
    void random() { vector.setRandom(vector.size()); }
    void zero() { vector.setZero(vector.size()); }    
    void ones() { vector.setOnes(vector.size()); }

    size_t innerStride() { return vector.innerStride(); }
    T* data() { return vector.data(); }
    void    resize(size_t size) { vector.resize(size); }
    void    resizeLike(const RowVector<T> & v) { vector.resizeLike(v.vector); }
    void    conservativeResize(size_t size) { vector.conservativeResize(size); }
    
    T coeff(size_t i) { return vector.coeff(i); }
    T& coeffRef(size_t i) { return vector.coeffRef(i); }
    
    void normalize()    {       vector.normalize();    }    
    RowVector<T> normalized() { RowVector<T> r; r.vector = vector.normalized(); return r; }    
    void print()     {        std::cout << vector << std::endl;    }    
    size_t cols() { return vector.cols(); }    
    T dot(const RowVector<T> & b) { return vector.dot(b.vector);  }            
    size_t  size() const { return vector.size(); }    
    void    fill(T v) { vector.fill(v); }
    RowVector<T>& eval() { vector.eval(); return *this; }
    
    T norm() { return vector.norm(); }
    // doesn't work
    //T lpNorm1() { return vector.lpNorm<1>(); }
    //T lpNorm() { return vector.lpNorm<Eigen::Infinity>(); }
    T squaredNorm() { return vector.squaredNorm(); }

    void map(std::vector<T> & a)
    {
        vector.resize(a.size());
        for(size_t i = 0; i < a.size(); i++)
            vector(i) = a[i];
    }

    RowVector<T>& head(RowVector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
    RowVector<T>& tail(RowVector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
    RowVector<T>& segment(RowVector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
    RowVector<T>& LinSpaced(T num, T a, T b)
    {        
        vector = EigenRowVector<T>::LinSpaced(num,a,b);
        return *this;
    }

    /*
    T inner_product(const RowVector<T> & v)
    { return vector.adjoint() * v.vector; }
    */

   RowVector<T>& operator = (const RowVector<T> & v) { vector = v.vector; return *this; }
   RowVector<T>& operator = (const EigenRowVector<T> & v) { vector = v; return *this; }

    T operator()(size_t i) { return vector(i); }
    T operator[](size_t i) { return vector(i); }
    T __getitem(size_t i) { return vector(i); }
    void __setitem(size_t i, T val) { vector(i) = val; }

    
    RowVector<T> operator + (const RowVector<T>& b) 
    { 
        RowVector<T> r(*this);
        r.vector = r.vector + b.vector;
        return r;
    }
    RowVector<T> operator - (const RowVector<T>& b) 
    { 
        RowVector<T> r(*this);
        r.vector = r.vector - b.vector;
        return r;
    }    
    
    T operator * (const RowVector<T>& b) 
    { 
        return vector.dot(b.vector);        
    }
   
    RowVector<T> operator * (const T b) 
    { 
        RowVector<T> r(*this);
        r.vector = r.vector * b;
        return r;
    }

    RowVector<T> operator / (const T b)  
    { 
        RowVector<T> r(*this);
        r.vector = r.vector / b;
        return r;
    }
    
    void aplus(const RowVector<T>& b) { vector += b.vector; }
    void aminus(const RowVector<T>& b) { vector -= b.vector; }
    void adot(const RowVector<T>& b) { vector.dot(b.vector); }
    
    void atimes(const T& b) { vector = vector * b; }
    void adiv(const T& b) { vector = vector / b; }


    
    RowVector<T> t() { return RowVector<T>(vector.transpose().eval());}    
    RowVector<T> abs() {  return RowVector<T>(vector.cwiseAbs());  }
    RowVector<T> inverse() { return RowVector<T>(vector.cwiseInverse()); }    
    RowVector<T> sqrt() { return RowVector<T>(vector.cwiseSqrt()); }
    RowVector<T> abs2() { return RowVector<T>(vector.cwiseAbs2()); }    

    

    RowVector<T> operator * (Matrix<T> & m);
    
    // add to each row
    Matrix<T> operator + (Matrix<T> & m);
    // subtract from each row 
    Matrix<T> operator - (Matrix<T> & m);
    // divide from each row 
    Matrix<T> operator / (Matrix<T> & m);

    Matrix<T> matrix();        
};


template<typename T> RowVector<T>  abs(RowVector<T> & vector ) { RowVector<T>  r; r = vector.abs(); return r;}
template<typename T> RowVector<T>  inverse(RowVector<T> & vector ) { RowVector<T>  r; r = vector.inverse(); return r; }
template<typename T> RowVector<T>  exp(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().exp(); return r;  }
template<typename T> RowVector<T>  log(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().log(); return r;  }
template<typename T> RowVector<T>  log1p(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().log1p(); return r; }
template<typename T> RowVector<T>  log10(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().log10(); return r; }
template<typename T> RowVector<T>  pow(RowVector<T> & vector ,const T& b) { RowVector<T>  r; r = vector.vector.array().pow(b); return r;}
template<typename T> RowVector<T>  sqrt(RowVector<T> & vector ) { RowVector<T>  r; r = vector.sqrt(); return r;}
template<typename T> RowVector<T>  rsqrt(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().rsqrt(); return r; }
template<typename T> RowVector<T>  square(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().square(); return r;}
template<typename T> RowVector<T>  cube(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().cube(); return r; }
template<typename T> RowVector<T>  abs2(RowVector<T> & vector ) { RowVector<T>  r; r = vector.abs2(); return r; }
template<typename T> RowVector<T>  sin(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().sin(); return r;}
template<typename T> RowVector<T>  cos(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().cos(); return r; }
template<typename T> RowVector<T>  tan(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().tan(); return r;}
template<typename T> RowVector<T>  asin(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().asin(); return r; }
template<typename T> RowVector<T>  acos(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().acos(); return r; }
template<typename T> RowVector<T>  atan(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().atan(); return r; }
template<typename T> RowVector<T>  sinh(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().sinh(); return r; }
template<typename T> RowVector<T>  cosh(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().cosh(); return r; }
template<typename T> RowVector<T>  tanh(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().tanh(); return r; }
template<typename T> RowVector<T>  ceil(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().ceil(); return r; }
template<typename T> RowVector<T>  floor(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().floor(); return r; }
template<typename T> RowVector<T>  round(RowVector<T> & vector ) { RowVector<T>  r; r = vector.vector.array().round(); return r; }

template<typename T> RowVector<T> asinh(const RowVector<T> & vector) { return RowVector<T>(vector.vector.array().asinh().matrix()); }
template<typename T> RowVector<T> acosh(const RowVector<T> & vector) { return RowVector<T>(vector.vector.array().acosh().matrix()); }
template<typename T> RowVector<T> atanh(const RowVector<T> & vector) { return RowVector<T>(vector.vector.array().atanh().matrix()); }
template<typename T> RowVector<T> rint(const RowVector<T> & vector)  { return RowVector<T>(vector.vector.array().rint().matrix());  }

template<typename T>
void random(RowVector<T> & v, int i) { v.random(i); }

template<typename T>
void random(RowVector<T> & vector) { vector.random(); }

template<typename T>
void zero(RowVector<T> & vector) { vector.zero(); }    

template<typename T>
void ones(RowVector<T> & vector) { vector.ones(); }

template<typename T>
size_t  innerStride(RowVector<T> & vector) { return vector.innerStride(); }

template<typename T>
void    resize(RowVector<T> & vector, size_t size) { vector.resize(size); }

template<typename T>
void    resizeLike(RowVector<T> & vector, const RowVector<T> & v) { vector.resizeLike(v.vector); }

template<typename T>
void    conservativeResize(RowVector<T> & vector,size_t size) { vector.conservativeResize(size); }

template<typename T>
T coeff(RowVector<T> & vector,size_t i) { return vector.coeff(i); }

template<typename T>
T& coeffRef(RowVector<T> & vector, size_t i) { return vector.coeffRef(i); }

template<typename T>
void print(RowVector<T> & vector)     {    vector.print();  }

template<typename T>
size_t cols(RowVector<T> & vector) { return vector.cols(); }

template<typename T>
T dot(RowVector<T> & vector, const RowVector<T> & b) { return vector.dot(b.vector);  }        

template<typename T>
size_t size(RowVector<T> & vector) { return vector.size(); }

template<typename T>
void fill(RowVector<T> & vector,T v) { vector.fill(v); }

template<typename T>
T norm(RowVector<T> & vector) { return vector.norm(); }

template<typename T>
T squaredNorm(RowVector<T> & vector) { return vector.squaredNorm(); }

template<typename T>
RowVector<T>& eval(RowVector<T> & vector) { return vector.eval(); }

template<typename T>
void normalize(RowVector<T> & vector) { vector.normalize(); }

template<typename T>
RowVector<T> normalized(RowVector<T> & vector) { return vector.normalized();  }    

template<typename T>
RowVector<T> transpose(RowVector<T> & vector) { return RowVector<T>(vector.transpose().eval());}    
template<typename T>
RowVector<T> cwiseAbs(RowVector<T> & vector) {  return RowVector<T>(vector.cwiseAbs());  }
template<typename T>
RowVector<T> cwiseInverse(RowVector<T> & vector) { return RowVector<T>(vector.cwiseInverse()); }    
template<typename T>
RowVector<T> cwiseSqrt(RowVector<T> & vector) { return RowVector<T>(vector.cwiseSqrt()); }
template<typename T>
RowVector<T> cwiseAbs2(RowVector<T> & vector) { return RowVector<T>(vector.cwiseAbs2()); }

}

