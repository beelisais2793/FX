#pragma once 

#include "datalogic.h"

namespace Std {
    // it is not compatible with vector
    struct BitArray : public StdObject
    {
        std::vector<bool> v;

        BitArray() {} 
        BitArray(size_t i) { v.resize(i); } 
        BitArray(const BitArray & a) { v = a.v; }

        BitArray& operator = (const BitArray & b) { v = b.v; return *this; }

        bool operator[](size_t i) { return v[i]; }
        void  set(size_t i, bool value) { v[i] = value; }
        bool     get(size_t i) { return v[i]; }
        void  resize(size_t i) { v.resize(i); }
        size_t size() { return v.size(); }
        size_t max_size() { return v.max_size(); }
        size_t capacity() { return v.capacity(); }
        bool   empty() { return v.empty(); }
        void reserve(size_t n) { v.reserve(n); }
        void shrink_to_fit() { v.shrink_to_fit(); }

        bool __getitem(size_t i) { return v[i]; }
        void __setitem(size_t i, const bool val) { v[i] = val; }


        BitArray& operator +(const BitArray& a)
        {
            v.insert(v.end(),a.v.begin(), a.v.end());
            return *this;
        }

        bool   at(size_t n) { return v.at(n); }
        bool   front() { return v.front(); }
        bool   back() { return v.back(); }

        void push_back(const bool val) { v.push_back(val); }
        void pop_back() { v.pop_back(); }

        ///void swap(Array<T> & x) { v.swap(x); }
        void clear() { v.clear(); }

        bool operator == (const BitArray & a) { return v == a.v; }
        bool operator != (const BitArray & a) { return v != a.v; }
        bool operator < (const BitArray & a) { return v < a.v; }
        bool operator > (const BitArray & a) { return v > a.v; }
        bool operator <= (const BitArray & a) { return v <= a.v; }
        bool operator >= (const BitArray & a) { return v >= a.v; }
    };


    template<class T>
    struct Array : public StdObject
    {

        std::vector<T> v;

        Array<T>() {} 
        Array<T>(size_t i) { v.resize(i); } 
        Array<T>(const Array<T> & a) { v = a.v; }
        Array<T>(const std::vector<T> & a) : v(a) {}
        Array<T>& operator = (const Array<T> & b) { v = b.v; return *this; }

        T& operator[](size_t i) { return v[i]; }
        void  set(size_t i, T value) { v[i] = value; }
        T     get(size_t i) { return v[i]; }
        void  resize(size_t i) { v.resize(i); }
        size_t size() { return v.size(); }
        size_t max_size() { return v.max_size(); }
        size_t capacity() { return v.capacity(); }
        bool   empty() { return v.empty(); }
        void reserve(size_t n) { v.reserve(n); }
        void shrink_to_fit() { v.shrink_to_fit(); }

        T __getitem(size_t i) { return v[i]; }
        void __setitem(size_t i, const T val) { v[i] = val; }
        
        Array<T> operator +(const Array<T>& a)    {
            Array<T> r;
            r.resize(v.size()+a.v.size());
            std::copy(v.begin(),v.end(),r.v.begin());
            r.v.insert(r.v.end(),a.v.begin(), a.v.end());
            return r;
        }

        T   at(size_t n) { return v.at(n); }
        T   front() { return v.front(); }
        T   back() { return v.back(); }
        T*  data() { return v.data(); }

        void insert(const Array<T> & a, size_t index)    {
            assert(index < size());
            v.insert(v.begin()+index,a.v.begin(),a.v.end());
        }
        void copy(Array<T> & r) { 
            r.v.clear();
            r.v.resize(v.size());
            std::copy(v.begin(),v.end(),r.v.begin());
        }
        void copy_backward(Array<T> & r) { 
            r.v.clear();
            r.v.resize(v.size());
            std::copy_backward(v.begin(),v.end(),r.v.begin());
        }
        void push_back(const T val) { v.push_back(val); }
        void pop_back() { v.pop_back(); }

        ///void swap(Array<T> & x) { v.swap(x); }
        void clear() { v.clear(); }

        /*
        bool operator == (const Array<T> & a) { return v == a.v; }
        bool operator != (const Array<T> & a) { return v != a.v; }
        bool operator < (const Array<T> & a) { return v < a.v; }
        bool operator > (const Array<T> & a) { return v > a.v; }
        bool operator <= (const Array<T> & a) { return v <= a.v; }
        bool operator >= (const Array<T> & a) { return v >= a.v; }
        */

        Array<T> slice(size_t p1)    {
            Array<T> r;
            r.resize(size()-p1);
            assert(p1 < size());        
            size_t x = 0;
            for(size_t i = p1; i < size(); i++) r[x++] = v[i];
            return r;
        }
        Array<T> slice(size_t p1, size_t p2)    {
            Array<T> r;
            r.resize(p2-p1);
            assert(p2 < size());
            assert(p1 < p2);
            size_t x = 0;
            for(size_t i = p1; i < p2; i++) r[x++] = v[i];
            return r;
        }
        Array<T> slice(size_t p1, size_t p2, size_t stride)    {
            Array<T> r;
            r.resize(p2-p1);
            assert(p2 < size());
            assert(p1 < p2);
            size_t x = 0;
            for(size_t i = p1; i < p2; i+=stride) r[x++] = v[i];
            return r;
        }
        void erase(size_t pos) { assert(pos < size()); v.erase(v.begin()+pos); }
        void erase(size_t p1, size_t p2) {
            assert(p1 < p2);
            assert(p2 < size());
            for(size_t i = p1; i < p2; i++) erase(i);
        }

        /* 
        void remove(const T& val)    {
            std::remove(v.begin(),v.end(),val);
        }
        void remove(const T& val,size_t p1, size_t p2)    {
            assert(p1 < p2);
            assert(p2 < size());
            std::remove(v.begin()+p1,v.begin()+p2,val);
        }
        void remove(const T& val, size_t p1)    {
            assert(p1 < size());
            std::remove(v.begin()+p1,v.end(),val);
        }
        */
        

        /*                
        void   reverse() { std::reverse(v.begin(), v.end()); }
        int count(const T val) { return std::count(v.begin(),v.end(),val); }            
        void fill(size_t i1, size_t i2,const T& val)    {
            assert(i1 < i2);
            std::fill(v.begin()+i1,v.begin()+i2,val);
        }

        void make_heap()     {
            std::make_heap(v.begin(),v.end());    
        }     
        void pop_heap() { std::pop_heap(v.begin(),v.end()); }
        void push_heap(const T& val){
            v.push_back(val);
            std::push_heap(v.begin(),v.end()); 
        }    
        bool next_permutation() { return std::next_permutation(v.begin(),v.end()); }        
        bool is_sorted() { return std::is_sorted(v.begin(), v.end()); }    
        bool is_heap() { return std::is_heap(v.begin(),v.end()); }    
        //bool is_partitioned() { return std::is_partitioned(v.begin(),v.end()); }    
        bool is_permutation() { return std::is_permutation(v.begin(),v.end(),v.begin()); }
        void merge_front(const Array<T> & a, const Array<T> & b)    {
            std::merge(a.v.begin(),a.v.end(),b.v.begin(),b.v.end(),v.begin());
        }
        void merge_back(const Array<T> & a, const Array<T> &b)    {
            std::merge(a.v.begin(),a.v.end(),b.v.begin(),b.v.end(),v.end());
        }    
        void random_shuffle() { std::random_shuffle(v.begin(),v.end()); }
        void replace(const T a, const T b)    {
            std::replace(v.begin(),v.end(),a,b);
        }
        void shuffle() { 
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(v.begin(),v.end(),std::default_random_engine(seed));
        }
        */
    };

    // it is not a linear algebra matrix just a 2-d array using std::vector
    template<class T>
    struct Matrix : public StdObject
    {
        std::vector<std::vector<T>> v;

        Matrix<T>() {} 
        Matrix<T>(size_t i, size_t j) { 
            v.resize(i); 
            for(size_t k = 0; k < i; i++)
                v[k].resize(j);
        } 
        Matrix<T>(const Matrix<T> & a) { v = a.v; }

        Matrix<T>& operator = (const Matrix<T> & b) { v = b.v; return *this; }
        
        Array<T> operator[](size_t index) { return Array<T>(v[index]); }    
        Array<T> __getitem(size_t index) { return (*this)[index]; }
        void     __setitem(size_t index, const Array<T> & r) {
            v[index] = r.v; 
        }
        Array<T> get_col(size_t col)
        {
            Array<T> r(v[0]);
            for(size_t i = 0; i < v.size(); i++)
                r[i] = v[i][col];
            return r;
        }
        /*
        void set_col(size_t col, const Array<T> & c)
        {        
            for(size_t i = 0; i < v.size(); i++)
            {
                v[i][col] = c[i];
            }
        }
        */
        T& operator()(size_t i, size_t j) { return v[i][j]; }
        void  set(size_t i, size_t j, T value) { v[i][j] = value; }
        T     get(size_t i, size_t j) { return v[i][j]; }

        Array<T> get_row(size_t r) { return Array<T>(v[r]); }

        void  resize(size_t rows, size_t cols) { 
            v.resize(rows);
            for(size_t l = 0; l < rows; l++)
                v[l].resize(cols); 
        }

        size_t rows() { return v.size(); }
        size_t cols() { return v[0].size();}
        bool   empty() { return v.empty(); }
        T      at(size_t row, size_t col) { return v[row].at(col); }
        void   clear() { v.clear(); }
            
        void push_back(const Array<T> & a) { v.push_back(a.v); }
    };
}
