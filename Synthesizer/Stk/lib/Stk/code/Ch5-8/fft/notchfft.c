/**************************************/
/*****  FFT Hack, by Perry Cook, 2000 */
/*****  This here filters sinusoidal  */
/*****  bands from an fft file.  You  */
/*****  make the fft file from a .wav */
/*****  file using the fft program    */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct ffthdr {
  char fftc[4];     // .fft
  long fftsize;     // fft size
  long windowsize;  // windowsize (<= fftsize)
  long hopsize;     // hop size (<=fftsize) 
  long dlength;     // original file data length in samples
  long srate;       // original file sampling rate
};

long findLeft(float* array,long loc,long length)   {     ///  CHECK LIMITS!!
    while (loc > 0 && array[loc-1] <= array[loc]) loc--;
    return loc;    
}

long findRight(float* array,long loc,long length)  {     ///  CHECK LIMITS!!
    while ((loc < length-1) && array[loc+1] <= array[loc]) loc++;
    return loc;    
}

void main(int argc, char *argv[])   {
    float *fftData;
	float *fftData2;
    float *magData;
    float *freqList;
    FILE *input, *output;
    int numFreqs=0, filtering=0;
    float temp,temp2,max,norm,nearFloor;
    long length,winWidth,tempi;
    long i,j,k,leftEnd,rightEnd;
    struct ffthdr fhdr =
        {".fft",256,256,256,0,22050};

    if (argc < 3)   {
        printf("useage: filtfft filein.fft fileout.fft -k# killList or -f# filtList\n");
                printf("   killList is list of frequencies to disallow\n");
                printf("   filtList is list of frequencies to only allow\n");
		exit(0);
    }
    else {
        input = fopen(argv[1], "rb");
        output = fopen(argv[2], "wb");
        if (!input || !output)    {
            printf("File Problems!!\n");
            exit(0);
        }
        i = 3;
        while (i < argc)        {
            if (argv[i][1] == 'k')      {
                filtering = -1;
                numFreqs = atoi(&argv[i][2]);
                freqList = (float *) malloc(4*numFreqs);
                for (j=0;j<numFreqs;j++)    {
                    freqList[j] = (float) atof(argv[i+1]);
                    i += 1;
                }
            }
            else if (argv[i][1] == 'f')      {
                filtering = 1;
                numFreqs = atoi(&argv[i][2]);
                freqList = (float *) malloc(4*numFreqs);
                for (j=0;j<numFreqs;j++)    {
                    freqList[j] = (float) atof(argv[i+1]);
                    i += 1;
                }
            }
            else 
                i += 1;
        }
        fread(&fhdr,4,6,input);
        length = fhdr.fftsize;
        fftData = (float *) malloc(4*length);
        if (filtering==1) fftData2 = (float *) malloc(4*length);
		magData = (float *) malloc(4*length/2);
        winWidth = 2 * (fhdr.fftsize / fhdr.windowsize); //window main lobe width
        fwrite(&fhdr,4,6,output);
        for (j=0;j<numFreqs;j++)        {
            freqList[j] = (long) (freqList[j] * (float) length / fhdr.srate + 0.5);
        }
        while (fread(fftData,fhdr.fftsize,4,input))       {
            for (i=0;i<length/2;i++)   {
                temp = fftData[i*2];
                temp *= temp;
                temp2 = fftData[i*2+1];
                temp2 *= temp2;
                magData[i] = (float) sqrt(temp + temp2);  // Compute Magnitudes
             }
             if (filtering == 1)       {
				 for (i=0;i<length;i++)	fftData2[i] = fftData[i];
             }
			 for (j=0;j<numFreqs;j++)       {
				tempi = freqList[j];
				max = magData[tempi];
				leftEnd = tempi - 2*winWidth;
				if (leftEnd < 0) leftEnd = 0;
				rightEnd = tempi + 2*winWidth;
				if (rightEnd >= length/2) rightEnd = length/2;
				for (k=leftEnd;k<rightEnd;k++)	{
					if (magData[k] > max)	{
						tempi = k;
						max = magData[k];
					}
				}
				leftEnd = findLeft(magData,tempi,length/2);
                rightEnd = findRight(magData,tempi,length/2);
                if (leftEnd != rightEnd)   {
					nearFloor = magData[leftEnd];
                    if (magData[rightEnd] < nearFloor)
						nearFloor = magData[rightEnd];
                    for (k=leftEnd;k<rightEnd;k++) {
						norm = nearFloor / magData[k];
						magData[k] *= norm;     //  FIX THESE
                        fftData[2*k] *= norm;
                        fftData[2*k+1] *= norm;
                    }
				}
             }
			 if (filtering == 1)	{
				 for (i=0;i<length;i++)
					 fftData[i] = fftData[i] - fftData2[i];
			 }
			 fwrite(fftData,fhdr.fftsize,4,output);
			 printf(".");
        }
        printf("\n");
        fclose(input);
        free(fftData);
		if (filtering ==1) free(fftData2);
        fclose(output);
        free(freqList);
		free(magData);
    }   
}


