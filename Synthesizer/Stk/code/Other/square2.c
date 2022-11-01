#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "waveio.h"

#define PI 3.141592
#define SRATE 22050.0
#define BASEFREQ 110.0

#define NUM_MODES 28

void main(void) {
    float out,env,temp;
    FILE *outFile;
    struct soundhdr hdr;
    short data;
    int notDone = 1;
    long i, j, k, total=0;
    int modes[NUM_MODES][2] =
      { {1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7},
        {2,2},{2,3},{2,4},{2,5},{2,6},{2,7},
        {3,3},{3,4},{3,5},{3,6},{3,7},
        {4,4},{4,5},{4,6},{4,7},
        {5,5},{5,6},{5,7},
        {6,6},{6,7},
        {7,7}};
    float freqs[NUM_MODES];
    float gains[NUM_MODES];

    for (i=0;i<NUM_MODES;i++)   {
       freqs[i] = 2*PI/SRATE*BASEFREQ*sqrt(modes[i][0]*modes[i][0] +
                                modes[i][1]*modes[i][1]);
    }

    notDone = 1;
    while (notDone)     {
        notDone = 0;
        for (i=0;i<NUM_MODES-1;i++)      {
//            if (freqs[i] == freqs[i+1]) {
//                freqs[i] = 0;
//                notDone = 1;
//            }
            if (freqs[i] > freqs[i+1])  {
                notDone = 1;
                temp = freqs[i];
                freqs[i] = freqs[i+1];
                freqs[i+1] = temp;
            }
        }
    }
    for (i=0;i<NUM_MODES;i++)  {
        gains[i] = freqs[0] / freqs[i];
        printf("%f %f\n",freqs[i],gains[i]);
    }

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    outFile = opensoundout("square2.wav",&hdr);
#else
    outFile = opensoundout("square2.snd",&hdr);
#endif
    env = 4000.0;
    for (k=0;k<4*SRATE;k++)     {
        out = 0;
        env = env * 0.99995;
        for (j=0;j<28;j++)   {
            out += env * gains[j] * sin(freqs[j]*k);
        }
        data = out;
        fwrite(&data,1,2,outFile);
        total += 1;
    }

    for (i=0;i<NUM_MODES;i++)   {
        env = 4000.0;
        for (k=0;k<SRATE/3;k++)     {
            env = env * 0.9998;
            out = env * gains[i] * sin(freqs[i]*k);
            data = out;
            fwrite(&data,1,2,outFile);
            total += 1;
        }
    }

    for (i=0;i<NUM_MODES;i++)   {
        env = 4000.0;
        for (k=0;k<SRATE/3;k++)     {
            out = 0;
            env = env * 0.9998;
            for (j=0;j<=i;j++)   {
                out += env * gains[j] * sin(freqs[j]*k);
            }
            data = out;
            fwrite(&data,1,2,outFile);
            total += 1;
        }
    }

    data = 0;
    for (k=0;k<2*SRATE;k++)     {
        fwrite(&data,1,2,outFile);
        total += 1;
    }

    env = 4000.0;
    for (k=0;k<4*SRATE;k++)     {
        out = 0;
        env = env * 0.99995;
        for (j=0;j<28;j++)   {
            out += env * gains[j] * sin(freqs[j]*k);
        }
        data = out;
        fwrite(&data,1,2,outFile);
        total += 1;
    }

    closesoundout(outFile,total);
}
        

