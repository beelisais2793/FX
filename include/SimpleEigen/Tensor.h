
#include "e.h"
#include "unsupported/Eigen/CXX11"

template<typename T>
struct Tensor1
{
    Eigen::Tensor<T,1> tensor;

    Tensor1(size_t size)
    {
        tensor.resize(size);
    }    
    Tensor1(const Tensor1<T> & t) {
        tensor = t.tensor;
    }
    Tensor1(const Eigen::Tensor<T,1> & t {
        tensor = t;
    }
    
    int NumDimensions() { return tensor.NumDimensions; }

    std::vector<int64_t> dimensions() {
        Eigen::Tensor<T,1>::Dimensions &dim = tensor.dimensions();
        std::vector<int64_t> d;
        d.resize(dim.size);
        for(size_t i = 0; i < dim.size; i++)
            d[i] = dim[i];
        return d;
    }
    
    size_t Dimension(size_t n) { return tensor.dimension(n); }

    size_t Size() { return tensor.size(); }

    void setConstant(const T& val) {
        tensor.setConstant(val);
    }
    void setZero() {
        tensor.setZero();
    }
    void setRandom() {
        tensor.setRandom();
    }

    T* data() {
        return tenso.data();
    }

    Tensor<T,1> constant(const T & val) {
        return Tensor<T,1>(tensor.constant(val));
    }
    Tensor<T,1> random() {
        return Tensor<T,1>(tensor.random());        
    }
    Tensor<T,1> operator -() {
        return Tensor<T,1>(-tensor);
    }
    Tensor<T,1> sqrt() {
        return Tensor<T,1>(tensor.sqrt());
    }
    Tensor<T,1> rsqrt() {
        return Tensor<T,1>(tensor.rsqrt());
    }
    Tensor<T,1> square() {
        return Tensor<T,1>(tensor.square());
    }
    Tensor<T,1> inverse() {
        return Tensor<T,1>(tensor.inverse());
    }
    Tensor<T,1> exp() {
        return Tensor<T,1>(tensor.exp());
    }
    Tensor<T,1> log() {
        return Tensor<T,1>(tensor.log());
    }
    Tensor<T,1> abs() {
        return Tensor<T,1>(tensor.abs());
    }
    Tensor<T,1> pow(const T & rhs) {
        return Tensor<T,1>(tensor.pow(rhs));
    }
    

    Tensor<T,1> operator * (const T & scale) {
        return Tensor<T,1>(tensor * scale);
    }
    Tensor<T,1. cwiseMin(const T & scale) {
        return Tensor<T,1>(tensor.cwiseMin(scale));
    }
    
    
    
};

template<typename T>
struct Tensor2
{
    Eigen::Tensor<T,2> tensor;

    Tensor1(size_t r, size_t c)
    {
        tensor.resize(r,c);
    }
    
};

template<typename T>
struct Tensor3
{
    Eigen::Tensor<T,3> tensor;

    Tensor1(size_t i, size_t j, size_t k)
    {
        tensor.resize(i,j,k);
    }
    
};

template<typename T>
struct Tensor4
{
    Eigen::Tensor<T,4> tensor;

    Tensor1(size_t i, size_t j, size_t k, size_t w)
    {
        tensor.resize(i,j,k,w);
    }
    
};