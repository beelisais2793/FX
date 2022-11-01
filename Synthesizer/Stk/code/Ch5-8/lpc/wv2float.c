#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/******
  convert wave/snd file to a raw normalized float file
*****/
        
#include "../../waveio.h"

int main(int argc,char *argv[])
{
    FILE *fileIn, *fileOut;
    long i;
    short data;
    float max=1.0,data_out;
    struct soundhdr hdr;
    
    if (argc!=3) {
        fprintf(stderr,"usage: wv2float filein.wav(snd) fileout.flt\n");
        exit(0);
    }

    fileIn = opensoundin(argv[1],&hdr);
    fileOut = fopen(argv[2],"wb");
    if (fileIn && fileOut)      {
        for (i=0;i<hdr.dlength/2;i++)   {
            fread(&data,1,2,fileIn);
            if (abs(data) > max) max = abs(data);
        }
        fseek(fileIn,44,SEEK_SET);

        for (i=0;i<hdr.dlength/2;i++)   {
            fread(&data,1,2,fileIn);
            data_out = (float) data / max;
            fwrite(&data_out,4,1,fileOut);
        }
    }
    else {
        printf("File Problems\n");
    }

    fclose(fileIn);
    fclose(fileOut);
    return 1;
}

