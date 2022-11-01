#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "../waveio.h"

/**********  REVERSE a SOUNDFILE   */

int main(int argc,char *argv[])
{
    FILE *fileIn, *fileOut;
    short *x_array,data;
    long i,x_length;
    struct soundhdr hdr;
    
    if (argc!=3) {
        fprintf(stderr,"usage: reverse filein.wav(snd) fileout.wav(snd)\n");
        exit(0);
    }
    printf("Reversing %s to output file %s\n",argv[1],argv[2]);


    fileIn = opensoundin(argv[1], &hdr);
    fileOut = opensoundout(argv[2],&hdr);
    if (fileIn && fileOut)      {
        fseek(fileIn,0,SEEK_SET);		/*  THIS AVOIDS A BUG in REDHAT 6.0 */
        fread(&hdr,44,1,fileIn);

        x_length = hdr.dlength/2;

        fwrite(&hdr,44,1,fileOut);

        x_array = (short *) malloc(2*x_length);
        fread(x_array,2,x_length,fileIn);
        
        for (i=x_length-1;i>=0;i--)  {
            data = x_array[i];
             fwrite(&data,2,1,fileOut);
	}
    }
    else {
       printf("File Problems\n");
    }
    free(x_array);
    fclose(fileIn);
    fclose(fileOut);

    return 1;
}
