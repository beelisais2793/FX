/*************************************************************/
/*****  Quick .wav and .fft header diagnostic program        */
/*****  by Perry Cook, Fall 2002                             */
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../waveio.h"

struct ffthdr {
  char fftc[4];     // .fft
  long fftsize;     // fft size
  long windowsize;  // windowsize (<= fftsize)
  long hopsize;     // hop size (<=fftsize) 
  long dlength;     // original file data length in samples
  long srate;       // original file sample rate 
};

void main(int argc, char *argv[])   {
    FILE *input;
    struct soundhdr hdr;
    struct ffthdr fhdr =
        {".fft",1024,512,256,0,0};

    if (argc < 2)   {
        printf("useage: headers filein.wav<fft>\n");
        printf("        Prints header data for .wav (.snd) or .fft files.\n");
        exit(0);
    }
#ifdef LITTLENDIAN
    if (argv[1][strlen(argv[1])-1]=='v')      {     // "wav"
        input = opensoundin(argv[1], &hdr);
        if (input)  {
            printf("File length in bytes = %li\n",hdr.flength);
            printf("Block size in bytes  = %li\n",hdr.block_size);
            printf("Format type            %i\n",hdr.format_tag);
            printf("    (1=PCM 257=Mu-law 258=A-Law 259=ADPCM)\n");
            printf("Number of channels   = %i\n",hdr.num_chans);
            printf("Sample rate          = %li\n",hdr.srate);
            printf("Bytes per second     = %li\n",hdr.bytes_per_sec);
            printf("Bytes per sample     = %i\n",hdr.bytes_per_samp);
            printf("Bits per second      = %i\n",hdr.bits_per_samp);
            printf("Data length in bytes = %li\n",hdr.dlength);
            fclose(input);
        }
        else {
            printf("Can't open input file!!\n");
            exit(0);
        }
#else
    if (argv[1][strlen(argv[1])-1]=='d')      {     // "snd"
        input = opensoundin(argv[1], &hdr);
        if (input)  {
            printf("Header length in bytes = %li\n",hdr.hdr_length);
            printf("Data length in bytes   = %li\n",hdr.dlength);
            printf("Data type/format/mode  = %li\n",hdr.mode);
            printf("    1=16bit linear, 3=8bit Mu-law???\n");
            printf("Sample rate            = %li\n",hdr.srate);
            printf("Number of channels     = %li\n",hdr.num_chans);
  //   ADD THIS SOME DAY char comment[20];
            fclose(input);
        }
        else {
            printf("Can't open input file!!\n");
            exit(0);
        }
#endif
    }
    else {
        input = fopen(argv[1], "rb");
        if (input)  {
            fread(&fhdr,4,6,input);
            printf("FFT size                  = %li\n",fhdr.fftsize);
            printf("Window size               = %li\n",fhdr.windowsize);
            printf("Hop size                  = %li\n",fhdr.hopsize);
            printf("Original file data size   = %li\n",fhdr.dlength);
            printf("Original file sample rate = %li\n",fhdr.srate);
            fclose(input);
        }
        else {
            printf("Can't open input file!!\n");
            exit(0);
        }
    }
}
