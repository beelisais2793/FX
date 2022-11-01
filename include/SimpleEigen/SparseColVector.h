#pragma once 

namespace SimpleEigen {
template<typename T>
using EigenSparseColVector = Eigen::SparseMatrix<T>;

template<class T>
struct SparseColVector : public Base
{    
    EigenSparseColVector<T> vector;
    
    SparseColVector() {}
    SparseColVector(size_t i) { vector = EigenSparseColVector<T>(i); }
    SparseColVector(size_t i, const T & v) { vector = EigenSparseColVector<T>(i); vector.fill(v); }
    SparseColVector(const SparseColVector<T> & v) { vector = v.vector;  }
    SparseColVector(const EigenSparseColVector<T> & v) { vector = v; }

    T get(int i) { return vector(i); }
    void  set(int i,T v) { vector(i) = v; }
    
    void random(int i) { vector = EigenSparseColVector<T>::Random(i); }
    void random() { vector.setRandom(vector.size()); }
    void zero() { vector.setZero(vector.size()); }    
    void ones() { vector.setOnes(vector.size()); }

    size_t  innerStride() { return vector.innerStride(); }
    T*      data() { return vector.data(); }
    void    resize(size_t size) { vector.resize(size); }
    void    resizeLike(const SparseColVector<T> & v) { vector.resizeLike(v.vector); }
    void    conservativeResize(size_t size) { vector.conservativeResize(size); }

    T coeff(size_t i) { return vector.coeff(i); }
    T& coeffRef(size_t i) { return vector.coeffRef(i); }

    void print()     {        std::cout << vector << std::endl;    }
    size_t cols() { return vector.cols(); }
    T dot(const SparseColVector<T> & b) { return vector.dot(b.vector);  }        
    size_t size() const { return vector.size(); }
    void fill(T v) { vector.fill(v); }
    T norm() { return vector.norm(); }
    //T lpNorm1() { return vector.lpNorm<1>(); }
    //T lpNorm() { return vector.lpNorm<Eigen::Infinity>(); }
    T squaredNorm() { return vector.squaredNorm(); }

    SparseColVector<T>& eval() { vector.eval(); return *this;  }

    void normalize()
    {
       vector.normalize();
    }
    SparseColVector<T> normalized() { SparseColVector<T> r; r.vector = vector.normalized(); return r; }    

    void map(std::vector<T> & a)
    {
        vector.resize(a.size());
        for(size_t i = 0; i < a.size(); i++)
            vector(i) = a[i];
    }

    SparseColVector<T>& head(SparseColVector<T>& r, size_t n) { r.vector = vector.head(n); return r; }
    SparseColVector<T>& tail(SparseColVector<T>& r, size_t n) { r.vector = vector.tail(n); return r; }
    SparseColVector<T>& segment(SparseColVector<T>& r, size_t i, size_t n) { r.vector = vector.segment(i,n); return r; }
    SparseColVector<T>& LinSpaced(T num, T a, T b)
    {        
        vector = EigenSparseColVector<T>::LinSpaced(num,a,b);
        return *this;
    }

    T operator()(size_t i) { return vector(i); }
    T operator[](size_t i) { return vector(i); }
    T __getitem(size_t i) { return vector(i); }
    void __setitem(size_t i, T val) { vector(i) = val; }

    /*
    T inner_product(const SparseColVector<T> & v)
    { return vector.adjoint() * v.vector; }
    */

    SparseColVector<T> operator + (const SparseColVector<T>& b) 
    { 
        SparseColVector<T> r(*this);
        r.vector = r.vector + b.vector;
        return r;
    }
    SparseColVector<T> operator - (const SparseColVector<T>& b) 
    { 
        SparseColVector<T> r(*this);
        r.vector = r.vector - b.vector;
        return r;
    }    
    
    T operator * (const SparseColVector<T>& b) 
    { 
        return vector.dot(b.vector);        
    }

    
    SparseColVector<T> operator * (const T b) 
    { 
        SparseColVector<T> r(*this);
        r.vector = r.vector * b;
        return r;
    }

    SparseColVector<T> operator / (const T b)  
    { 
        SparseColVector<T> r(*this);
        r.vector = r.vector / b;
        return r;
    }

    
    void aplus(const SparseColVector<T>& b) { vector += b.vector; }
    void aminus(const SparseColVector<T>& b) { vector -= b.vector; }
    void adot(const SparseColVector<T>& b) { vector.dot(b.vector); }
    
    void atimes(const T& b) { vector = vector * b; }
    void adiv(const T& b) { vector = vector / b; }


    SparseColVector<T> t() { return SparseColVector<T>(vector.transpose().eval());}    
    SparseColVector<T> abs() {  return SparseColVector<T>(vector.cwiseAbs());  }
    SparseColVector<T> inverse() { return SparseColVector<T>(vector.cwiseInverse()); }    
    SparseColVector<T> sqrt() { return SparseColVector<T>(vector.cwiseSqrt()); }
    SparseColVector<T> abs2() { return SparseColVector<T>(vector.cwiseAbs2()); }
    SparseColVector<T> operator * (SparseMatrix<T> & m);

    // add to each col
    SparseMatrix<T> operator + (SparseMatrix<T> & m);
    // subtract from each col 
    SparseMatrix<T> operator - (SparseMatrix<T> & m);
    // divide from each col
    SparseMatrix<T> operator / (SparseMatrix<T> & m);

    SparseMatrix<T> matrix();    
};

}