#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "../waveio.h"

/*************************************************/
/********       TIME SHIFT            ************/
/********       By Perry R. Cook      ************/
/********       Princeton U.  2001    ************/
/*************************************************/

int addsafe(short sample, short *data, long position, long length);	
short getsafe(short *data, long position, long length);
double float_random(double max);

void main(int argc,char *argv[])          /* time shift a file */
{
    FILE *wavIn, *wavOut;
    struct soundhdr hdr;
    double factor,hoptime,randamt = 0.0;
    short *data_in,*data_out;
    long num_samples,out_length,i,j,hopIn,hopOut,frame,num_frames;
    long timeIn=0, timeOut=0;
    float *window;

	if (argc<4) {
        fprintf(stderr,"usage: timeshif factor [-h hoptime] [-r randamt] filein fileout\n");
		fprintf(stderr,"       hoptime in seconds (default 80ms)\n");
		fprintf(stderr,"       randamt in percent (default 0%)\n");
		exit(0);
    }
    factor = atof(argv[1]);	
        wavIn = opensoundin(argv[argc-2],&hdr);
	if (!wavIn)  {
		printf("Input File Problem!\n");
		exit(0);
	}
        hoptime = 0.08;
	i = 2;
	while (i < argc-2)	{
		if (!strcmp(argv[i],"-h"))	{
			hoptime = atof(argv[i+1]);
			i += 2;
		}
		if (!strcmp(argv[i],"-r"))	{
			randamt = atof(argv[i+1]);
			i += 2;
		}
	}
	randamt /= 100.0;

	hopOut = (long) (hoptime * hdr.srate);

	num_samples = hdr.dlength / 2;

   	hopIn = (long) ((float) hopOut / factor);
    frame = (long) (0.75 * hopOut);
	window = (float*) malloc(frame*4*3);
	for (i=0;i<frame;i++)	{
		window[i] = (float) 0.5 * (float) i / frame;
		window[(3*frame)-1-i] = window[i];
	}
	for (i=frame;i<frame*2;i++)	{
		window[i] = 0.5;
	}

    data_in = (short*) calloc(2,num_samples);
    assert(data_in);
	fread(data_in,num_samples,2,wavIn);
	fclose(wavIn);
	out_length = (long) (num_samples * factor);
    data_out = (short*) calloc(2,out_length);
	assert(data_out);

	hdr.dlength = out_length * 2;
#ifdef LITTLENDIAN
        hdr.flength = hdr.dlength + 44;
#endif

        wavOut = opensoundout(argv[argc-1],&hdr);
	if (!wavOut)  {
		fclose(wavIn);
                free(data_in);
                free(data_out);
                free(window);
                printf("Output File Problem!\n");
		exit(0);
	}

    num_frames = num_samples / hopIn;

    timeIn = 0;
	timeOut = 0;
	while (timeOut < out_length)	{
        for (j=1;j<frame*3;j++)   {
            addsafe((short) (window[j]*getsafe(data_in,j+timeIn,num_samples)),
			data_out,j+timeOut,out_length);
		}
        timeIn += hopIn * (1.0 - randamt + float_random(2*randamt));
        timeOut += hopOut;
        printf(".");
	}
    printf("\n");

    fwrite(data_out,out_length,2,wavOut);
    fclose(wavOut);
	fclose(wavIn);
	free(window);
    free(data_in);
	free(data_out);

}

/** Bounds Checking for Arrays (so we can be careless :-) **/

int addsafe(short sample, short *data, long position, long length)	
{
    if (position < length)	{
		data[position] += sample;
		return 1;
	}
	else return 0;
}

short getsafe(short *data, long position, long length)	{
    if (position < length)
		return data[position];
	else 
		return 0;
}

#define __OS_Win_

// Return random float between 0.0 and max
double float_random(double max)     {
  double temp;
  temp = (double) (rand() * max);
  temp /= 32767.0;
  return temp;
}

