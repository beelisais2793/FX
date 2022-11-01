#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "waveio.h"
#define SRATE 44100
#define FREQ 110.0

void main(void) {
    long i;
    long tick;
    short data;
    struct soundhdr hdr;
    long total = 0;
    float env,delta;
    FILE *file;

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    file = opensoundout("pulses.wav",&hdr);
#else
    file = opensoundout("pulses.snd",&hdr);
#endif
    delta = 2.0 / (float) SRATE;
    env = 0;
    tick = SRATE / FREQ;
    for (i=0;i<SRATE/2;i++)       {
        if (total % tick == 0)
            data = env * 16384;
        else
            data = 0;
        fwrite(&data,2,1,file);
        env = env + delta;
        total += 1;
    }
    for (i=0;i<SRATE;i++)       {
        if (total % tick == 0)
            data = env * 16384;
        else
            data = 0;
        fwrite(&data,2,1,file);
        total += 1;
    }
    for (i=0;i<SRATE/2;i++)       {
        if (total % tick == 0)
            data = env * 16384;
        else
            data = 0;
        fwrite(&data,2,1,file);
        env = env - delta;
        total += 1;
    }
    closesoundout(file,total);

}


