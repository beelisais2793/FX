#pragma once 
#include <cmath>
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::WaveTable 
{
    template<typename T>
    struct TWaveCycle
    {
        sample_vector<T> cycle;    
        float phase,freq,inc;
        float sr;
        
        TWaveCycle(float sampleRate)
        {
            sr = sampleRate;
            cycle.resize(4096);                
            phase = 0.0f;          
        }
        void setFrequency(T f) {
            freq = f;
            inc   = freq/sr;                
        }
        T Tick(T I=1,T A=1, T X=0, T Y=0) {
            float r = phase*cycle.size();
            int   i = (int)r;
            float frac = r - i;
            float v1 = cycle[i];
            int   n = (i+1) % cycle.size();
            float v2 = cycle[n];        
            phase = std::fmod(phase + inc,1);
            float x = frac*v1 + (1.0-frac)*v2;  
            /*
            float q = 0;
            for(size_t i = 1; i < 20; i+=2)
                q  += std::sin(2.0f*M_PI*i*phase)/(float)i;
            x = (4.0/M_PI)*q;
            */
            return 0.1*x;
        }
    };

    template<typename T>
    struct TWaveVector
    {
        sample_vector<TWaveCycle<T>> table;
        size_t cycle_len;

        TWaveVector() = default;
        TWaveVector(size_t num_tables, size_t cycle_size) {
            table.resize(num_tables);
            cycle_len = cycle_size;
        }
        void resize(size_t n, size_t len) {
            table.resize(n);
            cycle_len = len;
        }
        TWaveVector<T>& operator = (const TWaveVector<T> & in) {
            table = in;
            cycle_len = in.size();
            return *this;
        }
    };

    template<typename T>
    struct TWaveMatrix
    {
        std::vector<TWaveVector<T>> matrix;
        size_t cycle_len;

        TWaveMatrix(size_t M, size_t num_tables, size_t cycle_size){
            matrix.resize(M);
            for(size_t i = 0; i < M; i++) {
                matrix[i].resize(num_tables,cycle_size);
            }            
        }
        TWaveVector<T>& operator()(size_t i) {
            return matrix[i];
        }
        void setVector(size_t i, TWaveVector<T> v) {
            matrix[i] = v;
        }
    };
}