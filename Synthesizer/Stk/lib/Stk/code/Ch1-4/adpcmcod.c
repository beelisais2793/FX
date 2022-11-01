/********************************************************/
/***    4 bit ADPCM compression code                  ***/
/***    originally written by Perry R. Cook for       ***/
/***    SIGGRAPH 1998 audio compression course.       ***/
/***    This program takes a 16 bit linear signed     ***/
/***    sound file and compresses it 4:1.             ***/
/***    Use adpcmdec.c to decompress files.           ***/
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
    long i;
    short XHAT1=0,Xn,Dn;
    short deln,del1=8;
    short delndec,del1dec=8,ln1dec;
    short ln1=1,sign,temp;
    short data[4];
    unsigned short data_out;
    struct soundhdr hdr;
    float M[16] = {0.909,0.909,0.909,0.909,1.21,1.4641,1.771561,2.143589,
		   0.909,0.909,0.909,0.909,1.21,1.4641,1.771561,2.143589};
    float del_table[16] = {0.0,0.25,0.5,0.75,1.0,1.25,1.5,1.75,
			0.0,-0.25,-0.5,-0.75,-1.0,-1.25,-1.5,-1.75};

    if (argc==3)        {
        if (argv[2][strlen(argv[2])-1] != 'p')   {
            printf("You should be creating a .adp file for output\n");
            exit(0);
        }
        file_in = opensoundin(argv[1],&hdr);
        if (!file_in)   {
            fprintf(stderr,"Input File Error\n");
            exit(0);
        }
#ifdef LITTLENDIAN
        hdr.format_tag = 259;
#endif
    /* we're not creating a valid adpcm .wav file here.  we're just */
    /* scamming the header to preserve sample rate, etc. we only hack */
    /* the format field, so nobody tries to play it as valid 16bit PCM. */
        file_out = fopen(argv[2],"wb");
        if (file_out) {
            fwrite(&hdr,44,1,file_out);
	    while(fread(&data,2,4,file_in)) {
		data_out = 0;
		for (i=0;i<4;i++)       {
		    Xn = data[i];
		    Dn = Xn - XHAT1;                  //   Form delta signal
		
		    deln = del1 * M[ln1];             //   Get new delta
		    temp = deln - MIN_STEP;           //   delta < min ???
		    if (temp<0)    {                  //   Skip 1 on positive
			deln = MIN_STEP;              //   set it to min
		    }    
		    temp = MAX_STEP - deln;           //   delta > max ???
		    if (temp<0) {                     //   Skip 1 on positive
			deln = MAX_STEP;              //   set it to max
		    }
		    del1 = deln;                      //   Stash delta
		
		    ln1 = 0;
			 
		    temp = Dn;                        //   to set flags
		    if (temp<0)  {                    //   Skip on positive
			ln1 = 8;                      //   Else set negative
			Dn = -Dn;                     //      bit in code word
		    }
		    temp = deln - Dn;                 //   Check magnitude
		    if (temp<0)  {                    //   Skip 2 on positive
			ln1 += 4;                     //   set MSB in code word
			Dn -= deln;                   //   decrease magnitude
		    }
		    temp = deln*0.5 - Dn;             //   Check magnitude
		    if (temp<0)       {               //   Skip 2 on positive
			ln1 += 2;                     //   set 2SB in code word
			Dn -= deln*0.5;               //   decrease magnitude
		    }
		    temp = deln*0.25 - Dn;            //   Check magnitude
		    if (temp<0)   {                   //   Skip 2 on positive
			ln1 += 1;                     //   set LSB in code word
		    }
		    
/*******************  DECODER EMULATION  *********************/
		    
		    ln1dec = ln1;
		    
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
		    
/*************************************************************/                    
		    
		    data_out = (data_out << 4) + ln1;
		}
		fwrite(&data_out,2,1,file_out);
	    }
	}
	else {
	    fprintf(stderr,"File troubles.  Check names, paths, etc.\n");
	}
	fclose(file_in);
	fclose(file_out);
    }
    else {
        fprintf(stderr,"format is: adpcmcod inputFile.wav(snd) outputFile.adp\n\n");
	exit(0);
    }
    exit(0);
}


