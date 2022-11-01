#pragma once

#include "datalogic.h"

namespace Std {
    template<class Key, class Data>
    struct AssociativeArray: public StdObject
    {
    
        std::map<Key,Data> m;

        AssociativeArray() {}

        

        void insert(const Key key, const Data & data) { m[key] = data; }
        void erase(const Key key) { m.erase(key); }
        void clear() { m.clear(); }
        bool empty() { return m.empty(); }  
        size_t size() { return m.size(); }
        size_t max_size() { return m.max_size(); }

        Data& operator[](const Key  key) { return m[key]; }
        Data __getitem(const Key  key) { return m[key]; }
        void __setitem(const Key  key, const Data val) { m[key] = val; }

        bool find(const Key key)
        {
            typename std::map<Key,Data>::iterator i;
            i = m.find(key);
            return i != m.end();
        }

        size_t count(const Key key) { return m.count(key); }  

        //bool operator == (const AssociativeArray<T> & a) { return m == a.m; }
        //bool operator != (const AssociativeArray<T> & a) { return m != a.m; }
        //bool operator < (const AssociativeArray<T> & a) { return m < a.m; }
        //bool operator > (const AssociativeArray<T> & a) { return m > a.m; }
        //bool operator <= (const AssociativeArray<T> & a) { return m <= a.m; }
        //bool operator >= (const AssociativeArray<T> & a) { return m >= a.m; }
    };

}
