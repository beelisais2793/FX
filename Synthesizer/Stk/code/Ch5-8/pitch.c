/*  Simple Autocorrelation-based pitch detection program
        by Perry R. Cook, Princeton U, 2002
    Takes a wavefile, and determines pitch.
    Also synthesizes a file psynth.wav, which contains a
    pitch pulse resynthesis of the estimated pitches.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

float autocorr(long size,short *data,float *result);

#include "../waveio.h"
#define PI 3.14159268

void main(int argc,char *argv[])
{
    struct soundhdr hdr;
    float *data;
    short *short_data,dout;
    FILE *fd,*fdout;
    long i,n_read,time=0,block=0,period,ticker;
    long block_size,hop_size;
    float pitch,power=0,temp,sgain;

    if (argc==2 || argc==3)  {
        fd = opensoundin(argv[1],&hdr);
        if (fd)     {
#ifdef LITTLENDIAN
            fdout = opensoundout("psynth.wav",&hdr);
#else
            fdout = opensoundout("psynth.snd",&hdr);
#endif
            if (argc==3) temp = atof(argv[2]);
            else temp = 0.05;
            block_size = temp * hdr.srate;
            short_data = (short *) malloc(block_size*2);
            data = (float *) malloc(block_size*4);
            hop_size = block_size / 2;
            n_read = fread(short_data,2,block_size,fd);
            ticker = 0;
            while (n_read>0)     {
                 period = autocorr(block_size,short_data,data);
                 if (period > 0)
                     pitch = (float) hdr.srate / period;
                 else {
                     pitch = -1;
                     period = hop_size;   // don't fire pitch resynth ticker
                 }
                 power = sqrt(data[0] / (float) block_size);
                 printf("%.3f ",time / (float) hdr.srate);
                 if (power > 70)      {
                     if (pitch > 0)
                         printf("%f %.3f \n",pitch,power);
                     else
                         printf("Noise %.3f\n",power);
                 }
                 else   {
                     printf("Silence......\n");
                 }
                 sgain = 5 * power;
                 for (i=0;i<block_size-hop_size;i++)  {
                     short_data[i] = short_data[hop_size+i];
                     if (ticker++ >= period)  {
                         dout = sgain;
                         ticker = 0;
                     }
                     else dout = 0;
                     fwrite(&dout,2,1,fdout);
                 }

                 n_read = fread(&short_data[block_size-hop_size],2,hop_size,fd);
                 if (n_read < hop_size)   {
                    for (i=block_size-hop_size+n_read;i<block_size;i++)  {
                         data[i] = 0;
                     }
                 }
                 time += hop_size;
                 block += 1;
               }
               printf("\n");
               fclose(fd);
               closesoundout(fdout,time);
               free(short_data);
               free(data);

             }
            else
               printf("I couldn't find your input file!!!  %s\n",argv[1]);
        }
        else   {
            printf("Format is pitch fileIn.wav [blockTime (in seconds, default 0.05)]\n");
       }
    return;
}

float autocorr(long size,short *data,float *result)
{
    long i,j,k;
    float temp,norm;

    for (i=0;i<size/2;i++)      {
        result[i] = 0.0;
	for (j=0;j<size-i-1;j++)	{
	    result[i] += data[i+j] * data[j];
	}
    }
    temp = result[0];
    j = (long) size*0.02;
    while (result[j]<temp && j < size/2)	{
	temp = result[j];
        j += 1;
    }
    temp = 0.0;
    for (i=j;i<size*0.5;i++) {
        if (result[i]>temp) {
	    j = i;
	    temp = result[i];
	}
    }
    norm = 1.0 / size;
    k = size/2;
    for (i=0;i<size/2;i++)
        result[i] *=  (k - i) * norm;
    if (result[j] == 0) j = 0;
    else if ((result[j] / result[0]) < 0.4) j = 0;
    else if (j > size/4) j = 0;
    return (float) j;
}

