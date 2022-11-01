#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../waveio.h"
#define SRATE 11025.0

/*****************************************/
/*   Generate an additive square wave    */
/*   Perry R. Cook, 2002                 */
/*****************************************/

void main(void) {
    FILE *file_out;
    struct soundhdr hdr;
    long i, j, k, total=0;
    short data;
    double temp;
    double freq = 220.0;
    int period;

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    file_out = opensoundout("squarewv.wav",&hdr);
#else
    file_out = opensoundout("squarewv.snd",&hdr);
#endif

    period = SRATE / freq / 2;
    data = 10000;
    while (total<2*SRATE)      {            // "Pure" 220 Hz. Square Wave
        for (i=0;i<period;i++)     {
            fwrite(&data,2,1,file_out);
            total = total + 1;
        }
        data = -data;
    }

    data = 0;
    for (i=0;i<SRATE/2;i++)       {         // Silence
        fwrite(&data,2,1,file_out);
        total += 1;
    }

    for (j=1;j<24;j+=2) {                   // Build bottom up
        for (i=0;i<SRATE;i++) {
            data = 0;
            total = total+1;
            for (k=1;k<=j;k+=2)   {
                temp = 6.28 * (double) total * (double) k * freq/(double) SRATE;
                data += 8192.0 * sin(temp) / (float) k;
            }
            fwrite(&data,2,1,file_out);
        }
        data = 0;
        for (i=0;i<SRATE/2;i++)       {
            fwrite(&data,2,1,file_out);
            total += 1;
        }
    }
    for (j=23;j>0;j-=2) {
        for (i=0;i<SRATE;i++) {
            data = 0;
            total = total+1;
            for (k=23;k>=j;k-=2)   {
                temp = 6.28 * (double) total * (double) k * freq/(double) SRATE;
                data += 8192.0 * sin(temp) / (float) k;
            }
            fwrite(&data,2,1,file_out);
        }
        data = 0;
        for (i=0;i<SRATE/2;i++)       {
            fwrite(&data,2,1,file_out);
            total += 1;
        }
    }

    data = 10000;
    for (j=0;j<2*SRATE;)      {         // "Pure" 220 Hz. Square Wave
        for (i=0;i<period;i++,j++)     {
            fwrite(&data,2,1,file_out);
            total = total + 1;
        }
        data = -data;
    }

    closesoundout(file_out,total);
}
   
