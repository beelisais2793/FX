#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SRATE 8000

#include "../waveio.h"

void main()     {
    FILE *file;
    struct soundhdr hdr;
    long i,j,total=0,OVERSAMP;
    double m,k,q,r,T,t,coeff1,coeff2,Y[3]={0},denom,temp;
    short ideal,approx;

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    file = opensoundout("nlmsprdm.wav",&hdr);
#else
    file = opensoundout("nlmsprdm.snd",&hdr);
#endif

    m = 0.001;
    k = 1000;
    q = 1;
    r = 0.002;
    OVERSAMP = 50;
    T = 1.0 / ((float) SRATE * (float) OVERSAMP);        //  Oversampled rate
//    temp =  (m + (T*r) + (T*T*k));                 //  Coefficient denominator
//    coeff1 = ((2.0*m) + (T*r)) / temp;
//    coeff2 = - m / temp;
      denom = (1 + T*T*k/m + T*r/m);
      coeff1 = (2 + T*r/m) / denom;
      coeff2 = - 1 / denom;


    printf("%f %f\n",coeff1,coeff2);

    Y[1] = 0.01;                                    // Normalized initial condition
    Y[2] = 0;  Y[0] = 0;
    for (i=0;i<16384;i++)   {
        t = (float) i / (float) SRATE;
        for (j=0;j<OVERSAMP;j++)      {                 // Digital approximation
            temp = T*T*q*Y[0] * Y[0] * Y[0] / m / denom;
            Y[0] = (Y[1] * coeff1) + (Y[2] * coeff2) - temp;
            Y[2] = Y[1];
            Y[1] = Y[0];
        }
        approx = 2000000 * (Y[1] - Y[2]);
        fwrite(&approx,2,1,file);
        total += 1;
    }

    Y[1] = 0.1;                                    // Normalized initial condition
    Y[2] = 0;  Y[0] = 0;
    for (i=0;i<16384;i++)   {
        t = (float) i / (float) SRATE;
        for (j=0;j<OVERSAMP;j++)      {                 // Digital approximation
            temp = T*T*q*Y[0] * Y[0] * Y[0] / m / denom;
            Y[0] = (Y[1] * coeff1) + (Y[2] * coeff2) - temp;
            Y[2] = Y[1];
            Y[1] = Y[0];
        }
        approx = 200000 * (Y[1] - Y[2]);
        fwrite(&approx,2,1,file);
        total += 1;
    }

    Y[1] = 1.0;                                // Normalized initial condition
    Y[2] = 0;  Y[0] = 0;
    for (i=0;i<16384;i++)   {
        t = (float) i / (float) SRATE;
        for (j=0;j<OVERSAMP;j++)      {                 // Digital approximation
            temp = T*T*q*Y[0] * Y[0] * Y[0] / m / denom;
            Y[0] = (Y[1] * coeff1) + (Y[2] * coeff2) - temp;
            Y[2] = Y[1];
            Y[1] = Y[0];
        }
        approx = 20000 * (Y[1] - Y[2]);
        fwrite(&approx,2,1,file);
        total += 1;
    }

    closesoundout(file,total);
}


