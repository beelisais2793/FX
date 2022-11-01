#pragma once 

namespace SimpleEigen {
template<typename T>
using EigenSparseRowVector = Eigen::SparseMatrix<T>;


template<class T>
struct SparseRowVector
{
    EigenSparseRowVector<T> vector;
    
    SparseRowVector() {}
    SparseRowVector(size_t i) { vector = EigenSparseRowVector<T>(i); }
    SparseRowVector(size_t i, const T & v) { vector = EigenSparseRowVector<T>(i); vector.fill(v); }
    SparseRowVector(const SparseRowVector<T> & v) { vector = v.vector;  }
    SparseRowVector(const EigenSparseRowVector<T> & v) { vector = v; }

    T get(int i) { return vector(i); }
    void set(int i,T v) { vector(i) = v; }
    
    void random(int i) { vector = EigenSparseRowVector<T>::Random(i); }
    void random() { vector.setRandom(vector.size()); }
    void zero() { vector.setZero(vector.size()); }    
    void ones() { vector.setOnes(vector.size()); }

    size_t innerStride() { return vector.innerStride(); }
    T* data() { return vector.data(); }
    void    resize(size_t size) { vector.resize(size); }
    void    resizeLike(const SparseRowVector<T> & v) { vector.resizeLike(v.vector); }
    void    conservativeResize(size_t size) { vector.conservativeResize(size); }
    
    T coeff(size_t i) { return vector.coeff(i); }
    T& coeffRef(size_t i) { return vector.coeffRef(i); }
    
    void normalize()    {       vector.normalize();    }    
    SparseRowVector<T> normalized() { SparseRowVector<T> r; r.vector = vector.normalized(); return r; }    
    void print()     {        std::cout << vector << std::endl;    }    
    size_t cols() { return vector.cols(); }    
    T dot(const SparseRowVector<T> & b) { return vector.dot(b.vector);  }            
    size_t  size() const { return vector.size(); }    
    void    fill(T v) { vector.fill(v); }
    SparseRowVector<T>& eval() { vector.eval(); return *this; }
    
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

    SparseRowVector<T>& head(SparseRowVector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
    SparseRowVector<T>& tail(SparseRowVector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
    SparseRowVector<T>& segment(SparseRowVector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
    SparseRowVector<T>& LinSpaced(T num, T a, T b)
    {        
        vector = EigenSparseRowVector<T>::LinSpaced(num,a,b);
        return *this;
    }

    /*
    T inner_product(const SparseRowVector<T> & v)
    { return vector.adjoint() * v.vector; }
    */

    T operator()(size_t i) { return vector(i); }
    T operator[](size_t i) { return vector(i); }
    T __getitem(size_t i) { return vector(i); }
    void __setitem(size_t i, T val) { vector(i) = val; }

    
    SparseRowVector<T> operator + (const SparseRowVector<T>& b) 
    { 
        SparseRowVector<T> r(*this);
        r.vector = r.vector + b.vector;
        return r;
    }
    SparseRowVector<T> operator - (const SparseRowVector<T>& b) 
    { 
        SparseRowVector<T> r(*this);
        r.vector = r.vector - b.vector;
        return r;
    }    
    
    T operator * (const SparseRowVector<T>& b) 
    { 
        return vector.dot(b.vector);        
    }
   
    SparseRowVector<T> operator * (const T b) 
    { 
        SparseRowVector<T> r(*this);
        r.vector = r.vector * b;
        return r;
    }

    SparseRowVector<T> operator / (const T b)  
    { 
        SparseRowVector<T> r(*this);
        r.vector = r.vector / b;
        return r;
    }
    
    void aplus(const SparseRowVector<T>& b) { vector += b.vector; }
    void aminus(const SparseRowVector<T>& b) { vector -= b.vector; }
    void adot(const SparseRowVector<T>& b) { vector.dot(b.vector); }
    
    void atimes(const T& b) { vector = vector * b; }
    void adiv(const T& b) { vector = vector / b; }


    
    SparseRowVector<T> t() { return SparseRowVector<T>(vector.transpose().eval());}    
    SparseRowVector<T> abs() {  return SparseRowVector<T>(vector.cwiseAbs());  }
    SparseRowVector<T> inverse() { return SparseRowVector<T>(vector.cwiseInverse()); }    
    SparseRowVector<T> sqrt() { return SparseRowVector<T>(vector.cwiseSqrt()); }
    SparseRowVector<T> abs2() { return SparseRowVector<T>(vector.cwiseAbs2()); }    

    SparseRowVector<T> operator * (Matrix<T> & m);
    
    // add to each row
    Matrix<T> operator + (Matrix<T> & m);
    // subtract from each row 
    Matrix<T> operator - (Matrix<T> & m);
    // divide from each row 
    Matrix<T> operator / (Matrix<T> & m);

    Matrix<T> matrix();        
};
}