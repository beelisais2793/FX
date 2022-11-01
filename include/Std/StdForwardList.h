#pragma once 

#include "datalogic.h" 
#include "StdVecarray.h"

namespace Std {
    template<class T>
    struct ForwardList : public StdObject
    {

        std::list<T> l;

        ForwardList(size_t size = 1) { l.resize(size); }   
        
        T& operator[](size_t index)
        {
            assert(index < l.size());
            typename std::list<T>::iterator it = l.begin();
            for(size_t i = 0; i < index; i++) it++;
            return *it;
        }
        T    __getitem(size_t key) { return (*this)[key]; }
        void __setitem(const size_t key, const T val) { (*this)[key] = val; }

        void insert_vector(size_t index, const Array<T> & a)
        {
            assert(index < l.size());    
            typename std::list<T>::iterator it = l.begin();        
            for(size_t i = 1; i < index; i++) it++;
            l.insert(it,a.v.begin(),a.v.end());
        }   
        void insert_at(size_t index, const T val)
        {        
            assert(index < l.size());    
            typename std::list<T>::iterator it = l.begin();        
            for(size_t i = 1; i < index; i++) it++;
            l.insert(it,val);
        }
        T remove_at(size_t index)
        {
            assert(index < l.size());
            typename std::list<T>::iterator it = l.begin();
            for(size_t i = 1; i < index; i++) it++;
            T r = *it;
            l.erase(it);
            return r;
        }
        
        void push_back(const T val) { l.push_back(val); }
        void push_front(const T val) { l.push_front(val); }

        void pop_back() { l.pop_back(); }
        void pop_front() { l.pop_back(); }

        bool    empty() { return l.empty(); }
        size_t  max_size() { return l.max_size(); } 
        void unique() { l.unique(); }

        T front() { return l.front(); }
        T back()  { return l.back(); }

        void resize(size_t s) { l.resize(s); }
        void clear() { l.clear(); }    
        void reverse() { l.reverse(); }

        size_t size() { return l.size(); }

        void swap(ForwardList<T> & x) { l.swap(x.l); }

    };

}    
