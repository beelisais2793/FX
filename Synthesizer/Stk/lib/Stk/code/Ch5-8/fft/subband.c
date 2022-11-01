/**************************************/
/*****  FFT Hack, by Perry Cook, 2000 */
/*****  This here prints out the N    */
/*****  most significant peaks in     */
/*****  each frame of an fft file     */
/*****  It can also do a quickie      */
/*****  sinusoidal resynthesis at the */
/*****  the same time.                */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct ffthdr {
  char fftc[4];     // .fft
  long fftsize;     // fft size
  long windowsize;  // windowsize (<= fftsize)
  long hopsize;     // hop size (<=fftsize) 
  long dlength;     // original file data length in samples
  long srate;       // original file sampling rate
};

void main(int argc, char *argv[])   {
    float *fftData;
    float *outData;
    FILE *input, *output;
    long i,j,length,lowbin,highbin,ramplen;
    float delta,amp;
    unsigned char tc = '0';
    char tempname[256];
    struct ffthdr fhdr =
        {".fft",256,256,256,0,22050};

    if (argc < 3)   {
        printf("useage: subband filein.fft filesout\n");
                exit(0);
    }
    else {
        input = fopen(argv[1], "rb");
    }
    if (input)        {
        fread(&fhdr,4,6,input);
        strcpy(tempname,argv[2]);
        tempname[strlen(tempname)-4] = tc;
        tempname[strlen(tempname)-3] = 0;
        strcat(tempname,".fft");
        output = fopen(tempname,"wb");
        if (!output)    {
            printf("Can't open output file!!\n");
            fclose(input);
            exit(0);
        }
        fclose(output);
        fftData = (float *) malloc(4*length);
        length = fhdr.fftsize;
        highbin = length / 256;
        if (highbin < 1) highbin = 1;
        lowbin = highbin / 2;
        j = 0;
        while (highbin < length)        {
            strcpy(tempname,argv[2]);
            tempname[strlen(tempname)-4] = tc;
            tempname[strlen(tempname)-3] = 0;
            strcat(tempname,".fft");
            printf(tempname);
            printf("\n");
            output = fopen(tempname,"wb");
            fwrite(&fhdr,4,6,output);
            while (fread(fftData,4,length,input))       {
                for (i=0;i<lowbin-ramplen;i++)   {
                    fftData[2*i] = 0;
                    fftData[2*i+1] = 0;
                }
                ramplen = (highbin-lowbin)/2;
                delta = 1.0 / (float) ramplen;
                amp = 0.0;
                for (i=lowbin-ramplen;i<lowbin;i++)  {
                    fftData[2*i] *= amp;
                    fftData[2*i+1] *= amp;
                    amp += delta;
                }
                for (i=highbin;i<highbin+ramplen;i++)      {
                    if (i<length)       {
                        fftData[2*i] *= amp;
                        fftData[2*i+1] *= amp;
                        amp -= delta;
                    }                 
                }
                for (i=highbin+ramplen;i<length;i++)   {
                    fftData[2*i] = 0;
                    fftData[2*i+1] = 0;
                }

/*                for (i=0;i<lowbin;i++)   {
                    fftData[2*i] = 0;
                    fftData[2*i+1] = 0;
                }
                for (i=highbin;i<length;i++)   {
                    fftData[2*i] = 0;
                    fftData[2*i+1] = 0;
                }
*/
                fwrite(fftData,4,length,output);
            }
            lowbin = highbin;
            highbin *= 2;
            tc += 1;
            fclose(output);
            fseek(input,24,SEEK_SET);
        }
        printf("\n");
        fclose(input);
        free(fftData);
    }
    else    {
        printf("Can't open input file!!\n");
    }
    
}


