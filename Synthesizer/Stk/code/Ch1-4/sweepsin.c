#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../waveio.h"

/*********************************************/
/*   Generate a mono wave of swept sine wave */
/*   Perry R. Cook, 2002                     */
/*********************************************/

void main(void) {
    FILE *file_out;
    struct soundhdr hdr;
    long i = 0;
    short data;
    double ratio = 1.00005;
    double freq = 100.0;

    fillheader(&hdr,44100);
#ifdef LITTLENDIAN
    file_out = opensoundout("sweepsin.wav",&hdr);
#else
    file_out = opensoundout("sweepsin.snd",&hdr);
#endif

    while (freq > 99.9) {
        i = i+1;
        data = 8192.0 * sin((double)i*freq/44100.0);
        fwrite(&data,2,1,file_out);
        freq = freq*ratio;
        if (freq > 15000.0) {
            ratio = 1.0 / ratio;
            freq = 15000.0;
        }
    }
    closesoundout(file_out,i);
}
   
