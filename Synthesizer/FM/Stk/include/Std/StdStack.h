#pragma once 

#include "datalogic.h"

namespace Std{
    template<class T>
    struct Stack : public StdObject
    {
        std::stack<T> s;

        Stack() {} 

        bool empty() { return s.empty(); }
        size_t size(){ return s.size(); }
        T& top() { return s.top(); }
        void push(const T val) { s.push(val); }
        void pop() { s.pop(); }
    };
}