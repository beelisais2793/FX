/********************************************************/
/***    8 bit log decompression code for              ***/
/***    originally written by Perry R. Cook for       ***/
/***    SIGGRAPH 1998 audio compression course.       ***/
/***    This program converts files created           ***/
/***    with mulaw8.c back into 16 bit linear sound   ***/
/********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../waveio.h"

#define MU 0.062
#define SCALE 0.060154

short deMuLaw(unsigned char inByte)     {
    short Sign;
    float temp;
    short dataOut;
    if (inByte & 128)                   // Then check sign
        Sign = 1;
    else 
        Sign = 0;
    inByte = inByte & 127;           // Absolute data
    temp = (float) inByte * SCALE;      // Undo mulaw
    temp = (exp(temp) - 1.0) / MU;         //   compression
    if (Sign) temp = -temp;                //     and add sign
    dataOut = temp;
    return dataOut;
}

void main(short argc, char *argv[]) {
    FILE *file_in,*file_out;
    unsigned short data_in;
    unsigned char char_data;
    struct soundhdr hdr;
    short data_out[2] = {0.0};
    long i = 0;

    if (argc==3)        {
        file_in = fopen(argv[1],"rb");
        if (!file_in)   {
            printf("Input file problem\n");
            exit(0);
        }
        fread(&hdr,44,1,file_in);
#ifdef LITTLENDIAN
        hdr.format_tag = 1;
#endif
        /*  change the format back to PCM  */
        file_out = opensoundout(argv[2],&hdr);
        if (file_out){
            while(fread(&data_in,2,1,file_in)) {     // Read compressed bytes
                char_data = data_in >> 8;              // Get first byte
                data_out[0] = deMuLaw(char_data);
                char_data = data_in & 255;             // Get second byte
                data_out[1] = deMuLaw(char_data);
                i += fwrite(data_out,2,2,file_out);
	    }
        }
	else {
	    fprintf(stderr,"File troubles.  Check names, paths, etc.\n");
	}
        fclose(file_in);
        closesoundout(file_out,i);
    }
    else {
        fprintf(stderr,"format is: demulaw inputFile.mu outputFile.wav\n");
    }
    exit(0);
}
