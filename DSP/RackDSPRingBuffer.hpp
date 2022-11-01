#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{
    /** A simple cyclic buffer.
    S must be a power of 2.
    Thread-safe for single producers and consumers.
    */
    template <typename T, size_t S>
    struct RingBuffer {
        T data[S];
        size_t start = 0;
        size_t end = 0;

        size_t mask(size_t i) const {
            return i & (S - 1);
        }
        void push(T t) {
            size_t i = mask(end++);
            data[i] = t;
        }
        void pushBuffer(const T* t, int n) {
            size_t i = mask(end);
            size_t e1 = i + n;
            size_t e2 = (e1 < S) ? e1 : S;
            std::memcpy(&data[i], t, sizeof(T) * (e2 - i));
            if (e1 > S) {
                std::memcpy(data, &t[S - i], sizeof(T) * (e1 - S));
            }
            end += n;
        }
        T shift() {
            return data[mask(start++)];
        }
        void shiftBuffer(T* t, size_t n) {
            size_t i = mask(start);
            size_t s1 = i + n;
            size_t s2 = (s1 < S) ? s1 : S;
            std::memcpy(t, &data[i], sizeof(T) * (s2 - i));
            if (s1 > S) {
                std::memcpy(&t[S - i], data, sizeof(T) * (s1 - S));
            }
            start += n;
        }
        void clear() {
            start = end;
        }
        bool empty() const {
            return start == end;
        }
        bool full() const {
            return end - start == S;
        }
        size_t size() const {
            return end - start;
        }
        size_t capacity() const {
            return S - size();
        }
    };
}