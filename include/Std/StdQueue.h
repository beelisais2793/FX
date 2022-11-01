#pragma once 

#include "datalogic.h"

namespace Std {
    template<class T>
    struct Queue : public StdObject
    {
        std::queue<T> q;

        Queue() {}
        Queue(const Queue & que) : q(que.q) {} 

        bool empty() { return q.empty(); }
        size_t size() { return q.size(); }
        T& front() { return q.front(); }
        T& back() { return q.back(); }
        void push(const T val) { q.push(val); }
        void pop() { q.pop(); }
    };
}    