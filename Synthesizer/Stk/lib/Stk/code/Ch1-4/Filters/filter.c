/*   General Pole-Zero digital filter program by Perry R. Cook
     Originally written at the Stanford Center for Computer
     Research in Music and Acoustics (CCRMA), sometime in 1992

  The program takes a filter specification as a text file in the format:
	
order
inCoeff1 inCoeff2 inCoeff3 . . . inCoeffOrder
outCoeff1 outCoeff2 outCoeff3 . . . outCoeffOrder
gainCoeff

  and applies the digital filter to an input soundfile to yield an
  output soundfile.  All calculations are double precision floating point.

	Hacked for .wav files in summer 2001, PRC.	
        Hacked for .wav and/or .snd files in 2002, PRC.                */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAX_COEFFS 40
#define BUFFER_SIZE 2048

int order;
double inCoeffs[MAX_COEFFS];
double outCoeffs[MAX_COEFFS];
double outNorm;;
double inZ[MAX_COEFFS] = {0.0};
double outZ[MAX_COEFFS] = {0.0};

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
    return output * outNorm;
}

void getFloats(int order, char* inputString, double* coeffs)     {
    char *strPt;
    long i;

    strPt = inputString;
    for (i=0;i<order;i++)	{
        coeffs[i] = (double) atof(strPt);
        strPt = (char *) strchr(strPt+1,' ');
        printf("%f ",coeffs[i]);
    }
    printf("\n");
}

#include "../../waveio.h"

int main(int argc,char *argv[])     /* apply fixed filter to a file */
{
    short data[BUFFER_SIZE];
    FILE *out_file,*in_file;
    long mode,i,j,numRead;
    char inputString[2048],*strPt;
    struct soundhdr hdr;

    if (argc<4) {
        fprintf(stderr,"usage: filter specFile filein fileout\n");
	exit(0);
    }
    else {
        printf("Filtering by %s file %s to output file %s\n",argv[1],argv[2],argv[3]);
    }
        in_file = fopen(argv[1],"rb");
    if (!in_file) 	{
        fprintf(stderr,"Can't open spec_file!!\n");
	exit(0);
    }

    fgets(inputString,2048,in_file);
    printf("Order: %i\n",order);
    order = atoi(inputString);
    fgets(inputString,2048,in_file);
    printf("FIR: ");
    getFloats(order, inputString, inCoeffs);
    fgets(inputString,2048,in_file);
    printf("IIR: ");
    getFloats(order, inputString, outCoeffs);
    fgets(inputString,2048,in_file);
    outNorm = (double) atof(inputString);
    printf("Gain: %f\n",outNorm);
    fclose(in_file);

    in_file = opensoundin(argv[2],&hdr);
    if (!in_file) 	{
        fprintf(stderr,"Can't open input file!!\n");
	exit(0);
    }

    out_file = opensoundout(argv[3],&hdr);
    if (!out_file) {
        fprintf(stderr,"Can't open output file!!!\n");
	fclose(in_file);
	exit(0);
    }    
    
    clear_filt();

    numRead = fread(data,2,BUFFER_SIZE,in_file);
    while (numRead>0)    {
        printf(".");
        for (i=0;i<numRead;i++)   {
            data[i] = filt_tick(data[i]);
        }
        fwrite(data,2,numRead,out_file);
        numRead = fread(data,2,BUFFER_SIZE,in_file); 
    }
    printf("\n");
    fclose(out_file);
    fclose(in_file);
    return 1;
}

