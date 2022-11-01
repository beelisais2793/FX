/**************************************/
/*****  FFT Hack, by Perry Cook, 2000 */
/*****  This here prints out the N    */
/*****  most significant peaks in     */
/*****  each frame of an fft file     */
/*****  It can also do a quickie      */
/*****  sinusoidal resynthesis at the */
/*****  the same time.                */
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

#include "../../waveio.h"

void main(int argc, char *argv[])   {
    float *fftData,max;
    float *outData;
    float *freqList;
	float *freqMask;
    FILE *input, *output;
    int filtering = 0, numFreqs = 0, itemp;
    int notDone,synth=0,verbose = 1;
    long *peaks;
    float *magns;
    float temp,temp2,pitch,time,gain=1.0;
    double PI, phase = 0,delta;
    long length,maxloc,winWidth,numPeaks;
    long i,j,peak,newHop,total=0;
    short data;
    struct ffthdr fhdr =
        {".fft",256,256,256,0,22050};
    struct soundhdr hdr;

    fillheader(&hdr,22050);

    if (argc < 3)   {
        printf("useage: peaksfft NUMPEAKS filein.fft [OPTIONS]\n");
        printf("        OPTIONS: [-s synthfile.wav] [-q(uiet mode)]\n");
		printf("                 [-g gainMult] [-p pitchMult] [-t timeMult]\n");
                printf("                 [-k# killList or -f# filtList]\n");
                printf("   killList is list of frequencies to disallow\n");
                printf("   filtList is list of frequencies to only allow\n");
		exit(0);
    }
    else {
        input = fopen(argv[2], "rb");
        numPeaks = atoi(argv[1]);
        peaks = (long *) calloc(4,numPeaks+1);
        magns = (float *) calloc(4,numPeaks+1);
        pitch = 1.0;
        time = 1.0;
        i = 3;
        while (i < argc)        {
            if (argv[i][1] == 's')  {
                output = opensoundout(argv[i+1],&hdr);
		        synth = 1;
            }
            if (argv[i][1] == 'p')  {
                pitch = (float) atof(argv[i+1]);
            }
            if (argv[i][1] == 't')      {
                time = (float) atof(argv[i+1]);
            }
            if (argv[i][1] == 'q') {
                verbose = 0;
            }
            if (argv[i][1] == 'g') {
                gain = atof(argv[i+1]);
            }
            if (argv[i][1] == 'k')      {
                filtering = -1;
                numFreqs = atoi(&argv[i][2]);
                freqList = (float *) malloc(4*numFreqs);
                for (j=0;j<numFreqs;j++)    {
                    freqList[j] = atof(argv[i+1]);
                    i += 1;
                }
            }
            if (argv[i][1] == 'f')      {
                filtering = 1;
                numFreqs = atoi(&argv[i][2]);
                freqList = (float *) malloc(4*numFreqs);
                for (j=0;j<numFreqs;j++)    {
                    freqList[j] = atof(argv[i+1]);
                    i += 1;
                }
            }
            i += 1;
        }
    }
    if (input)        {
        fread(&fhdr,4,6,input);
        length = fhdr.fftsize;
        fftData = (float *) malloc(4*length);
        winWidth = 2 * (fhdr.fftsize / fhdr.windowsize); //window main lobe width
        if (synth)      {
            hdr.dlength = (long) (time * (float) fhdr.dlength * 2.0f);
#ifdef LITTLENDIAN
            hdr.flength = hdr.dlength + 4;
            hdr.bytes_per_sec = fhdr.srate*2;
#endif
            hdr.srate = fhdr.srate;
            PI = 4.*atan(1.0);
            delta = pitch * 2 * PI / (double) fhdr.fftsize;
            outData = (float *) calloc(4,(long) (time * (float) length));
        }
        if (filtering != 0)        {
            length = fhdr.fftsize;
            freqMask = (float *) malloc(length/2 * 4);
			if (filtering == -1)	{
				for (i=0;i<length/2;i++) freqMask[i] = 1.0;
                for (j=0;j<numFreqs;j++)	{
					itemp = freqList[j]*length/fhdr.srate;
					for (i=itemp-winWidth;i<itemp+winWidth;i++)
						freqMask[i] = 0.0;
				}
			}
			else if (filtering == 1)	{
				for (i=0;i<length/2;i++) freqMask[i] = 0.0;
                for (j=0;j<numFreqs;j++)	{
					itemp = freqList[j]*length/fhdr.srate;
					i = itemp;
					//  for (i=itemp-winWidth;i<itemp+winWidth;i++)
						freqMask[i] = 1.0;
				}
			}
		}
        while (fread(fftData,fhdr.fftsize,4,input))       {
            length = fhdr.fftsize;
            for (i=0;i<length/2;i++)   {
                temp = fftData[i*2];
                temp *= temp;
                temp2 = fftData[i*2+1];
                temp2 *= temp2;
                fftData[i] = (float) sqrt(temp + temp2);  // Throw away phase
            }
            length = length / 2;                  // and only look at magnitude
            if (filtering != 0)	{
			    for (i=0;i<length;i++)
					fftData[i] *= freqMask[i];

			}
			for (i=0;i<winWidth;i++)     {
                fftData[i] = 0;                   // null out low frequency terms
            }
            peak = 0;
            while (peak < numPeaks)        {
                max = 0.0;
                maxloc = 0;
                for (i=0;i<length;i++) {
                    if (fabs(fftData[i] > max))   {
                        max = (float) fabs(fftData[i]);
                        maxloc = i;
                    }
                }
                peaks[peak] = maxloc;
                magns[peak] = max;
                if (maxloc >= winWidth)  {
                    for (i=maxloc-winWidth;i<maxloc+winWidth;i++)
                        fftData[i] = 0;
                }
                peak += 1;
            }
            if (numPeaks > 1)   {
                notDone = 1;
                while (notDone)     {
                    notDone = 0;
                    for (j=0;j<numPeaks-1;j++)  {
                        if (peaks[j] > peaks[j+1])  {
                            if (peaks[j+1] > 0)     {
                                max = magns[j];
								maxloc = peaks[j];
								peaks[j] = peaks[j+1];
								magns[j] = magns[j+1];
								peaks[j+1] = maxloc;
								magns[j+1] = max;
								notDone = 1;
							}
						}
					}
                }
            }
			if (verbose)	{ 
				for (i=0;i<numPeaks;i++)        {
                                        printf("%li,%f ;",fhdr.srate * peaks[i] / fhdr.fftsize,magns[i]);
				}				
				printf("\n\n");
			}
			else printf(".");

            if (synth)  {
                newHop = (long) (time * (float) fhdr.hopsize);
                for (i=0;i<newHop;i++)       {
                    outData[i] = outData[i+newHop];
                }
                for (i=newHop;i<newHop*2;i++) {
                    outData[i] = 0;
                }
                for (i=0;i<newHop*2;i++) {
                    temp = 0.0;
                    for (peak = 0;peak<numPeaks;peak++)     {
                        temp += (float) (magns[peak] * cos(peaks[peak]*phase));
                    }
                    temp *= (1.0 - cos(PI * i / newHop));
                    outData[i] += temp;
                    data = (short) (gain * outData[i]);
                    if (i < newHop)	{
                        fwrite(&data,2,1,output);
                        total += 1;
                    }
                    phase += delta;
                    //  temp /= fhdr.fftsize;
                }
                phase -= (delta * newHop);
            }
                
        }
		printf("\n");
        fclose(input);
        free(fftData);
        if (synth)	{
			free(outData);
                        closesoundout(output,total);
		}
        free(peaks);
        free(magns);
        if (filtering != 0)	{
			free(freqList);
			free(freqMask);
		}
    }
    else    {
		printf("Can't open input (or output) file!!\n");
    }
    
}


