#include <stdlib.h>
// #include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/******
  convert wave file to a raw (normalized) float file
*****/
        
#include "../../waveio.h"

int main(int argc,char *argv[])
{
    FILE *fileIn, *fileOut;
    long i = 0,total = 0;
    short data_out;
    float max=0.00001,data_in;
    struct soundhdr hdr;
    
    if (argc<3) {
        fprintf(stderr,"usage: float2wv filein.flt fileout.wav(snd) [-s srate] [-g gain]\n");
        fprintf(stderr,"       default srate = 22050\n");
        fprintf(stderr,"       gain is normalization factor\n");
        fprintf(stderr,"       default = 1.0 = auto-normalization\n");
        exit(0);
    }

    fileIn = fopen(argv[1],"rb");
    fillheader(&hdr,22050);

    if (fileIn)      {

        total = 0;
        while(fread(&data_in,1,4,fileIn))   {
            total++;
            if (fabs(data_in) > max) max = fabs(data_in);
        }
        fseek(fileIn,0,SEEK_SET);

        max = 32000.0 / max;
        if (argc>3) {
            i = 3;
            while (i < argc)    {
                if (argv[i][1]=='g') {
                    if (atof(argv[i+1]) == 0.0)
                        max = 1.0;
                    else
                        max = max * atof(argv[i+1]);
                }
                if (argv[i][1]=='s')    
                    hdr.srate = atoi(argv[i+1]);
                i += 1;
            }
        }
        fileOut = opensoundout(argv[2],&hdr);
        if (!fileOut)   {
            printf("File Problems\n");
            fclose(fileIn);
            exit(0);
        }
        while(fread(&data_in,1,4,fileIn))   {
            data_out = data_in * max;
            fwrite(&data_out,2,1,fileOut);
        }
    }
    else {
        printf("File Problems\n");
    }

    fclose(fileIn);
    closesoundout(fileOut,total);
    return 1;
}

