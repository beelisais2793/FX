#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "waveio.h"
#define SRATE 11025.0

/*****************************************/
/*   FOF synthesis of ahh, eee, ooo      */
/*   Perry R. Cook, 2002                 */
/*   I'm cheating here some, as the      */
/*   Formants must be multiples of       */
/*   the intended fundamental pitch,     */
/*   and I don't actually overlap-add    */
/*   the FOF functions, but rather "re-  */
/*   excite the decaying sinusoids.      */
/*   Thus it's really a filter-like      */
/*   model.  Real overlap-add FOF        */
/*   synthesis takes much more work.     */
/*****************************************/

double ahhF[3] = {660, 1100, 2640};
double eeeF[3] = {330, 1870, 2860};
double oooF[3] = {330, 660, 2750};
double ahhQ[3] = {0.996,0.997,0.995};
double eeeQ[3] = {0.998,0.995,0.993};
double oooQ[3] = {0.998,0.996,0.993};
double ahhA[3] = {1.0, 0.5, 0.5};
double eeeA[3] = {1.0, 0.3, 0.2};
double oooA[3] = {1.0, 0.14, 0.2};

void main(void) {
    FILE *file_out;
    struct soundhdr hdr;
    long i, j, k, total=0;
    short data;
    double a[3];
    double temp;
    double freq = 110.0;
    int period;

    fillheader(&hdr,SRATE);
#ifdef LITTLENDIAN
    file_out = opensoundout("fofs.wav",&hdr);
#else
    file_out = opensoundout("fofs.snd",&hdr);
#endif

    period = SRATE / freq;

    for (j=0;j<3;j++)   {
        a[j] = ahhA[j];
        for (i=0;i<SRATE;i++) {
            total = total+1;
            temp = 6.28 * (double) total * ahhF[j] / (double) SRATE;
            data = 8192.0 * a[j] * sin(temp);
            fwrite(&data,2,1,file_out);
            a[j] *= ahhQ[j];
        }
    }
    a[0] = ahhA[0];
    a[1] = ahhA[1];
    a[2] = ahhA[2];
    for (i=0;i<SRATE;i++) {
        data = 0;
        total = total+1;
        for (j=0;j<3;j++)   {
            temp = 6.28 * (double) total * ahhF[j] / (double) SRATE;
            data += 8192.0 * a[j] * sin(temp);
            a[j] *= ahhQ[j];
        }
        fwrite(&data,2,1,file_out);
    }
    for (i=0;i<SRATE*2;i++)     {
        if (i%period == 0)       {
            a[0] += ahhA[0];
            a[1] += ahhA[1];
            a[2] += ahhA[2];
        }
        data = 0;
        total = total+1;
        for (j=0;j<3;j++)   {
            temp = 6.28 * (double) total * ahhF[j] / (double) SRATE;
            data += 2048.0 * a[j] * sin(temp);
            a[j] *= ahhQ[j];
        }
        fwrite(&data,2,1,file_out);
    }
    data = 0;
    for (i=0;i<SRATE/2;i++)     {
         total = total + 1;
         fwrite(&data,2,1,file_out);
    }

    for (j=0;j<3;j++)   {
        a[j] = eeeA[j];
        for (i=0;i<SRATE;i++) {
            total = total+1;
            temp = 6.28 * (double) total * eeeF[j] / (double) SRATE;
            data = 8192.0 * a[j] * sin(temp);
            fwrite(&data,2,1,file_out);
            a[j] *= eeeQ[j];
        }
    }
    a[0] = eeeA[0];
    a[1] = eeeA[1];
    a[2] = eeeA[2];
    for (i=0;i<SRATE;i++) {
        data = 0;
        total = total+1;
        for (j=0;j<3;j++)   {
            temp = 6.28 * (double) total * eeeF[j] / (double) SRATE;
            data += 8192.0 * a[j] * sin(temp);
            a[j] *= eeeQ[j];
        }
        fwrite(&data,2,1,file_out);
    }
    for (i=0;i<SRATE*2;i++)     {
        if (i%period == 0)       {
            a[0] += eeeA[0];
            a[1] += eeeA[1];
            a[2] += eeeA[2];
        }
        data = 0;
        total = total+1;
        for (j=0;j<3;j++)   {
            temp = 6.28 * (double) total * eeeF[j] / (double) SRATE;
            data += 2048.0 * a[j] * sin(temp);
            a[j] *= eeeQ[j];
        }
        fwrite(&data,2,1,file_out);
    }
    data = 0;
    for (i=0;i<SRATE/2;i++)     {
         total = total + 1;
         fwrite(&data,2,1,file_out);
    }


    for (j=0;j<3;j++)   {
        a[j] = oooA[j];
        for (i=0;i<SRATE;i++) {
            total = total+1;
            temp = 6.28 * (double) total * oooF[j] / (double) SRATE;
            data = 8192.0 * a[j] * sin(temp);
            fwrite(&data,2,1,file_out);
            a[j] *= oooQ[j];
        }
    }
    a[0] = oooA[0];
    a[1] = oooA[1];
    a[2] = oooA[2];
    for (i=0;i<SRATE;i++) {
        data = 0;
        total = total+1;
        for (j=0;j<3;j++)   {
            temp = 6.28 * (double) total * oooF[j] / (double) SRATE;
            data += 8192.0 * a[j] * sin(temp);
            a[j] *= oooQ[j];
        }
        fwrite(&data,2,1,file_out);
    }
    for (i=0;i<SRATE*2;i++)     {
        if (i%period == 0)       {
            a[0] += oooA[0];
            a[1] += oooA[1];
            a[2] += oooA[2];
        }
        data = 0;
        total = total+1;
        for (j=0;j<3;j++)   {
            temp = 6.28 * (double) total * oooF[j] / (double) SRATE;
            data += 2048.0 * a[j] * sin(temp);
            a[j] *= oooQ[j];
        }
        fwrite(&data,2,1,file_out);
    }
    data = 0;
    for (i=0;i<SRATE/2;i++)     {
         total = total + 1;
         fwrite(&data,2,1,file_out);
    }


    closesoundout(file_out,total);
}
   
