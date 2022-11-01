#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "waveio.h"
#define SRATE 44100

short white_noise()     {
   return (rand() - 16384);
}

/* Paul Kellet's Pinking Filter */
float pinkIt(short input)     {
    static float pink, b0=0, b1=0, b2=0, b3=0, b4=0, b5=0, b6=0;

    b0 = 0.99886 * b0 + input * 0.0555179;
    b1 = 0.99332 * b1 + input * 0.0750759;
    b2 = 0.96900 * b2 + input * 0.1538520;
    b3 = 0.86650 * b3 + input * 0.3104856;
    b4 = 0.55000 * b4 + input * 0.5329522;
    b5 = -0.7616 * b5 - input * 0.0168980;
    pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + (input * 0.5362);
    b6 = input * 0.115926;
    return pink*0.2;
}

void main(void) {
    long i,ldata=0;
    short data;
    struct soundhdr hdr;
    long total = 0;
    float env,delta;
    FILE *file;

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    file = opensoundout("noises.wav",&hdr);
#else
    file = opensoundout("noises.snd",&hdr);
#endif
    delta = 2.0 / (float) SRATE;
    env = 0;
    for (i=0;i<SRATE/2;i++)       {
        data = env * white_noise();
        fwrite(&data,2,1,file);
        env = env + delta;
        total += 1;
    }
    for (i=0;i<2*SRATE;i++)       {
        data = env * white_noise();
        fwrite(&data,2,1,file);
        total += 1;
    }
    for (i=0;i<SRATE/2;i++)       {
        data = env * white_noise();
        fwrite(&data,2,1,file);
        env = env - delta;
        total += 1;
    }


    env = 0;
    for (i=0;i<SRATE/2;i++)       {
        data = env * pinkIt(white_noise());
        fwrite(&data,2,1,file);
        env = env + delta;
        total += 1;
    }
    for (i=0;i<2*SRATE;i++)       {
        data = env * pinkIt(white_noise());
        fwrite(&data,2,1,file);
        total += 1;
    }
    for (i=0;i<SRATE/2;i++)       {
        data = env * pinkIt(white_noise());
        fwrite(&data,2,1,file);
        env = env - delta;
        total += 1;
    }

    closesoundout(file,total);

}


