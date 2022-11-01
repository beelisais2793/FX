#pragma once 

namespace SimpleEigen {

template<typename T>
using EigenArray = Eigen::Array<T,Eigen::Dynamic,Eigen::RowMajor>;

template<class T>
struct Array : public Base
{
    EigenArray<T> array;

    Array() {} 
    Array(int size) { array = EigenArray<T>(size); }
    Array(int size, const T & val) { 
        array = EigenArray<T>(size); 
        array.fill(val);
    }
    Array(const Array<T> & a) { array = a.array; }
    Array(const EigenArray<T> & a) { array = a; }
    
    Array<T>& operator = (const Array<T> & a) {
        array = a.array;
        return *this;
    }
    T& operator()(size_t i) { return array(i); }
    T& operator[](size_t i) { return array[i];}
    void  __setitem(size_t i, T v) { array[i] = v; }
    T __getitem(size_t i) { return array[i]; }

    size_t size() const { return array.size(); }  

    void    random(int i) { array = EigenArray<T>::Random(i); }
    void    random() { array.setRandom(); }    
    void    fill(T v) { array.fill(v); }
    size_t  cols() const { return array.cols(); }
        
        /*
    void map(std::vector<T> & a)
    {
        array.resize(a.size());
        for(size_t i = 0; i < a.size(); i++)
            array(i) = a[i];
    }
    void unmap(std::vector<T> & out)
    {
        out.resize(array.size());
        for(size_t i = 0; i < array.size(); i++)
            out[i] = array[i];        
    }
    */
    Array<T> operator + (const Array<T>& b) 
    { 
        Array<T> r(*this);
        r.array = r.array + b.array;
        return r;
    }    
    Array<T> operator + (Scalar<T> & b) 
    { 
        Array<T> r(*this);
        r.array = r.array + b.val;
        return r;
    }    
    Array<T> operator - (const Array<T>& b) 
    { 
        Array<T> r(*this);
        r.array = r.array - b.array;
        return r;
    }
    Array<T> operator - (Scalar<T>& b) 
    { 
        Array<T> r(*this);
        r.array = r.array - b.val;
        return r;
    }
    Array<T> operator * (const Array<T>& b) 
    { 
        Array<T> r(*this);
        r.array = r.array * b.array;
        return r;
    }
    Array<T> operator * (Scalar<T>& b) 
    { 
        Array<T> r(*this);
        r.array = r.array * b.val;
        return r;
    }
    Array<T> operator / (const Array<T>& b)  
    { 
        Array<T> r(*this);
        r.array = r.array / b.array;
        return r;
    }
    Array<T> operator / (Scalar<T>& b)  
    { 
        Array<T> r(*this);
        r.array = r.array / b.val;
        return r;
    }

    RowVector<T> get_row_vector() { return RowVector<T>(array.matrix()); }
    ColVector<T> get_col_vector() { return ColVector<T>(array.matrix()); }

    void print() 
    {
        std::cout << array << std::endl;
    }

    void resize(size_t size) { array.resize(size); }

};

template<typename T>
Array<T> abs(const Array<T> & array) { return Array<T>(array.array.abs()); }
template<typename T>
Array<T> inverse(const Array<T> & array) { return Array<T>(array.array.inverse());   }
template<typename T>
Array<T> exp(const Array<T> & array) { return Array<T>(array.array.exp());   }
template<typename T>
Array<T> log(const Array<T> & array) { return Array<T>(array.array.log());   }
template<typename T>
Array<T> log1p(const Array<T> & array) { return Array<T>(array.array.log1p());  }
template<typename T>
Array<T> log10(const Array<T> & array) { return Array<T>(array.array.log10());  }
template<typename T>
Array<T> pow(const Array<T> & array,const T& b) { return Array<T>(array.array.pow(b));  }
template<typename T>
Array<T> sqrt(const Array<T> & array) { return Array<T>(array.array.sqrt());  }
template<typename T>
Array<T> rsqrt(const Array<T> & array) { return Array<T>(array.array.rsqrt());  }
template<typename T>
Array<T> square(const Array<T> & array) { return Array<T>(array.array.square());  }
template<typename T>
Array<T> cube(const Array<T> & array) { return Array<T>(array.array.cube());  }
template<typename T>
Array<T> abs2(const Array<T> & array) { return Array<T>(array.array.abs2());  }
template<typename T>
Array<T> sin(const Array<T> & array) { return Array<T>(array.array.sin());  }
template<typename T>
Array<T> cos(const Array<T> & array) { return Array<T>(array.array.cos());  }
template<typename T>
Array<T> tan(const Array<T> & array) { return Array<T>(array.array.tan()); }
template<typename T>
Array<T> asin(const Array<T> & array) { return Array<T>(array.array.asin());  }
template<typename T>
Array<T> acos(const Array<T> & array) { return Array<T>(array.array.acos());  }
template<typename T>
Array<T> atan(const Array<T> & array) { return Array<T>(array.array.atan());  }
template<typename T>
Array<T> sinh(const Array<T> & array) { return Array<T>(array.array.sinh());  }
template<typename T>
Array<T> cosh(const Array<T> & array) { return Array<T>(array.array.cosh());  }
template<typename T>
Array<T> tanh(const Array<T> & array) { return Array<T>(array.array.tanh());  }    
template<typename T>
Array<T> ceil(const Array<T> & array) { return Array<T>(array.array.ceil());  }
template<typename T>
Array<T> floor(const Array<T> & array) { return Array<T>(array.array.floor());  }
template<typename T>
Array<T> round(const Array<T> & array) { return Array<T>(array.array.round());  }


template<typename T> Array<T> asinh(const Array<T> & array) { return Array<T>(array.array.asinh()); }
template<typename T> Array<T> acosh(const Array<T> & array) { return Array<T>(array.array.acosh()); }
template<typename T> Array<T> atanh(const Array<T> & array) { return Array<T>(array.array.atanh()); }
template<typename T> Array<T> rint(const Array<T> & array)  { return Array<T>(array.array.rint());  }

template<typename T>
size_t size(Array<T> & array) { return array.size(); }  
template<typename T>
void    random(int i, Array<T> & array) { array.random(i); }
template<typename T>
void    random(Array<T> & array) { array.random(); }    
template<typename T>
void    fill(Array<T> & array, T v) { array.fill(v); }
template<typename T>
size_t  cols(Array<T> & array) { return array.cols(); }

template<typename T>
Array<T> operator + (T a, Array<T> & b) {
    Array<T> r(b);
    r.array = a + b.array;
    return r;
}
template<typename T>
Array<T> operator - (T a, Array<T> & b) {
    Array<T> r(b);
    r.array = a - b.array;
    return r;
}
template<typename T>
Array<T> operator * (T a, Array<T> & b) {
    Array<T> r(b);
    r.array = a * b.array;
    return r;
}

template<typename T>
Array<T> scalar_array(size_t n, T v) {
    Array<T> r(n);
    r.fill(v);
    return r;
}

template<typename T>
RowVector<T> get_row_vector(Array<T> & array) { return RowVector<T>(array.array.matrix()); }

template<typename T>
ColVector<T> get_col_vector(Array<T> & array) { return ColVector<T>(array.array.matrix()); }

template<typename T>
void resize(Array<T> & a, size_t n) { a.resize(n); }

template<typename T>
void println(Array<T> & a) { a.print(); }

}