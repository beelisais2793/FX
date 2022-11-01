#pragma once 

#include "datalogic.h"

namespace Std {
    template<typename T>
    struct CBuffer: public StdObject
    {
        T * buffer; 
        size_t W; 

        CBuffer() : buffer(NULL),W(0)
        {
            
        }
        CBuffer(size_t w)
        {
            W = w;
            buffer = new T[W];   
            memset(buffer, 0x0, sizeof(T)*W);
            assert(buffer != NULL);
        }
        ~CBuffer()
        {
            if(buffer) delete buffer;
        }

        

        CBuffer<T>& copy(CBuffer<T> & to)
        {
            if(to.buffer) delete to.buffer;
            to.W = W; 
            to.buffer = new T[W];
            memcpy(to.buffer, buffer, sizeof(T)*W);
            return to;
        }

        T& operator[](size_t index) {
            assert(index < W);
            return buffer[index];
        }

        T __getitem(size_t index) { return (*this)[index]; }
        void __setitem(size_t index, const T val) { 
            assert(index < W);
            buffer[index] = val;
        }

        size_t size() { return W; }
        void resize(size_t w)
        {
            
            T * nb = new T[w];
            for(size_t i = 0; i < W; i++) nb[i] = buffer[i];
            W = w;
            delete buffer;
            buffer = nb;
        }

    };
}