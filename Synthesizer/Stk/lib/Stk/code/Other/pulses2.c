#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "waveio.h"
#define SRATE 44100
#define FREQ 147.0

short ticker(long tick)  {
    static long counter=0;
    counter -= 1;
    if (counter <= 0) {
        counter = tick;
        return 16384;
    }
    else return 0;
}

void main(void) {
    long i;
    long tick,ttarg;
    short data;
    struct soundhdr hdr;
    long total = 0;
    float env,delta;
    FILE *file;

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    file = opensoundout("pulses2.wav",&hdr);
#else
    file = opensoundout("pulses2.wav",&hdr);
#endif
    delta = 2.0 / (float) SRATE;
    env = 0;
    tick = SRATE / FREQ;
    ttarg = tick;
    for (i=0;i<SRATE/2;i++)       {
        data = env * ticker(tick);
        fwrite(&data,2,1,file);
        env = env + delta;
        total += 1;
    }
    for (i=0;i<SRATE/2;i++)       {
        data = ticker(tick);
        fwrite(&data,2,1,file);
        total += 1;
    }
    ttarg /= 0.75;
    for (i=0;i<SRATE;i++)       {
        data = env * ticker(tick);
        if (data > 0 & ttarg>tick) tick = tick+4;
        fwrite(&data,2,1,file);
        total += 1;
    }
    ttarg /= 0.8333;
    for (i=0;i<SRATE;i++)       {
        data = env * ticker(tick);
        if (data > 0 & ttarg>tick) tick = tick+4;
        fwrite(&data,2,1,file);
        total += 1;
    }
    ttarg /= 0.8;
    for (i=0;i<SRATE/2;i++)       {
        data = env * ticker(tick);
        if (data > 0 & ttarg>tick) tick = tick+4;
        fwrite(&data,2,1,file);
        total += 1;
    }
    for (i=0;i<1.5*SRATE;i++)       {
        data = env * ticker(tick);
        if (data > 0) tick = ttarg * (1.0 + 0.03*sin((float) 36.0*total/SRATE));
        fwrite(&data,2,1,file);
        total += 1;
    }
    for (i=0;i<SRATE/2;i++)       {
        data = env * ticker(tick);
        if (data > 0) tick = ttarg * (1.0 + 0.03*sin((float) 36.0*total/SRATE));
        fwrite(&data,2,1,file);
        env = env - delta;
        total += 1;
    }
    closesoundout(file,total);

}

