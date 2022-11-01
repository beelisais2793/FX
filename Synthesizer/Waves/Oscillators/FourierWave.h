#pragma once 
#include <vector>
#include <iostream>
#include "WaveTable.h"

namespace SoundWave 
{
    struct FourierWave
    {
        SoundWave::WaveTableOsc wave;
        float        *F;
        float        *P;
        float        *A;
        size_t        n;
        float         sr;

        FourierWave(float sample_rate, int wave_size=1024) {
            n = sample_rate/2;
            F = new float[n];
            P = new float[n];
            A = new float[n];
            sr= sample_rate;

            std::vector<float> sine(wave_size);
            SoundWave::MakeSine(sine,wave_size,1.0f,sr);
            wave.addWaveTable(wave_size,sine,sr/2.0f);
        }
        ~FourierWave() {
            if(F) delete [] F;
            if(A) delete [] A;
        }

        void SetSaw(float f) {
            size_t max_harmonics = (sr/2)/f;
            memset(F,0,n*sizeof(float));
            memset(A,0,n*sizeof(float));        
            for(size_t i = 1; i < max_harmonics; i++)
            {
                F[i-1] = i*f;
                A[i-1] = 1.0f/(float)i;
            }
        }
        void SetSquare(float f) {
            size_t max_harmonics = (sr/2)/f;
            memset(F,0,n*sizeof(float));
            memset(A,0,n*sizeof(float));        
            for(size_t i = 1; i < max_harmonics; i++)
            {                
                F[i-1] = i*f;
                if(i % 2 == 0) A[i-1] = 0;
                else A[i-1] = 1.0f/(float)i;
            }
        }
        void SetTriangle(float f) {
            size_t max_harmonics = (sr/2)/f;
            memset(F,0,n*sizeof(float));
            memset(A,0,n*sizeof(float));        
            for(size_t i = 1; i < max_harmonics; i++)
            {                
                F[i-1] = i*f;
                if(i % 2 == 0) A[i-1] = 0;
                else A[i-1] = std::pow(-1.0f,(i-1)/2.0f)/(float)(i*i);
            }
        }
        void SetSine(float f)
        {
            memset(F,0,n*sizeof(float));
            memset(A,0,n*sizeof(float));        
            F[0] = f;
            A[0] = 1.0f;
        }
        float Tick() {
            float r = 0.0f;
            for(size_t i = 0; i < n; i++) {
                if(F[i] == 0.0f) break;       
                wave.SetPhase(P[i]);                         
                P[i] += F[i]/sr;
                if(P[i] >= 1.0f) P[i] -= 1.0f;                
                r += A[i]*wave.Tick();
            }
            return r;
        }
    };
}