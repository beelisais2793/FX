/*
     Code to apply the biquad filters of Figure 3.14 repeatedly
     to a soundfile.  8k sample rate is assumed.
     Perry R. Cook, 2002
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int order = 2;
double outGain;
double inCoeffs[2] = {0.0,-1.0};
double outCoeffs[2] = {0.0};
double inZ[2] = {0.0};
double outZ[2] = {0.0};

void clear_filt()
{
    long i;
    for (i=0;i<order;i++)	{
        inZ[i] = 0.0;
	outZ[i] = 0.0;
    }
}

double filt_tick(double input)
{
    long i;
    double output;
    output = input;
    for (i=order-1;i>0;i--) {
        output +=  (inCoeffs[i] * inZ[i]) - (outCoeffs[i] * outZ[i]);
  	inZ[i] = inZ[i-1];
	outZ[i] = outZ[i-1];
    }
    output +=  (inCoeffs[0] * inZ[0]) - (outCoeffs[0] * outZ[0]);
    inZ[0] = input;
    outZ[0] = output;
    return output * outGain;
}

#include "../../waveio.h"

#define BUFFER_SIZE 2048

int main(int argc,char *argv[])     /* apply filters to a file */
{
    short data[BUFFER_SIZE];
    FILE *out_file,*in_file;
    long mode,i,j,total=0,numRead;
    struct soundhdr hdr;

    if (argc<3) {
        fprintf(stderr,"usage: doresons filein fileout\n");
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

    outCoeffs[1] = 0.99 * 0.99;
    outGain = 0.01;
    for (i=0;i<4001;i+=500)        {
        clear_filt();
        outCoeffs[0] = -2.0*0.99*cos(6.283*(float)i/8000.0);
        numRead = fread(data,2,BUFFER_SIZE,in_file);
        while (numRead>0)    {
            printf(".");
            for (j=0;j<numRead;j++)   {
                data[j] = filt_tick(data[j]);
                total += 1;
            }
            fwrite(data,2,numRead,out_file);
            numRead = fread(data,2,BUFFER_SIZE,in_file);
        }
        fclose(in_file);
        in_file = opensoundin(argv[1],&hdr);
    }
    printf("\n");

    outCoeffs[1] = 0.97 * 0.97;
    outGain = 0.03;
    for (i=0;i<4001;i+=500)        {
        clear_filt();
        outCoeffs[0] = -2.0*0.97*cos(6.283*(float)i/8000.0);
        numRead = fread(data,2,BUFFER_SIZE,in_file);
        while (numRead>0)    {
            printf(".");
            for (j=0;j<numRead;j++)   {
                data[j] = filt_tick(data[j]);
                total += 1;
            }
            fwrite(data,2,numRead,out_file);
            numRead = fread(data,2,BUFFER_SIZE,in_file);
        }
        fclose(in_file);
        in_file = opensoundin(argv[1],&hdr);
    }
    printf("\n");
    closesoundout(out_file,total);
    fclose(in_file);
    return 1;
}

