#pragma once 

#include <Eigen/Eigen>
#include <vector>
#include <map>
#include <iostream> 
#include <cstdint>
#include <complex>
#include <stack>
#include <cmath>
#include <cfloat>

//gt#include "cuda_runtime.h"
//#define FLT_MIN 1.17549435e-38

namespace SimpleEigen {
    template<typename T> struct RowVector;
    template<typename T> struct ColVector;
    template<typename T> struct Matrix;
    template<typename T> struct SparseRowVector;
    template<typename T> struct SparseColVector;
    template<typename T> struct SparseMatrix;
    template<typename T> struct Array;
    template<typename T> struct Array2D;
}

#include "Base.h"
#include "Complex.h"
#include "Scalar.h"
#include "RowVector.h"
#include "ColVector.h"
#include "Matrix.h"
#include "ColMatrix.h"
#include "Array.h"
#include "Array2D.h"
#include "SparseRowVector.h"
#include "SparseColVector.h"
#include "SparseMatrix.h"

namespace SimpleEigen {





template<typename T>
RowVector<T> RowVector<T>::operator * (Matrix<T> &b) 
{ 
    RowVector<T> r;        
    r.vector =  b.matrix * vector;
    return r;
}

template<typename T>
Matrix<T> RowVector<T>::operator + (Matrix<T> &b) 
{     
    Matrix<T> r;        
    for(size_t i = 0; i < b.rows(); i++)
        for(size_t j = 0; j < b.cols(); j++)
            r.matrix(i,j) = b.matrix(i,j) + vector(j);
    return r;
}

template<typename T>
Matrix<T> RowVector<T>::operator - (Matrix<T> &b) 
{     
    Matrix<T> r;        
    for(size_t i = 0; i < b.rows(); i++)
        for(size_t j = 0; j < b.cols(); j++)
            r.matrix(i,j) = b.matrix(i,j) - vector(j);
    return r;
}

template<typename T>
Matrix<T> RowVector<T>::operator / (Matrix<T> &b) 
{     
    Matrix<T> r;        
    for(size_t i = 0; i < b.rows(); i++)
        for(size_t j = 0; j < b.cols(); j++)
            r.matrix(i,j) = b.matrix(i,j) / vector(j);
    return r;
}




template<typename T>
Matrix<T> RowVector<T>::matrix() { return Matrix<T>(vector); }


template<typename T>
ColVector<T> ColVector<T>::operator * (Matrix<T> &b) 
{ 
    ColVector<T> r;        
    r.vector =  b.matrix * vector;
    return r;
}


template<typename T>
Matrix<T> ColVector<T>::operator + (Matrix<T> &b) 
{     
    Matrix<T> r;        
    for(size_t i = 0; i < b.rows(); i++)
        for(size_t j = 0; j < b.cols(); j++)
            r.matrix(i,j) = b.matrix(i,j) + vector(j);
    return r;
}

template<typename T>
Matrix<T> ColVector<T>::operator - (Matrix<T> &b) 
{     
    Matrix<T> r;        
    for(size_t i = 0; i < b.rows(); i++)
        for(size_t j = 0; j < b.cols(); j++)
            r.matrix(i,j) = b.matrix(i,j) - vector(j);
    return r;
}

template<typename T>
Matrix<T> ColVector<T>::operator / (Matrix<T> &b) 
{     
    Matrix<T> r;        
    for(size_t i = 0; i < b.rows(); i++)
        for(size_t j = 0; j < b.cols(); j++)
            r.matrix(i,j) = b.matrix(i,j) / vector(j);
    return r;
}


template<typename T>
Matrix<T> ColVector<T>::matrix() { return Matrix<T>(vector); }


template<typename T>
RowVector<T> Scalar<T>::operator * (const RowVector<T> & v)
{
    RowVector<T> r;
    r.vector = v.vector * val;
    return r;
}
template<typename T>
RowVector<T> Scalar<T>::operator / (const RowVector<T> & v)
{
    RowVector<T> r;
    r.vector = v.vector / val;
    return r;
}

template<typename T>
RowVector<T> Scalar<T>::operator + (const RowVector<T> & v)
{
    RowVector<T> r;
    r.vector = v.vector + rowvector(v).vector;
    return r;
}

template<typename T>
RowVector<T> Scalar<T>::operator - (const RowVector<T> & v)
{
    RowVector<T> r;
    r.vector = v.vector - rowvector(v).vector;
    return r;
}


template<typename T>
ColVector<T> Scalar<T>::operator * (const ColVector<T> & v)
{
    ColVector<T> r;
    r.vector = v.vector * val;
    return r;
}


template<typename T>
ColVector<T> Scalar<T>::operator / (const ColVector<T> & v)
{
    ColVector<T> r;
    r.vector = v.vector / val;
    return r;
}

template<typename T>
ColVector<T> Scalar<T>::operator + (const ColVector<T> & v)
{
    ColVector<T> r;
    r.vector = v.vector + colvector(v).vector;
    return r;
}

template<typename T>
ColVector<T> Scalar<T>::operator - (const ColVector<T> & v)
{
    ColVector<T> r;
    r.vector = v.vector - colvector(v).vector;
    return r;
}


template<typename T>
Matrix<T> Scalar<T>::operator * (Matrix<T> & m)
{
    Matrix<T> r(m.rows(),m.cols(),0);
    r.matrix = m.matrix * val;
    return r;
}

template<typename T>
Matrix<T> Scalar<T>::operator / (Matrix<T> & m)
{
    Matrix<T> r(m.rows(),m.cols(),0);
    r.matrix = m.matrix / val;
    return r;
}

template<typename T>
Matrix<T> Scalar<T>::operator + (Matrix<T> & m)
{
    Matrix<T> r(m.rows(),m.cols(),0);
    for(size_t i = 0; i < m.rows(); i++)
        for(size_t j = 0; j < m.cols(); j++)
            r.matrix(i,j) = m.matrix(i,j) + val;
    return r;
}

template<typename T>
Matrix<T> Scalar<T>::operator - (Matrix<T> & m)
{
    Matrix<T> r(m.rows(),m.cols(),0);
    for(size_t i = 0; i < m.rows(); i++)
        for(size_t j = 0; j < m.cols(); j++)
            r.matrix(i,j) = m.matrix(i,j) - val;
    return r;
}


template<typename T>
Array<T> Scalar<T>::operator * (Array<T> & v)
{
    Array<T> a = v* array(v).array;
    return a;
}
template<typename T>
Array<T> Scalar<T>::operator / (Array<T> & v)
{
    Array<T> a = v/array(v).array;
    return a;
}
template<typename T>
Array<T> Scalar<T>::operator + (Array<T> & v)
{
    Array<T> a = v + array(v).array;
    return a;
}
template<typename T>
Array<T> Scalar<T>::operator - (Array<T> & v)
{
    Array<T> a = v - array(v).array;
    return a;
}


template<typename T>
RowVector<T> Scalar<T>::rowvector(const RowVector<T> & v)
{
    RowVector<T> r = RowVector<T>(v.size(),val);
    return r;
}
template<typename T>
ColVector<T> Scalar<T>::colvector(const ColVector<T> & v)
{
    ColVector<T> r = ColVector<T>(v.size(),val);
    return r;
}
template<typename T>
Matrix<T> Scalar<T>::matrix(Matrix<T> & m)
{
    Matrix<T> r = Matrix<T>(m.rows(),m.cols(),val);
    return r;
}
template<typename T>
Array<T> Scalar<T>::array(Array<T> & a)
{
    Array<T> r = Array<T>(a.size(),val);
    return r;
}
template<typename T>
Array2D<T> Scalar<T>::array2d(Array2D<T> & a)
{
    Array2D<T> r = Array2D<T>(a.rows(),a.cols(),val);
    return r;
}


template<typename T>
RowVector<T> Scalar<T>::rowvector(size_t size)
{
    RowVector<T> r = RowVector<T>(size,val);
    return r;
}
template<typename T>
ColVector<T> Scalar<T>::colvector(size_t size)
{
    ColVector<T> r = ColVector<T>(size,val);
    return r;
}
template<typename T>
Matrix<T> Scalar<T>::matrix(size_t r, size_t c)
{
    Matrix<T> ret = Matrix<T>(r,c,val);
    return ret;
}
template<typename T>
Array<T> Scalar<T>::array(size_t size)
{
    Array<T> r = Array<T>(size,val);
    return r;
}
template<typename T>
Array2D<T> Scalar<T>::array2d(size_t r, size_t c)
{
    Array2D<T> ret = Array2D<T>(r,c,val);
    return ret;
}

template<typename T>
RowVector<T> rowvector(RowVector<T> & v, const T & val)
{
    RowVector<T> r = RowVector<T>(v.size(),val);
    return r;
}
template<typename T>
ColVector<T> colvector(ColVector<T> & v, const T & val)
{
    ColVector<T> r = ColVector<T>(v.size(),val);
    return r;
}
template<typename T>
Matrix<T> matrix(Matrix<T> & m, const T & val)
{
    Matrix<T> r = Matrix<T>(m.rows(),m.cols(),val);
    return r;
}
template<typename T>
Array<T> array(Array<T> & a, const T & val)
{
    Array<T> r = Array<T>(a.size(),val);
    return r;
}


}


