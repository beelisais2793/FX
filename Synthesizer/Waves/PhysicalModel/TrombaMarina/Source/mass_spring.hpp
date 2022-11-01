/*
  ==============================================================================

    mass_spring.cpp
    Created: 25 Mar 2018 5:08:02pm
    Author:  Nikolaj Schwab Andersson

  ==============================================================================
*/
#pragma once

#include <math.h>

class MassSpring
{
public:
   double frequency = 0;

    void setSampleRate(double sampleRate)
    {
        T = double (1.0f/sampleRate);
        alpha = double (2.0f/T);
        frequency = 1/(2*M_PI) * sqrt(k/m);
        updateMatrices();
    }
    
    void setFrequency(float freq, float mass)
    {
        frequency = freq;
        m = mass;
        double temp = frequency * (2 * M_PI) * sqrt(m);
        k = temp * temp;
        
        updateMatrices();
    }
    
    void init(double res, double stiffness, double mass, double initialState[2])
    {
        b = res;
        k = stiffness;
        m = mass;
        frequency = 1/(2*M_PI) * sqrt(k/m);
        
        x[0] = initialState[0];
        x[1] = initialState[1];
    }
    
    void setInput(double input)
    {
        u = input;
    }
    
    void run()
    {
        updateStates();
    }
    
   double getOutput()
    {
        return x[0];
    }
    
    
private:

    void updateStates()
    {
        double tempX0 = x[0];
        double tempX1 = x[1];
        
        x[0] = Ad[0]*tempX0 + Ad[1]*tempX1 + Bd[0]*(u + du);
        x[1] = Ad[2]*tempX0 + Ad[3]*tempX1 + Bd[1]*(u + du);
        
        // update input to avoid inf if u is not set
        du = u;
        u = 0.0;
    }
    
    void updateMatrices()
    {
       double temp1[4], temp2[4];
        
        A[0] = 0.0;
        A[1] = 1.0;
        A[2] = -k/m;
        A[3] = -b/m;
        
        B[0] = 0.0;
        B[1] = 1.0/m;
        
        temp1[0] = alpha - A[0];
        temp1[1] = -A[1];
        temp1[2] = -A[2];
        temp1[3] = alpha - A[3];
        
        invertMatrix(temp1, H);

        
        temp2[0] = alpha + A[0];
        temp2[1] = A[1];
        temp2[2] = A[2];
        temp2[3] = alpha+A[3];

        // make new matrices to make the updateStates easier
        Ad[0] = H[0]*temp2[0] + H[1]*temp2[2];
        Ad[1] = H[0]*temp2[1] + H[1]*temp2[3];
        
        Ad[2] = H[2]*temp2[0] + H[3]*temp2[2];
        Ad[3] = H[2]*temp2[1] + H[3]*temp2[3];

        Bd[0] = H[0]*B[0] + H[1]*B[1];
        Bd[1] = H[2]*B[0] + H[3]*B[1];
        
    }
    
    void invertMatrix(double (&m)[4], double (&invOut)[4])
    {
        // one over the determinant
       double det = 1.0/(m[0]*m[3] - m[2]*m[1]);
        
        // Matrix invertion
        invOut[0] = det*m[3];
        invOut[1] = -det*m[1];
        
        invOut[2] = -det*m[2];
        invOut[3] = det*m[0];
    
    }
    
    
   double u = 0.0;
   double du = 0.0; // delayed input
    
    // vectors and matrices
   double x[2] = {0.0, 0.0};
    
   double A[4] = {0.0, 0.0, 0.0, 0.0};
   double B[2] = {0.0, 0.0};
    
   double Ad[4] = {0.0, 0.0, 0.0, 0.0};
   double Bd[2] = {0.0, 0.0};
    
   double H[4] = {0.0, 0.0, 0.0, 0.0};
    
   double T = 0.0;
   double alpha = 0.0;
    
   double b = 0.05;      // Resistance
   double k = 1000000;  // spring stiffness
   double m = 0.1;     // mass
};
