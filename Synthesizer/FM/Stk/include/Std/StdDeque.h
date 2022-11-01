#pragma once
#include "datalogic.h"

namespace Std { 
    template<class T>
    struct Deque : public StdObject
    {
        std::deque<T> d; 

        Deque() {} 

        
        size_t size() { return d.size(); }
        size_t max_size() { return d.max_size(); }
        void resize(size_t n) { d.resize(n); }
        bool empty() { return d.empty(); }
        void shrink_to_fit() { d.shrink_to_fit(); }

        T& operator[](size_t n) { return d[n];}
        
        T& __getitem(size_t n) { return (*this)[n]; }
        void __setitem(size_t n, const T val) { d[n] = val; }

        T& at(size_t n) { return d[n]; }
        T& front() { return d.front(); }
        T& back()  { return d.back(); }

        void push_back(const T val) { d.push_back(val); }
        void push_front(const T val) { d.push_front(val); }
        T pop_back() { T r = d.back(); d.pop_back(); return r; }
        T pop_front() { T r = d.front(); d.pop_front(); return r; }

        void insert_at(size_t index, const T val)
        {
            assert(index < d.size());
            typename std::deque<T>::iterator it = d.begin();
            for(size_t i = 1; i < index; i++) *it++;
            d.insert(it,val);
        }
        void remove_at(size_t index)
        {
            assert(index < d.size());
            typename std::deque<T>::iterator it = d.begin();
            for(size_t i = 1; i < index; i++) *it++;
            d.erase(it);
        }

        void clear() { d.clear(); }
    };

}
