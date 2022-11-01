/********************************************************/
/***    8 bit log compression code for                ***/
/***    Originally created by Perry R. Cook           ***/
/***    for SIGGRAPH 1998 audio compression course.   ***/ 
/***        by Perry R. Cook, Princeton University    ***/
/***    This program takes a 16 bit linear sound      ***/
/***    file and compresses it 2:1.                   ***/
/***    Use demulaw8.c to decompress files.           ***/
/********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../waveio.h"

#define MU 0.062
#define SCALE 0.060154

/** WE USE THIS ROUTINE FOR SIGNED MAGNITUDE STORAGE **/
unsigned short doMuSignedMag(short inShort) {
    unsigned short abslut,result;
    float temp;
    short Sign=0;

    abslut = abs(inShort);                   // Absolute of data
    Sign = (inShort<0);                      // Sign of data
    temp = log(1.0 + (MU * (float) abslut)); // Basic Mulaw
    temp /= SCALE;                           // equation
    result = (unsigned short) temp;          // Compute
                                             // second byte
    if (Sign) result = result | 128;         // with sign bit
    return result;
}

void main(short argc, char *argv[]) {
    FILE *file_in,*file_out;
    struct soundhdr hdr;
    unsigned short data_out;
    long i=0;
    short data_in[2];
    
    if (argc==3)        {
        if (argv[2][strlen(argv[2])-1] != 'u')   {
            printf("You should be creating a .mu file for output\n");
            exit(0);
        }
        file_in = opensoundin(argv[1],&hdr);
        if (!file_in)   {
            fprintf(stderr,"Input file error\n");
        }
#ifdef LITTLENDIAN
        hdr.format_tag = 257;
#endif
    /* we're not creating a valid mulaw .wav file here.  we're just */
    /* scamming the header to preserve sample rate, etc. we only hack */
    /* the format field, so nobody tries to play it as valid 16bit PCM. */
        file_out = fopen(argv[2],"wb");
        if (file_out){
            fwrite(&hdr,44,1,file_out);
            while(fread(&data_in,2,2,file_in)) {         // Load two words
                data_out = doMuSignedMag(data_in[0]);
                data_out = (data_out << 8) + doMuSignedMag(data_in[1]);
                i += 2;
                fwrite(&data_out,2,1,file_out);          // write out in sets of 4
	    }
	}
	else {
	    fprintf(stderr,"File troubles.  Check names, paths, etc.\n");
	}
	fclose(file_in);
        fclose(file_out);
    }
    else {
        fprintf(stderr,"format is: mulaw inputFile.wav outputFile.mu\n");
    }
    exit(0);
}

/*******************   NOTE ************************/
/**  WE COULD USE THIS FOR UNSIGNED BYTE MAPPING  **/
/**  THIS IS THE MORE INDUSTRY STANDARD BIT PACK  **/
/***************************************************/

unsigned short doMu(short inShort)       {
    unsigned short abslut,result;
    float temp;
    short Sign=0;

    abslut = abs(inShort);                // Absolute of data
    Sign = (inShort<0);                   // Sign of data
    temp = log(1.0 + (MU * (float) abslut)); // Basic Mulaw
    temp /= SCALE;                           // equation
    result = (unsigned short) temp;          // Compute 
    if (Sign) {
        result = 127-result;         //   output byte
    }
    else        {
        result = 255-result;
    }
    return result;
}


