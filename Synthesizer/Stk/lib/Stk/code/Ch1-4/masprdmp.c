#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../waveio.h"
#define SRATE 8000

void main()     {
    FILE *out_file;
    struct soundhdr hdr;
    long i,j,total=0,OVERSAMP;
    double m,k,r,tau,omega,T,t,coeff1,coeff2,Y[3]={0},temp;
    short ideal,approx;

    m = 0.001/2; // 0.001/2; // Decrease this, frequency will go down
    k = 1000*2; // 1000*2;   //  Increase this, frequency will go up
    r = 0.002;             //  This controls loss (decay)
    tau = r / 2 / m;
    omega = sqrt(k/m - tau*tau);

    OVERSAMP = 1000;       //  Increase this, we get more accurate
    T = 1.0 / ((float) SRATE * (float) OVERSAMP);   //  Oversampled rate
    temp =  (m + (T*r) + (T*T*k));           //  Coefficient denominator
    coeff1 = ((2.0*m) + (T*r)) / temp;
    coeff2 = - m / temp;
//    printf("%f %f\n",coeff1,coeff2);
        Y[1] = 1.0;                          // Normalized initial condition

    fillheader(&hdr,SRATE);
#ifdef LITTENDIAN
    out_file = opensoundout("masprdmp.wav",&hdr);
#else
    out_file = opensoundout("masprdmp.snd",&hdr);
#endif

    for (i=0;i<SRATE*4;i++)   {
        t = (float) i / (float) SRATE;
        ideal = 16000 * exp(-t*tau) * cos(omega * t); // closed form exact
        fwrite(&ideal,2,1,out_file);
        total += 1;
    }

    ideal = 0;
    for (i=0;i<SRATE;i++)   {
        fwrite(&ideal,2,1,out_file);
        total += 1;
    }

    for (i=0;i<(SRATE*4);i++)   {
        for (j=0;j<OVERSAMP;j++)      {      // Digital approximation
            Y[0] = (Y[1] * coeff1) + (Y[2] * coeff2);
            Y[2] = Y[1];
            Y[1] = Y[0];
        }
        approx = 16000 * (Y[1] - Y[2]);
        fwrite(&approx,2,1,out_file);
        total += 1;
    }
    closesoundout(out_file,total);
}


