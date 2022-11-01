#pragma once 

#include "datalogic.h"

namespace Std {
    template<class T> 
    struct Set : public StdObject
    {
        std::set<T> s;

        Set() {}

        bool empty() { return s.empty(); }
        size_t size() { return s.size(); }
        size_t max_size() { return s.max_size(); }

        void insert( const T & val) { s.insert(val); }
        void erase(const T & val) { s.erase(s.find(val)); }
        bool search(const T & val) { return s.find(val) != s.end(); }
        void clear() { s.clear(); }
        size_t count(const T & val) { return s.count(val); }

    };
};    