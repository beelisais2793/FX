/**************************************************/
/*  Filters noise through a filter implemented    */
/*  using various factored topologies.            */
/**************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "waveio.h"

void main(int argc, char *argv[]) {
    long i;
    float x, w, y, x2, y1;
    short data;
    FILE *out_file,*in_file;
    struct soundhdr hdr;

/****************  DIRECT FORM *******************/
    float Z[4]={0,0,0,0};
    float a[4]={-1.2925,1.2925,-1.0,0.0};
    float b[4]={-0.2125,-1.4725,-0.20825,-0.9604};

/**********  CASCADE FACTORED FORM 1 *************/
    float Z1T1[2]={0,0},Z2T1[2]={0,0};
    float a1T1[2]={-1.0,0.0};
    float b1T1[2]={0.6,-0.98};
    float a2T1[2]={-0.2925,1.0};
    float b2T1[2]={-0.8125,-0.98};

/**********  CASCADE FACTORED FORM 2 *************/
    float Z1T2[2]={0,0},Z2T2[2]={0,0};
    float a1T2[2]={-0.2925,1.0};
    float b1T2[2]={0.6,-0.98};
    float a2T2[2]={-1.0,0.0};
    float b2T2[2]={-0.8125,-0.98};


    if (argc<3) {
        fprintf(stderr,"usage: F315-16.c filein fileout\n");
	exit(0);
    }
    in_file = opensoundin(argv[1],&hdr);
    if (!in_file) 	{
        fprintf(stderr,"Can't open input file!!\n");
	exit(0);
    }
    out_file = opensoundout(argv[2],&hdr);
    if (!out_file)       {
        fprintf(stderr,"Can't open output file!!\n");
        fclose(in_file);
        exit(0);
    }

    while (fread(&data,2,1,in_file))       {
        x = data*0.1;
        w = x + b[0]*Z[0] + b[1]*Z[1] + b[2]*Z[2] + b[3]*Z[3];
        y = w + a[0]*Z[0] + a[1]*Z[1] + a[2]*Z[2] + a[3]*Z[3];
        Z[3] = Z[2];
        Z[2] = Z[1];
        Z[1] = Z[0];
        Z[0] = w;
        data = y;
        fwrite(&data,2,1,out_file);
        i += 1;
    }
    fseek(in_file,0,SEEK_SET);

    while (fread(&data,2,1,in_file))       {
        x = data*0.1;
        w = x + b1T1[0]*Z1T1[0] + b1T1[1]*Z1T1[1];
        y1 = w + a1T1[0]*Z1T1[0] + a1T1[1]*Z1T1[1];
        Z1T1[1] = Z1T1[0];
        Z1T1[0] = w;
        x2 = y1;

        w = x2 + b2T1[0]*Z2T1[0] + b2T1[1]*Z2T1[1];
        y = w + a2T1[0]*Z2T1[0] + a2T1[1]*Z2T1[1];
        Z2T1[1] = Z2T1[0];
        Z2T1[0] = w;

        data = y;
        fwrite(&data,2,1,out_file);
        i += 1;
    }
    fseek(in_file,0,SEEK_SET);

    while (fread(&data,2,1,in_file))       {
        x = data*0.1;
        w = x + b1T2[0]*Z1T2[0] + b1T2[1]*Z1T2[1];
        y1 = w + a1T2[0]*Z1T2[0] + a1T2[1]*Z1T2[1];
        Z1T2[1] = Z1T2[0];
        Z1T2[0] = w;
        x2 = y1;

        w = x2 + b2T2[0]*Z2T2[0] + b2T2[1]*Z2T2[1];
        y = w + a2T2[0]*Z2T2[0] + a2T2[1]*Z2T2[1];
        Z2T2[1] = Z2T2[0];
        Z2T2[0] = w;

        data = y;
        fwrite(&data,2,1,out_file);
        i += 1;
    }

    closesoundout(out_file,i);

}
