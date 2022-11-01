/********************************************************/
/***    4 bit ADPCM decompression code                ***/
/***    originally written by Perry R. Cook for       ***/
/***    SIGGRAPH 1998 audio compression course.       ***/
/***    This program decompresses files created       ***/
/***    by adpcmcod.c                                 ***/
/********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_STEP 2048
#define MIN_STEP 16

#include "../waveio.h"

void main(short argc, char *argv[]) {
    FILE *file_in,*file_out;
    long i = 0;
    short XHAT1=0,Xn,Dn;
    short deln;
    short delndec,del1dec=8,ln1dec;
    short sign,temp;
    short data[4];
    unsigned short data_in;
    struct soundhdr hdr;
    float M[16] = {0.909,0.909,0.909,0.909,1.21,1.4641,1.771561,2.143589,
		   0.909,0.909,0.909,0.909,1.21,1.4641,1.771561,2.143589};
    float del_table[16] = {0.0,0.25,0.5,0.75,1.0,1.25,1.5,1.75,
			0.0,-0.25,-0.5,-0.75,-1.0,-1.25,-1.5,-1.75};

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
        file_out = opensoundout(argv[2],&hdr);
        if (file_out){
            while(fread(&data_in,1,2,file_in)) {
		for (i=0;i<4;i++)       {
		    ln1dec = data_in >> 12;               //  Load data
		    data_in = data_in << 4;
		    
		    delndec = del1dec * M[ln1dec];        //  form new delta
		    temp = delndec - MIN_STEP;            //  test against min
		    if (temp<0)                           //  Skip 1 on positive
			delndec = MIN_STEP;               //  set to min
		    temp = MAX_STEP - delndec;            //  test against max
		    if (temp<0)                           //  Skip 1 on positive
			delndec = MAX_STEP;               //  set to max
		    del1dec = delndec;                    //  save delta

		    XHAT1 += delndec * del_table[ln1dec]; //  do sample update

		    if (XHAT1>32000 || XHAT1<-32000)      //  Skip if no saturate
			XHAT1 *= 0.95;                    //  Else fix it

		    data[i] = XHAT1;                      //  Output Samples

		}
                i += fwrite(&data,4,2,file_out);
	    }
	}
	else {
	    fprintf(stderr,"File troubles.  Check names, paths, etc.\n");
	}
	fclose(file_in);
        closesoundout(file_out,i);
    }
    else {
        fprintf(stderr,"format is: adpcmdec inputFile.adp outputFile.wav(snd)\n\n");
	exit(0);
    }
    exit(0);
}

