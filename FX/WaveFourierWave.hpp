#pragma once 
#include <vector>
#include <iostream>
#include "WaveTable.hpp"

struct FourierWave
{
    WaveTableOsc wave;
    double        *F;
    double        *P;
    double        *A;
    size_t        n;
    double         sr;

    FourierWave(double sample_rate, int wave_size=1024) {
        n = sample_rate/2;
        F = new double[n];
        P = new double[n];
        A = new double[n];
        sr= sample_rate;

        std::vector<double> sine(wave_size);
        MakeSineTable(sine,wave_size,1.0f,sr);
        wave.addWaveTable(wave_size,sine,sr/2.0f);
    }
    ~FourierWave() {
        if(F) delete [] F;
        if(A) delete [] A;
    }

    void SetSaw(double f) {
        size_t max_harmonics = (sr/2)/f;
        memset(F,0,n*sizeof(double));
        memset(A,0,n*sizeof(double));        
        for(size_t i = 1; i < max_harmonics; i++)
        {
            F[i-1] = i*f;
            A[i-1] = 1.0f/(double)i;
        }
    }
    void SetSquare(double f) {
        size_t max_harmonics = (sr/2)/f;
        memset(F,0,n*sizeof(double));
        memset(A,0,n*sizeof(double));        
        for(size_t i = 1; i < max_harmonics; i++)
        {                
            F[i-1] = i*f;
            if(i % 2 == 0) A[i-1] = 0;
            else A[i-1] = 1.0f/(double)i;
        }
    }
    void SetTriangle(double f) {
        size_t max_harmonics = (sr/2)/f;
        memset(F,0,n*sizeof(double));
        memset(A,0,n*sizeof(double));        
        for(size_t i = 1; i < max_harmonics; i++)
        {                
            F[i-1] = i*f;
            if(i % 2 == 0) A[i-1] = 0;
            else A[i-1] = std::pow(-1.0f,(i-1)/2.0f)/(double)(i*i);
        }
    }
    void SetSine(double f)
    {
        memset(F,0,n*sizeof(double));
        memset(A,0,n*sizeof(double));        
        F[0] = f;
        A[0] = 1.0f;
    }
    double Tick() {
        double r = 0.0f;
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

