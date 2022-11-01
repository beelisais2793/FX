/*************************************************************/
/*****  FFT based cross-synthesis channel vocoder emulator   */
/*****  program, by Perry Cook, 2002                         */
/*****  Takes two .fft files (use fft program to convert     */
/*****  sound files to .fft files.  The spectral energy in   */
/*****  octave subbands of the "Filter" file is imparted on  */
/*****  the "Source" file.   Optional high-freq. "sibilance  */
/*****  band" pass through is available, which passes source */
/*****  high frequency sound directly through to output.     */
/*************************************************************/

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
  long srate;       // original file sample rate 
};

void main(int argc, char *argv[])   {
    float *source,*filter,*result,*tempres;
    float filt,sourc,filtot,sourctot,norm,binwidth;
    long i,j,reI,imI,highband,lowband,botband,time,bandwidth;
    long fsize,wsize,hsize,nread;
    int sibilance;
    int ok;
    FILE *inputS, *inputF, *output;
    struct ffthdr fhdrS =
        {".fft",1024,512,256,0,0};
    struct ffthdr fhdrF =
        {".fft",1024,512,256,0,0};

    if (argc < 4)   {
        printf("useage: vocod sourceFile.fft filterFile.fft outputFile.fft [-s]\n");
        printf("        imparts spectral shape of filter file\n");
        printf("        onto source file\n");
        printf(" -s switch causes direct sibilance band (4k+) pass\n");
        exit(0);
    }
    sibilance = 0;
    if (argc == 5)
        if (argv[4][1] == 's')
            sibilance = 1;
    if (sibilance) 
        printf("Setting sibilance pass-through\n");
    else
        printf("No direct sibilance band\n");
    
    inputS = fopen(argv[1],"rb");
    inputF = fopen(argv[2],"rb");
    output = fopen(argv[3],"wb");
    ok = 1;
    if (!inputS) ok = 0;
    if (!inputF) ok = 0;
    if (!output) ok = 0;
    fread(&fhdrS,4,6,inputS);
    fread(&fhdrF,4,6,inputF);
    if (fhdrS.fftsize != fhdrF.fftsize) ok = 0;
    if (fhdrS.windowsize != fhdrF.windowsize) ok = 0;
    if (fhdrS.hopsize != fhdrF.hopsize) ok = 0;

    if (!ok)    {
        printf("Problem!!! either opening files!!\n");
        printf("           or fft sizes (window, etc.) are not same\n");
        fclose(inputS);
        fclose(inputF);
        fclose(output);
        exit(0);
    }

    fwrite(&fhdrF,4,6,output);
    fsize = fhdrS.fftsize;
    wsize = fhdrS.windowsize;
    hsize = fhdrS.hopsize;

    source = (float *) calloc(2,4 * fsize);
    filter = (float *) calloc(2,4 * fsize);
    time = 0;
    binwidth = fhdrS.srate / fhdrS.fftsize;
    botband = 20.0 / binwidth;
    while (nread = fread(source,4,fsize,inputS))    {
        fread(filter,4,fsize,inputF);
        printf(".");

        highband = 4000.0 / binwidth;
//        highband = 2 * (highband / 2);          // Make sure even number
        if (sibilance)  {
            for (i=highband;i<fsize;i++)    {   // Pass through sibilance
                source[i] = filter[i];
            }
        }
        else    {
            highband = fsize - 1;
        }
        lowband = highband / 2;
//        lowband = 2 * (lowband / 2);            // Make sure even number
        while (lowband > botband)      {        // Do subbands down to 20 hz.
            filtot = 0;
            sourctot = 1.0;                //  base energy to avoid /0
            bandwidth = highband - lowband;
            for (i=lowband;i<highband;i++) {
                reI = 2*i;
                imI = reI+1;
                filt = filter[reI]*filter[reI] +
                        filter[imI]*filter[imI];
                filtot += filt;                 // Square power in filter band
                sourc = source[reI]*source[reI] +
                        source[imI]*source[imI]; 
                sourctot += sourc;              // Square power in source band
            }
            sourctot = sqrt(sourctot / bandwidth);
            filtot = sqrt(filtot / bandwidth);
            norm = filtot / sourctot;           // Make source match filt
            for (i=lowband;i<highband;i++) {
                source[2*i] *= norm;            // Do cross-synthesis
                source[2*i+1] *= norm;
            }
            highband = lowband;                 // Go down by octaves
//            highband = 2 * (highband / 2);      // Make sure even number
            lowband = highband / 2;
//            lowband = 2 * (lowband / 2);        // Make sure even number
        }
        for (i=0;i<=botband;i++) source[i] = 0.0;

        fwrite(source,4,fsize,output);
    }
    printf("\n");
    fclose(inputS);
    fclose(inputF);
    fclose(output);
    free(source);
    free(filter);    
}


