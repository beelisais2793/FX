#pragma once

#include "rackdsp.hpp"

namespace RackDSP
{

    /** A cyclic buffer which maintains a valid linear array of size S by sliding along a larger block of size N.
    The linear array of S elements are moved back to the start of the block once it outgrows past the end.
    This happens every N - S pushes, so the push() time is O(1 + S / (N - S)).
    For example, a float buffer of size 64 in a block of size 1024 is nearly as efficient as RingBuffer.
    Not thread-safe.
    */
    template <typename T, size_t S, size_t N>
    struct AppleRingBuffer {
        T data[N];
        size_t start = 0;
        size_t end = 0;

        void returnBuffer() {
            // move end block to beginning
            // may overlap, but memmove handles that correctly
            size_t s = size();
            std::memmove(data, &data[start], sizeof(T) * s);
            start = 0;
            end = s;
        }
        void push(T t) {
            if (end + 1 > N) {
                returnBuffer();
            }
            data[end++] = t;
        }
        T shift() {
            return data[start++];
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
        /** Returns a pointer to S consecutive elements for appending, requesting to append n elements.
        */
        T* endData(size_t n) {
            if (end + n > N) {
                returnBuffer();
            }
            return &data[end];
        }
        /** Actually increments the end position
        Must be called after endData(), and `n` must be at most the `n` passed to endData()
        */
        void endIncr(size_t n) {
            end += n;
        }
        /** Returns a pointer to S consecutive elements for consumption
        If any data is consumed, call startIncr afterwards.
        */
        const T* startData() const {
            return &data[start];
        }
        void startIncr(size_t n) {
            // This is valid as long as n < S
            start += n;
        }
    };
}