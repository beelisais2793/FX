%{
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <random>
#include <memory>
%}

%inline %{
namespace std {
    template<typename T>
    struct vector_iterator
    {
        typename std::vector<T>::iterator iter;
        std::vector<T> v;

        vector_iterator(const std::vector<T> & vec) {
            v = vec;
        }   
        vector_iterator(const std::vector<T> & vec, const typename std::vector<T>::iterator & i) {
            iter = i;
            v = vec;
        }

        vector_iterator<T>& operator = (const T& val) {
            *iter = val;
            return *this;
        }
        vector_iterator<T>& operator = (const vector_iterator<T>& val) {
            iter = val.iter;
            v    = val.v;
            return *this;;
        }
                
        void next() {
            if(iter != v.end()) iter++;
        }
        void prev() {
            if(iter != v.begin()) iter--;
        }
        void forward(size_t i) {
            iter += i;
        }
        void backward(size_t i) {
            iter -= i;
        }
        void jump(size_t i) {
            iter = i;
        }
        
        T value() { return *iter; }
        void set_value(const T& val) { *iter = val; }
    };
}%}

namespace std 
{
    
    template<typename T>
    class vector
    {    
    public:
        
        vector(size_t n, const T & value = T()) { resize(n,value); }        
        vector(const std::vector<T> & v);
        
        std::vector<T>& operator = (const std::vector<T> & v);

        %extend {
            // lua starts with 1
            T    __getitem(size_t i) { return (*$self)[i-1]; }
            void __setitem(size_t i, T val) { (*$self)[i-1] = val; }

            std::vector_iterator<T> begin() {
                std::vector_iterator<T> r(*$self,$self->begin());                
                return r;
            }
            std::vector_iterator<T> end() {
                std::vector_iterator<T> r(*$self,$self->end());                
                return r;
            }
            /*
            std::vector_iterator<T> rbegin() {
                std::vector_iterator<T> r(*$self,$self->rbegin());                
                return r;
            }
            std::vector_iterator<T> rend() {
                std::vector_iterator<T> r(*$self,$self->rend());                
                return r;
            }
            std::vector_iterator<T> cbegin() {
                std::vector_iterator<T> r(*$self,$self->cbegin());                
                return r;
            }
            std::vector_iterator<T> cend() {
                std::vector_iterator<T> r(*$self,$self->cend());                
                return r;                
            }
            std::vector_iterator<T> crbegin() {
                std::vector_iterator<T> r(*$self,$self->crbegin());                
                return r;
            }
            std::vector_iterator<T> crend() {
                std::vector_iterator<T> r(*$self,$self->crend());                
                return r;
            }
            */

            void erase(size_t i) { $self->erase($self->begin()+i-1); }
            void erase(size_t i, size_t n) { $self->erase($self->begin()+i-1,$self->begin()+n-1); }
            //void insert(size_t i, std::vector<T>& x) { $self->insert($self->begin()+i-1,x.begin()); }            
            //void emplace(size_t i, const T x) { $self->emplace($self->begin()+i-1,x); }
        }

        T& front();
        T& back();

        void push_back(const T & q);
        void pop_back() ;

        
        T&     at(size_t i);
        void   assign(size_t n, const T & val);
        T*     data();
        size_t size();
        bool empty() const;
        void resize(size_t n);
        void clear();        
        
        void swap(std::vector<T> & x);
        void shrink_to_fit();
        void reserve(size_t n);
        size_t max_size();
        size_t capacity();
    };

    template<typename T> bool binary_search(std::vector<T> & a, const T& val) { std::binary_search(a.begin(),a.end(),val,r); }
    template<typename T> void copy(std::vector<T> & dst, const std::vector<T> & src) { std::copy(src.begin(),src.end(),dst.begin()); }
    /*
    template<typename T> void rcopy(std::vector<T> & dst, const std::vector<T> & src) { std::copy(src.rbegin(),src.rend(),dst.begin()); }
    template<typename T> void ccopy(std::vector<T> & dst, const std::vector<T> & src) { std::copy(src.cbegin(),src.cend(),dst.begin()); }
    template<typename T> void crcopy(std::vector<T> & dst, const std::vector<T> & src) { std::copy(src.crbegin(),src.crend(),dst.begin()); }
    */
    template<typename T> void sort(std::vector<T> & vec) { std::sort(vec.begin(),vec.end()); }
    /*
    template<typename T> void rsort(std::vector<T> & vec) { std::sort(vec.rbegin(),vec.rend()); }
    template<typename T> void csort(std::vector<T> & vec) { std::sort(vec.cbegin(),vec.cend()); }
    template<typename T> void crsort(std::vector<T> & vec) { std::sort(vec.crbegin(),vec.crend()); }
    */
    template<typename T> void random_shuffle(std::vector<T> & vec) { std::random_shuffle(vec.begin(),vec.end()); }
    template<typename T> bool equal(const std::vector<T> & a, const std::vector<T> & b) { return std::equal(a.begin(),a.end(),b.begin()); }
    template<typename T> std::vector_iterator<T> find(const std::vector<T> & a, const T & val) { return std::vector_iterator<T>(std::find(a.begin(),a.end(),val)); }
    template<typename T> void replace(std::vector<T> & a, const T& val, const T& r) { std::replace(a.begin(),a.end(),val,r); }
    template<typename T> void remove(std::vector<T> & a, const T& val) { std::remove(a.begin(),a.end(),val,r); }
    template<typename T> const T min(const std::vector<T> & v) { return std::min(v); }
    template<typename T> const T max(const std::vector<T> & v) { return std::max(v); }
    template<typename T> void reverse(std::vector<T> & vec) { std::reverse(vec.begin(),vec.end()); }
    template<typename T> void swap(std::vector<T> & dst, const std::vector<T> & src) { std::swap(src.begin(),src.end(),dst.begin()); }
}
