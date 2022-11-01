#pragma once 

#include <vector>
#include <iostream>
#include "TWaveTable.hpp"

namespace SoundAlchemy::WaveTables
{
    template<typename T>
    struct TFourierWave : public Object<T>
    {
        SoundAlchemy::TWaveTable<T> wave;
        T        *F;
        T        *P;
        T        *A;
        size_t        n;
        T         sr;

        TFourierWave(T sample_rate, int wave_size=1024) {
            n = sample_rate/2;
            F = new T[n];
            P = new T[n];
            A = new T[n];
            sr= sample_rate;

            std::vector<T> sine(wave_size);
            MakeSine(sine,wave_size,1.0f,sr);
            wave.addWaveTable(wave_size,sine,sr/2.0f);
        }
        ~TFourierWave() {
            if(F) delete [] F;
            if(A) delete [] A;
        }

        void SetSaw(T f) {
            size_t max_harmonics = (sr/2)/f;
            memset(F,0,n*sizeof(T));
            memset(A,0,n*sizeof(T));        
            for(size_t i = 1; i < max_harmonics; i++)
            {
                F[i-1] = i*f;
                A[i-1] = 1.0f/(T)i;
            }
        }
        void SetSquare(T f) {
            size_t max_harmonics = (sr/2)/f;
            memset(F,0,n*sizeof(T));
            memset(A,0,n*sizeof(T));        
            for(size_t i = 1; i < max_harmonics; i++)
            {                
                F[i-1] = i*f;
                if(i % 2 == 0) A[i-1] = 0;
                else A[i-1] = 1.0f/(T)i;
            }
        }
        void SetTriangle(T f) {
            size_t max_harmonics = (sr/2)/f;
            memset(F,0,n*sizeof(T));
            memset(A,0,n*sizeof(T));        
            for(size_t i = 1; i < max_harmonics; i++)
            {                
                F[i-1] = i*f;
                if(i % 2 == 0) A[i-1] = 0;
                else A[i-1] = std::pow(-1.0f,(i-1)/2.0f)/(T)(i*i);
            }
        }
        void SetSine(T f)
        {
            memset(F,0,n*sizeof(T));
            memset(A,0,n*sizeof(T));        
            F[0] = f;
            A[0] = 1.0f;
        }
        T Tick() {
            T r = 0.0f;
            for(size_t i = 0; i < n; i++) {
                if(F[i] == 0.0f) break;       
                wave.SetPhase(P[i]);                         
                P[i] += F[i]/sr;
                if(P[i] >= 1.0f) P[i] -= 1.0f;                
                r += A[i]*wave.Tick();
            }
            return r;
        }
        T Tick(T I, T Amp=1, T X =1, T Y = 1) {
            T r = 0.0f;
            for(size_t i = 0; i < n; i++) {
                if(F[i] == 0.0f) break;       
                wave.SetPhase(P[i]);                         
                P[i] += F[i]/sr;
                if(P[i] >= 1.0f) P[i] -= 1.0f;                
                r += A[i]*wave.Tick();
            }
            return I*X*Y*Amp*r;
        }
    };
}