#pragma once

#include "Std.h"
#include <ccomplex>
#include <complex>
#include "cppmkl/cppmkl_allocator.h"
#include "cppmkl/cppmkl_vml.h"
#include "cppmkl/cppmkl_cblas.h"

namespace Std
{
    
    template<typename T>
    struct Vector
    {
        std::vector<T, cppmkl::cppmkl_allocator<T> > vector;
        
        Vector() = default;
        Vector(size_t n) { assert(n > 0); vector.resize(n); }
        Vector(const Vector<T> & v) { vector = v.vector; }
        Vector(const std::vector<T> & v) {
            vector.resize(v.size());
            memcpy(vector.data(),v.data(),v.size()*sizeof(T));
        }

        Vector<T>& operator = (const Vector<T> & v)
        {
            vector = v.vector;
            return *this;
        }

        T min() { return *std::min_element(vector.begin(),vector.end()); }
        T max() { return *std::max_element(vector.begin(),vector.end()); }
        size_t min_index() { return std::distance(vector.begin(), std::min_element(vector.begin(),vector.end())); }
        size_t max_index() { return std::distance(vector.begin(), std::max_element(vector.begin(),vector.end())); }
        
        T& operator[](size_t i) { return vector[i]; }
        const T& operator[](size_t i) const { return vector[i]; }
        T __getitem(size_t i) { return vector[i]; }
        void __setitem(size_t i, T ) { vector[i] = ; }

        size_t size() const { return vector.size(); }
        T* data() { return vector.data(); }
        void resize(size_t i) { vector.resize(i); }

        T front() const { return vector.front(); }
        T back() const  { return vector.back();  }

        void push_back(T ) { vector.push_back(); }
        T    pop_back() { T x = vector.back(); vector.pop_back(); return x; }

        Vector<T>& operator +=  (const Vector<T> & v) { 
            cppmkl::vadd(size(),vector.data(),v.vector.data(),vector.data());
            return *this;
        }
        Vector<T>& operator -=  (const Vector<T> & v) { 
            cppmkl::vsub(size(),vector.data(),v.vector.data(),vector.data());
            return *this;
        }
        Vector<T>& operator *=  (const Vector<T> & v) { 
            cppmkl::vmul(size(),vector.data(),v.vector.data(),vector.data());
            return *this;
        }
        Vector<T>& operator /=  (const Vector<T> & v) { 
            cppmkl::vdiv(size(),vector.data(),v.vector.data(),vector.data());
            return *this;
        }
        Vector<T>& operator %=  (const Vector<T> & v) { 
            cppmkl::vmodf(size(),vector.data(),v.vector.data(),vector.data());
            return *this;
        }

        Vector<T> operator + (const Vector<T> & v) { 
            Vector<T> r(size());            
            cppmkl::vadd(size(),vector.data(),v.vector.data(),r.vector.data());
            return r;
        }
        Vector<T> operator - (const Vector<T> & v) { 
            Vector<T> r(size());            
            cppmkl::vsub(size(),vector.data(),v.vector.data(),r.vector.data());
            return r;            
        }
        Vector<T> operator * (const Vector<T> & v) { 
            Vector<T> r(size());            
            cppmkl::vmul(size(),vector.data(),v.vector.data(),r.vector.data());
            return r;
        }
        Vector<T> operator / (const Vector<T> & v) { 
            Vector<T> r(size());            
            cppmkl::vdiv(size(),vector.data(),v.vector.data(),r.vector.data());
            return r;
        }
        Vector<T> operator % (const Vector<T> & v) { 
            Vector<T> r(size());            
            cppmkl::vmodf(size(),vector.data(),v.vector.data(),r.vector.data());
            return r;
        }        

        void zero() {
            memset(vector.data(),0x00,size()*sizeof(T));
        }
        void fill(T x) {
            for(size_t i = 0; i < size(); i++) vector[i] = x;
        }
        void ones() {
            fill((T)1);
        }
        void random(T min = T(0), T max = T(1)) {
            StdRandomUniform r;
            for(size_t i = 0; i < size(); i++) vector[i] = r.random(min,max);
        }

        Vector<T> slice(size_t start, size_t len) {
            Vector<T> x(len);
            memcpy(x.vector.data(),vector.data()+start,len*sizeof(T));
            return x;
        }
    };

    template<typename T> struct Matrix;
    template<typename T>
    struct MatrixView 
    {
        Matrix<T> * matrix;
        size_t row;
        MatrixView(Matrix<T> * m, size_t r) {
            matrix = m;
            row = r;
        }

        T& operator[](size_t i);
        T  __getitem(size_t i);
        void __setitem(size_t i, T );
    };

    template<typename T>
    struct Matrix
    {
        Vector<T> matrix;
        size_t M;
        size_t N;

        Matrix() { M = N = 0; }
        Matrix(size_t m, size_t n) {
            matrix.resize(m*n);
            M = m;
            N = n;
            assert(M > 0);
            assert(N > 0);
        }
        Matrix(const Matrix<T> & m) {
            matrix = m.matrix;
            M = m.M;
            N = m.N;
        }
        Matrix(T * ptr, size_t m, size_t n)
        {
            matrix.resize(m*n);
            M = m;
            N = n;
            memcpy(matrix.data(),ptr,m*n*sizeof(T));
        }

        Matrix<T>& operator = (const Matrix<T> & m) {
            matrix = m.matrix;
            M = m.M;
            N = m.N;
            return *this;
        }

        size_t size() const { return matrix.size(); }
        size_t rows() const { return M; }
        size_t cols() const { return N; }

        void resize(size_t r, size_t c) {
            M = r;
            N = c;
            matrix.resize(r*c);
        }
        T& operator()(size_t i, size_t j) { return matrix[i*N + j]; }
        
        MatrixView<T> operator[](size_t row) { return MatrixView<T>(&matrix,row); }

        Matrix<T> operator + (const Matrix<T> & m) {
            Matrix<T> r(rows(),cols());
            r.matrix = matrix + m.matrix;
            return r;
        }
        Matrix<T> operator - (const Matrix<T> & m) {
            Matrix<T> r(rows(),cols());
            r.matrix = matrix - m.matrix;
            return r;
        }
        Matrix<T> operator * (Matrix<T> & b) {
            assert(N == b.M);
            Matrix<T> r(rows(),b.cols());
            int m = M;
            int k = N;
            int n = b.N;
            cppmkl::cblas_gemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,m,n,k,1.0,matrix.data(),m,b.matrix.data(),n,0.0,r.matrix.data(),n);
            return r;
        }
        
        Matrix<T> operator + (const Vector<T> & x) {
            Matrix<T> r(rows(),cols());
            for(size_t i = 0; i < M; i++)
                cppmkl::vadd(matrix.size(),matrix.data()+i*N,x.vector.data(),r.vector.data()+i);
            return r;                
        }
        Matrix<T> operator - (const Vector<T> & x) {
            Matrix<T> r(rows(),cols());
            for(size_t i = 0; i < M; i++)
                cppmkl::vsub(matrix.size(),matrix.data()+i*N,x.vector.data(),r.vector.data()+i);
            return r;
        }
        Matrix<T> operator * (const Vector<T> & x) {
            Matrix<T> r(rows(),cols());
            for(size_t i = 0; i < M; i++)
                cppmkl::vmul(N,matrix.data()+i*N,x.vector.data(),r.matrix.data()+i*N);
            return r;
        }
        Matrix<T> operator / (const Vector<T> & x) {
            Matrix<T> r(rows(),cols());
            for(size_t i = 0; i < M; i++)
                cppmkl::vdiv(N,matrix.data()+i*N,x.vector.data(),r.matrix.data()+i*N);
            return r;
        }
        Matrix<T> operator % (const Vector<T> & x) {
            Matrix<T> r(rows(),cols());
            for(size_t i = 0; i < M; i++)
                cppmkl::vmodf(N,matrix.data()+i*N,x.vector.data(),r.matrix.data()+i*N);
            return r;
        }

        Matrix<T> hadamard(const Matrix<T> & m) {
            Matrix<T> r(rows(),cols());
            assert(rows() == m.rows() && cols() == m.cols());
            cppmkl::vmul(size(),matrix.data(),m.matrix.data(),r.matrix.data());
        }

        Matrix<T> transpose() {
            Matrix<T> r(cols(),rows());
            for(size_t i = 0; i < rows(); i++)
                for(size_t j = 0; j < cols(); j++)
                    r(j,i) = matrix[i][j];
            return r;
        }
        void zero() {
            memset(matrix.data(),0x00,size()*sizeof(T));
        }
        void fill(T x) {
            for(size_t i = 0; i < size(); i++) matrix[i] = x;
        }
        void ones() {
            fill((T)1);
        }
        void random(T min = T(0), T max = T(1)) {
            StdRandomUniform r;
            for(size_t i = 0; i < size(); i++) matrix[i] = r.random(min,max);
        }
        void identity() {
            size_t x = 0;
            zero();
            for(size_t i = 0; i < rows(); i++)
            {
                matrix[i*N + x++] = 0;
            }
        }
        Vector<T>& get_matrix() { return matrix; }

    };

    /*
    template<typename T> struct Cube;
    template<typename T>
    struct CubeView 
    {
        Cube<T> * cube;
        size_t row;
        CubeView(Matrix<T> * m, size_t r) {
            matrix = m;
            row = r;
        }

        MatrixView<T> operator[](size_t i);
        VectorView<T> operator[](size_t i, size_t j);
        T  __getitem(size_t i);
        void __setitem(size_t i, T );
    };
    */

    template<typename T>
    struct Cube
    {
        Vector<T> cube;
        size_t M;
        size_t N;
        size_t O;

        Cube() { M = N = O = 0; }
        Cube(size_t m, size_t n, size_t o) {
            M = m;
            N = n;
            O = o;
            assert(M > 0);
            assert(N > 0);
            assert(O > 0);
            cube.resize(M*N*O);
        }
        Cube(const Cube<T> & c) {
            cube = c.cube;
        }

        Cube<T>& operator = (const Cube<T> & m) {
            cube = m.cube;
            M = m.M;
            N = m.N;
            O = m.O;
            return *this;
        }

        size_t size() const { return cube.size(); }
        size_t rows() const { return M; }
        size_t cols() const { return N; }
        size_t depth() const { return O; }

        void resize(size_t r, size_t c, size_t k) {
            M = r;
            N = c;
            O = k;
            matrix.resize(r*c*k);
        }
        T& operator()(size_t i, size_t j, size_t k) { return matrix[i*N + j*O + k]; }
        
        Cube<T> operator + (const Cube<T> & m) {
            Cube<T> r(rows(),cols(),depth());
            r.cube = cube + m.cube;
            return r;
        }
        Cube<T> operator - (const Cube<T> & m) {
            Cube<T> r(rows(),cols(),depth());
            r.cube = matrix - m.cube;
            return r;
        }
        Cube<T> operator * (Cube<T> & b) {            
            Cube<T> r(rows(),cols(),depth());
            r.cube = cube * b.cube;
            return r;
        }
        
        //MatrixView<T> operator[](size_t row) { return MatrixView<T>(&matrix,row); }
        
        Cube<T> hadamard(const Cube<T> & c) {
            Cube<T> r(rows(),cols(),depth());
            assert(rows() == c.rows() && cols() == c.cols() && depth() == c.depth());
            cppmkl::vmul(size(),cube.data(),c.cube.data(),r.cube.data());
        }
        
        void zero() {
            memset(cube.data(),0x00,size()*sizeof(T));
        }
        void fill(T x) {
            for(size_t i = 0; i < size(); i++) cube[i] = x;
        }
        void ones() {
            fill((T)1);
        }
        void random(T min = T(0), T max = T(1)) {
            StdRandomUniform r;
            for(size_t i = 0; i < size(); i++) cube[i] = r.random(min,max);
        }
        Vector<T>& get_cube() { return cube; }

    };

    template<typename T>
    struct RealFFT1D
    {
        DFTI_DESCRIPTOR_HANDLE handle1;        
        size_t size;
        
        RealFFT1D(size_t size) {
            DFTI_CONFIG_UE prec;
            if(typeid(T) == typeid(float)) prec = DFTI_SINGLE;
            else prec = DFTI_DOUBLE;
            DftiCreateDescriptor(&handle1, prec, DFTI_REAL,  1, size );
            DftiSetue(handle1, DFTI_PLACEMENT, DFTI_NOT_INPLACE); //Out of place FFT
            DftiSetue(handle1, DFTI_BACKWARD_SCALE, 1.0f / size);
            DftiCommitDescriptor(handle1);            
            this->size = size;
        }
        ~RealFFT1D() {
            DftiFreeDescriptor(&handle1);            
        }

        void Forward( Vector<T> & input, Vector<std::complex<T>> & output) {
            output.resize(size);
            Vector<float> x(size*2);            
            DftiComputeForward(handle1, input.vector.data(),x.vector.data());
            memcpy(output.data(),x.data(), x.size()*sizeof(float));            
        }
        void Backward( Vector<std::complex<T>> & input, Vector<T> & output) {
            output.resize(size);
            Vector<float> x(size*2);            
            memcpy(x.data(),input.data(),x.size()*sizeof(float));
            DftiComputeBackward(handle1, x.vector.data(), output.vector.data());
        }                
    };

    template<typename T = float>
    struct ComplexFFT1D
    {
        DFTI_DESCRIPTOR_HANDLE handle1;        
        size_t size;
        
        ComplexFFT1D(size_t size) {
            DFTI_CONFIG_UE prec;
            if(typeid(T) == typeid(float)) prec = DFTI_SINGLE;
            else prec = DFTI_DOUBLE;
            DftiCreateDescriptor(&handle1, prec, DFTI_COMPLEX, 1, size );
            DftiSetue(handle1, DFTI_PLACEMENT, DFTI_NOT_INPLACE); //Out of place FFT
            DftiCommitDescriptor(handle1);            
            this->size = size;
        }
        ~ComplexFFT1D() {
            DftiFreeDescriptor(&handle1);            
        }

        void Forward( Vector<std::complex<T>> & input, Vector<std::complex<T>> & output) {
            output.resize(size);
            DftiComputeForward(handle1, input.vector.data(),output.vector.data());
        }
        void Backward( Vector<std::complex<T>> & input, Vector<std::complex<T>> & output) {
            output.resize(size);
            DftiComputeBackward(handle1, input.vector.data(), output.vector.data());
        }        
    };

    template<typename T>
    T& MatrixView<T>::operator[](size_t i) {
        return matrix->matrix[row*matrix->N + i];
    }

    template<typename T>    
    T  MatrixView<T>::__getitem(size_t i) {
        return matrix->matrix[row*matrix->N + i];
    }

    template<typename T>    
    void MatrixView<T>::__setitem(size_t i, T )
    {
        matrix->matrix[row*matrix->N + i] = ;
    }

    template<typename T>
    Matrix<T> matmul(const Matrix<T> & a, const Matrix<T> & b)
    {
        Matrix<T> r = a * b;
        return r;
    }

    template<typename T>
    Vector<T> sqr(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsqr(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> abs(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vabs(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> inv(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vinv(a.vector.data(),r.vector.data());
        return r;            
    }
    template<typename T>
    Vector<T> sqrt(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsqrt(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> rsqrt(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vinvsqrt(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> cbrt(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcbrt(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> rcbrt(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vinvcbrt(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> pow(const Vector<T> & a,const Vector<T> & b) {
        Vector<T> r(a.size());
        cppmkl::vpow(a.vector.data(),b.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> pow2o3(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vpow2o3(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> pow3o2(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vpow3o2(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> pow(const Vector<T> & a,const T b) {
        Vector<T> r(a.size());
        cppmkl::vpowx(a.vector.data(),b,r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> hypot(const Vector<T> & a,const T b) {
        Vector<T> r(a.size());
        cppmkl::vhypot(a.vector.data(),b,r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> exp(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vexp(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> exp2(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vexp2(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> exp10(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vexp10(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> expm1(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vexpm1(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> ln(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vln(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> log10(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vlog10(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> log2(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vlog2(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> logb(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vlogb(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> log1p(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vlog1p(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> cos(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcos(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> sin(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsin(a.vector.data(),r.vector.data());
        return r;            
    }
    template<typename T>
    Vector<T> tan(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vtan(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> cosh(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcosh(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> sinh(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsinh(a.vector.data(),r.vector.data());
        return r;            
    }
    template<typename T>
    Vector<T> tanh(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vtanh(a.vector.data(),r.vector.data());
        return r;            
    }
    template<typename T>
    Vector<T> acos(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vacos(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> asin(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vasin(a.vector.data(),r.vector.data());
        return r;            
    }
    template<typename T>
    Vector<T> atan(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vatan(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> atan2(const Vector<T> & a,const Vector<T> &n) {
        Vector<T> r(a.size());
        cppmkl::vatan2(a.vector.data(),n.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> acosh(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vacosh(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> asinh(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vasinh(a.vector.data(),r.vector.data());
        return r;            
    }
    template<typename T>
    Vector<T> atanh(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vatanh(a.vector.data(),r.vector.data());
        return r;
    }        
    template<typename T>
    Vector<T> sincos(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsincos(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> erf(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::verf(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> erfinv(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::verfinv(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> erfc(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::verfc(a.vector.data(),r.vector.data());
        return r;
    }
    template<typename T>
    Vector<T> cdfnorm(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcdfnorm(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> cdfnorminv(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcdfnorm(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> floor(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vfloor(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> ceil(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vceil(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> trunc(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vtrunc(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> round(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vround(a.vector.data(),r.vector.data());
        return r;        
    }    
    template<typename T>
    Vector<T> nearbyint(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vnearbyint(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> rint(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vrint(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> fmod(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vmodf(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> mulbyconj(const Vector<T> & a, const Vector<T> & b) {
        Vector<T> r(a.size());
        cppmkl::vmulbyconj(a.vector.data(),b.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> conj(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vconj(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> arg(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::varg(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> CIS(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vCIS(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> cospi(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcospi(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> sinpi(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsinpi(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> tanpi(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vtanpi(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> acospi(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vacospi(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> asinpi(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vasinpi(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> atanpi(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vatanpi(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> atan2pi(const Vector<T> & a, const Vector<T> & b) {
        Vector<T> r(a.size());
        cppmkl::vatanpi(a.vector.data(),b.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> cosd(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vcosd(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> sind(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vsind(a.vector.data(),r.vector.data());
        return r;
    }    
    template<typename T>
    Vector<T> tand(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vtand(a.vector.data(),r.vector.data());
        return r;
    }       
    template<typename T>
    Vector<T> lgamma(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vlgamma(a.vector.data(),r.vector.data());
        return r;
    }       
    template<typename T>
    Vector<T> tgamma(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vtgamma(a.vector.data(),r.vector.data());
        return r;
    }       
    template<typename T>
    Vector<T> expint1(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::vexpint1(a.vector.data(),r.vector.data());
        return r;
    }       
    template<typename T>
    Matrix<T> sqr(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsqr(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> abs(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vabs(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> inv(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vinv(a.matrix.data(),r.matrix.data());
        return r;            
    }
    template<typename T>
    Matrix<T> sqrt(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsqrt(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> rsqrt(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vinvsqrt(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> cbrt(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcbrt(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> rcbrt(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vinvcbrt(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> pow(const Matrix<T> & a,const Matrix<T> & b) {
        Matrix<T> r(a.size());
        cppmkl::vpow(a.matrix.data(),b.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> pow2o3(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vpow2o3(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> pow3o2(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vpow3o2(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> pow(const Matrix<T> & a,const T b) {
        Matrix<T> r(a.size());
        cppmkl::vpowx(a.matrix.data(),b,r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> hypot(const Matrix<T> & a,const T b) {
        Matrix<T> r(a.size());
        cppmkl::vhypot(a.matrix.data(),b,r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> exp(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vexp(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> exp2(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vexp2(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> exp10(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vexp10(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> expm1(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vexpm1(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> ln(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vln(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> log10(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vlog10(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> log2(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vlog2(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> logb(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vlogb(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> log1p(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vlog1p(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> cos(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcos(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> sin(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsin(a.matrix.data(),r.matrix.data());
        return r;            
    }
    template<typename T>
    Matrix<T> tan(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vtan(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> cosh(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcosh(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> sinh(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsinh(a.matrix.data(),r.matrix.data());
        return r;            
    }
    template<typename T>
    Matrix<T> tanh(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vtanh(a.matrix.data(),r.matrix.data());
        return r;            
    }
    template<typename T>
    Matrix<T> acos(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vacos(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> asin(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vasin(a.matrix.data(),r.matrix.data());
        return r;            
    }
    template<typename T>
    Matrix<T> atan(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vatan(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> atan2(const Matrix<T> & a,const Matrix<T> &n) {
        Matrix<T> r(a.size());
        cppmkl::vatan2(a.matrix.data(),n.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> acosh(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vacosh(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> asinh(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vasinh(a.matrix.data(),r.matrix.data());
        return r;            
    }
    template<typename T>
    Matrix<T> atanh(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vatanh(a.matrix.data(),r.matrix.data());
        return r;
    }        
    template<typename T>
    Matrix<T> sincos(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsincos(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> erf(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::verf(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> erfinv(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::verfinv(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> erfc(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::verfc(a.matrix.data(),r.matrix.data());
        return r;
    }
    template<typename T>
    Matrix<T> cdfnorm(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcdfnorm(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> cdfnorminv(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcdfnorm(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> floor(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vfloor(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> ceil(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vceil(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> trunc(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vtrunc(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> round(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vround(a.matrix.data(),r.matrix.data());
        return r;        
    }    
    template<typename T>
    Matrix<T> nearbyint(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vnearbyint(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> rint(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vrint(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> fmod(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vmodf(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> mulbyconj(const Matrix<T> & a, const Matrix<T> & b) {
        Matrix<T> r(a.size());
        cppmkl::vmulbyconj(a.matrix.data(),b.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> conj(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vconj(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> arg(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::varg(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> CIS(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vCIS(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> cospi(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcospi(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> sinpi(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsinpi(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> tanpi(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vtanpi(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> acospi(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vacospi(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> asinpi(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vasinpi(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> atanpi(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vatanpi(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> atan2pi(const Matrix<T> & a, const Matrix<T> & b) {
        Matrix<T> r(a.size());
        cppmkl::vatanpi(a.matrix.data(),b.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> cosd(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vcosd(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> sind(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vsind(a.matrix.data(),r.matrix.data());
        return r;
    }    
    template<typename T>
    Matrix<T> tand(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vtand(a.matrix.data(),r.matrix.data());
        return r;
    }       
    template<typename T>
    Matrix<T> lgamma(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vlgamma(a.matrix.data(),r.matrix.data());
        return r;
    }       
    template<typename T>
    Matrix<T> tgamma(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vtgamma(a.matrix.data(),r.matrix.data());
        return r;
    }       
    template<typename T>
    Matrix<T> expint1(const Matrix<T> & a) {
        Matrix<T> r(a.size());
        cppmkl::vexpint1(a.matrix.data(),r.matrix.data());
        return r;
    }       

    template<typename T>
    Cube<T> sqr(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsqr(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> abs(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vabs(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> inv(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vinv(a.cube.data(),r.cube.data());
        return r;            
    }
    template<typename T>
    Cube<T> sqrt(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsqrt(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> rsqrt(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vinvsqrt(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> cbrt(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcbrt(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> rcbrt(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vinvcbrt(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> pow(const Cube<T> & a,const Cube<T> & b) {
        Cube<T> r(a.size());
        cppmkl::vpow(a.cube.data(),b.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> pow2o3(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vpow2o3(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> pow3o2(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vpow3o2(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> pow(const Cube<T> & a,const T b) {
        Cube<T> r(a.size());
        cppmkl::vpowx(a.cube.data(),b,r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> hypot(const Cube<T> & a,const T b) {
        Cube<T> r(a.size());
        cppmkl::vhypot(a.cube.data(),b,r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> exp(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vexp(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> exp2(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vexp2(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> exp10(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vexp10(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> expm1(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vexpm1(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> ln(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vln(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> log10(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vlog10(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> log2(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vlog2(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> logb(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vlogb(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> log1p(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vlog1p(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> cos(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcos(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> sin(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsin(a.cube.data(),r.cube.data());
        return r;            
    }
    template<typename T>
    Cube<T> tan(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vtan(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> cosh(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcosh(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> sinh(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsinh(a.cube.data(),r.cube.data());
        return r;            
    }
    template<typename T>
    Cube<T> tanh(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vtanh(a.cube.data(),r.cube.data());
        return r;            
    }
    template<typename T>
    Cube<T> acos(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vacos(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> asin(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vasin(a.cube.data(),r.cube.data());
        return r;            
    }
    template<typename T>
    Cube<T> atan(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vatan(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> atan2(const Cube<T> & a,const Cube<T> &n) {
        Cube<T> r(a.size());
        cppmkl::vatan2(a.cube.data(),n.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> acosh(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vacosh(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> asinh(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vasinh(a.cube.data(),r.cube.data());
        return r;            
    }
    template<typename T>
    Cube<T> atanh(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vatanh(a.cube.data(),r.cube.data());
        return r;
    }        
    template<typename T>
    Cube<T> sincos(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsincos(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> erf(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::verf(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> erfinv(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::verfinv(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> erfc(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::verfc(a.cube.data(),r.cube.data());
        return r;
    }
    template<typename T>
    Cube<T> cdfnorm(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcdfnorm(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> cdfnorminv(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcdfnorm(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> floor(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vfloor(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> ceil(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vceil(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> trunc(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vtrunc(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> round(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vround(a.cube.data(),r.cube.data());
        return r;        
    }    
    template<typename T>
    Cube<T> nearbyint(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vnearbyint(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> rint(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vrint(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> fmod(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vmodf(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> mulbyconj(const Cube<T> & a, const Cube<T> & b) {
        Cube<T> r(a.size());
        cppmkl::vmulbyconj(a.cube.data(),b.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> conj(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vconj(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> arg(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::varg(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> CIS(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vCIS(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> cospi(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcospi(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> sinpi(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsinpi(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> tanpi(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vtanpi(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> acospi(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vacospi(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> asinpi(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vasinpi(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> atanpi(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vatanpi(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> atan2pi(const Cube<T> & a, const Cube<T> & b) {
        Cube<T> r(a.size());
        cppmkl::vatanpi(a.cube.data(),b.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> cosd(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vcosd(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> sind(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vsind(a.cube.data(),r.cube.data());
        return r;
    }    
    template<typename T>
    Cube<T> tand(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vtand(a.cube.data(),r.cube.data());
        return r;
    }       
    template<typename T>
    Cube<T> lgamma(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vlgamma(a.cube.data(),r.cube.data());
        return r;
    }       
    template<typename T>
    Cube<T> tgamma(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vtgamma(a.cube.data(),r.cube.data());
        return r;
    }       
    template<typename T>
    Cube<T> expint1(const Cube<T> & a) {
        Cube<T> r(a.size());
        cppmkl::vexpint1(a.cube.data(),r.cube.data());
        return r;
    }       

    template<typename T>
    Vector<T> copy(const Vector<T> & a) {
        Vector<T> r(a.size());
        cppmkl::cublas_copy(a.size(),a.vector.data(),1,r.vector.data(),1);
        return r;
    }       

    template<typename T> sum(const Vector<T> & a) {        
        return cppmkl::cublas_sum(a.size(), a.vector.data(),1);
        return r;
    }       

    template<typename T>
    Vector<T> add(const Vector<T> & a, const Vector<T> & b) {
        Vector<T> r(b);
        cppmkl::cublas_sxpy(a.size(),1.0,a.vector.data(),1,r.vector.data(),1);
        return r;
    }       
    template<typename T>
    Vector<T> sub(const Vector<T> & a, const Vector<T> & b) {
        Vector<T> r(a);
        cppmkl::cublas_sxpy(a.size(),-1.0,b.vector.data(),1,r.vector.data(),1);
        return r;
    }       
    template<typename T>
    T dot(const Vector<T> & a, const Vector<T> & b) {
        return cppmkl::cublas_dot(a.size(),a.vector.data(),b.vector.data());
    }       
    template<typename T>
    T nrm2(const Vector<T> & a) {
        Vector<T> r(b);
        return cppmkl::cublas_nrm2(a.size(),a.vector.data());        
    }       
    
    template<typename T>
    void scale(Vector<T> & x, T alpha) {
        cblas_scal(x.size(),alpha,x.vector.data(),1);
    }


    template<typename T>
    size_t min_index(const Matrix<T> & m) { return cppmkl::cblas_iamin(m.size(),m.matrix.data(),1); }
    template<typename T>
    size_t max_index(const Matrix<T> & m) { return cppmkl::cblas_iamax(m.size(),m.matrix.data(),1); }
    
    template<typename T>
    size_t min_index(const Vector<T> & v) { return cppmkl::cblas_iamin(v.size(),v.vector.data(),1); }
    template<typename T>
    size_t max_index(const Vector<T> & v) { return cppmkl::cblas_iamax(v.size(),v.vector.data(),1); }

    template<typename T>
    Vector<T> linspace(size_t n, T start, T inc=1) {
        Vector<T> r(n);
        for(size_t i = 0; i < n; i++) {
            r[i] = start + i*inc;
        }
    }
    template<typename T>
    Vector<T> linspace(T start, T end, T inc=1) {
        size_t n = (end - start)/inc;
        Vector<T> r(n);
        for(size_t i = 0; i < n; i++) {
            r[i] = start + i*inc;
        }
    }
}