#pragma once 

namespace SimpleEigen {
template<typename T>
using EigenArray2D = Eigen::Array<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>;

template<class T>
struct Array2D : public Base
{
    EigenArray2D<T> array;

    Array2D() {} 
    Array2D(int i, int j) { array = EigenArray2D<T>(i,j); }
    Array2D(int i, int j, const T & val) { 
        array = EigenArray2D<T>(i,j); 
        array.fill(val);
    }
    Array2D(const Array2D<T> & a) { array = a.array; }
    Array2D(const EigenArray2D<T> & a) { array = a; }
    
    Array2D<T>& operator = (const Array2D<T> & a) {
        array = a.array;
        return *this;
    }
    
    T& operator()(size_t r, size_t c) { return array(r,c); }
    
    size_t size() const { return array.size(); }  
    size_t rows() const { return array.rows(); }
    size_t cols() const { return array.cols(); }

    void random(int i,int j) { array = EigenArray2D<T>::Random(i,j); }
    void random() { array.setRandom(); }    
    void fill(T v) { array.fill(v); }
    
    void resize(size_t i, size_t j) { array.resize(i,j); }
        
    
    /* 
    void map(std::vector<T> & a)
    {
        array.resize(array.rows()*array.cols());
        for(size_t i = 0; i < array.rows(); i++)
            for(size_t j = 0; j < array.cols(); j++)
                a[i*array.cols()+j] = array(i,j);
    }
    void unmap(std::vector<T> & a)
    {
        a.resize(array.rows()*array.cols());
        for(size_t i = 0; i < array.rows(); i++)
            for(size_t j = 0; j < array.cols(); j++)
                array(i,j) = a[i*array.cols()+j];
    }
    */
    Array2D<T> operator + (const Array2D<T>& b) 
    { 
        Array2D<T> r(*this);
        r.array = r.array + b.array;
        return r;
    }    
    Array2D<T> operator + (Scalar<T> & b) 
    { 
        Array2D<T> r(*this);
        r.array = r.array + b.val;
        return r;
    }    
    Array2D<T> operator - (const Array2D<T>& b) 
    { 
        Array2D<T> r(*this);
        r.array = r.array - b.array;
        return r;
    }
    Array2D<T> operator - (Scalar<T>& b) 
    { 
        Array2D<T> r(*this);
        r.array = r.array - b.val;
        return r;
    }
    Array2D<T> operator * (const Array2D<T>& b) 
    { 
        Array2D<T> r(*this);
        r.array = r.array * b.array;
        return r;
    }
    Array2D<T> operator * (Scalar<T>& b) 
    { 
        Array2D<T> r(*this);
        r.array = r.array * b.val;
        return r;
    }
    Array2D<T> operator / (const Array2D<T>& b)  
    { 
        Array2D<T> r(*this);
        r.array = r.array / b.array;
        return r;
    }
    Array2D<T> operator / (Scalar<T>& b)  
    { 
        Array2D<T> r(*this);
        r.array = r.array / b.val;
        return r;
    }

    Matrix<T> to_matrix() { 
        Matrix<T> m; 
        m.matrix=array.matrix(); 
        return m; 
    }

    void from_matrix(const Matrix<T> & m) { array = m.matrix.array(); }

    void print() 
    {
        std::cout << array << std::endl;
    }
};

template<typename T>
Array2D<T> abs(const Array2D<T> & array) { return Array2D<T>(array.array.abs()); }
template<typename T>
Array2D<T> inverse(const Array2D<T> & array) { return Array2D<T>(array.array.inverse());   }
template<typename T>
Array2D<T> exp(const Array2D<T> & array) { return Array2D<T>(array.array.exp());   }
template<typename T>
Array2D<T> log(const Array2D<T> & array) { return Array2D<T>(array.array.log());   }
template<typename T>
Array2D<T> log1p(const Array2D<T> & array) { return Array2D<T>(array.array.log1p());  }
template<typename T>
Array2D<T> log10(const Array2D<T> & array) { return Array2D<T>(array.array.log10());  }
template<typename T>
Array2D<T> pow(const Array2D<T> & array,const T& b) { return Array2D<T>(array.array.pow(b));  }
template<typename T>
Array2D<T> sqrt(const Array2D<T> & array) { return Array2D<T>(array.array.sqrt());  }
template<typename T>
Array2D<T> rsqrt(const Array2D<T> & array) { return Array2D<T>(array.array.rsqrt());  }
template<typename T>
Array2D<T> square(const Array2D<T> & array) { return Array2D<T>(array.array.square());  }
template<typename T>
Array2D<T> cube(const Array2D<T> & array) { return Array2D<T>(array.array.cube());  }
template<typename T>
Array2D<T> abs2(const Array2D<T> & array) { return Array2D<T>(array.array.abs2());  }
template<typename T>
Array2D<T> sin(const Array2D<T> & array) { return Array2D<T>(array.array.sin());  }
template<typename T>
Array2D<T> cos(const Array2D<T> & array) { return Array2D<T>(array.array.cos());  }
template<typename T>
Array2D<T> tan(const Array2D<T> & array) { return Array2D<T>(array.array.tan()); }
template<typename T>
Array2D<T> asin(const Array2D<T> & array) { return Array2D<T>(array.array.asin());  }
template<typename T>
Array2D<T> acos(const Array2D<T> & array) { return Array2D<T>(array.array.acos());  }
template<typename T>
Array2D<T> atan(const Array2D<T> & array) { return Array2D<T>(array.array.atan());  }
template<typename T>
Array2D<T> sinh(const Array2D<T> & array) { return Array2D<T>(array.array.sinh());  }
template<typename T>
Array2D<T> cosh(const Array2D<T> & array) { return Array2D<T>(array.array.cosh());  }
template<typename T>
Array2D<T> tanh(const Array2D<T> & array) { return Array2D<T>(array.array.tanh());  }
template<typename T>
Array2D<T> ceil(const Array2D<T> & array) { return Array2D<T>(array.array.ceil());  }
template<typename T>
Array2D<T> floor(const Array2D<T> & array) { return Array2D<T>(array.array.floor());  }
template<typename T>
Array2D<T> round(const Array2D<T> & array) { return Array2D<T>(array.array.round());  }

template<typename T> Array2D<T> asinh(const Array2D<T> & array) { return Array2D<T>(array.array.asinh()); }
template<typename T> Array2D<T> acosh(const Array2D<T> & array) { return Array2D<T>(array.array.acosh()); }
template<typename T> Array2D<T> atanh(const Array2D<T> & array) { return Array2D<T>(array.array.atanh()); }
template<typename T> Array2D<T> rint(const Array2D<T> & array)  { return Array2D<T>(array.array.rint());  }

template<typename T>
size_t size(const Array2D<T> & array) { return array.size(); }  

template<typename T>
void println(Array2D<T> & array) { array.print(); }  

template<typename T>
size_t rows(const Array2D<T> & array) { return array.rows(); }

template<typename T>
size_t cols(const Array2D<T> & array) { return array.cols(); }

template<typename T>
void random(int i,int j,Array2D<T> & array) { array.random(i,j); }

template<typename T>
void random(Array2D<T> & array) { array.random(); }    

template<typename T>
void fill(Array2D<T> & array,T v) { array.fill(v); }
    
template<typename T>
void resize(Array2D<T> & array, size_t i, size_t j) { array.resize(i,j); }
    

template<typename T>
Array2D<T> operator + (T a, Array2D<T> & b) {
    Array2D<T> r(b);
    r.array = a + b.array;
    return r;
}
template<typename T>
Array2D<T> operator - (T a, Array2D<T> & b) {
    Array2D<T> r(b);
    r.array = a - b.array;
    return r;
}
template<typename T>
Array2D<T> operator * (T a, Array2D<T> & b) {
    Array2D<T> r(b);
    r.array = a * b.array;
    return r;
}

template<typename T>
Array2D<T> scalar_array2d(size_t n, T v) {
    Array2D<T> r(n);
    r.fill(v);
    return r;
}
}