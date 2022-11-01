/*************************************************************/
/*****  FFT Program, by Perry Cook, uses FFT code snip       */
/*****  from the San Diego CARL package.  This program       */
/*****  converts a mono 16 bit .wav file to a float .fft     */
/*****  or back (fft to wav).  The format for the .fft file  */
/*****  is real, imag, real, imag, ...                       */
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define FORWARD 1
#define INVERSE 0

#include "../../waveio.h"

struct ffthdr {
  char fftc[4];     // .fft
  long fftsize;     // fft size
  long windowsize;  // windowsize (<= fftsize)
  long hopsize;     // hop size (<=fftsize) 
  long dlength;     // original file data length in samples
  long srate;       // original file sample rate 
};

void rfft(float *x, long N, int forward);
void cfft(float *x, long NC, int forward);
void bitreverse(float *x, long N);
int makewindow(float *win, long length);
int dowindow(float *fdata, float *window, long length);

void main(int argc, char *argv[])   {
    float *result,*tempres=0,*window=0;
    short *data;
    float max,norm=1.0;
	long i,time,fsize=1024,wsize=512,hsize=256,nread;
    int wavein;
    long length,srate;
    FILE *input, *output;
    struct soundhdr hdr;
    struct ffthdr fhdr =
        {".fft",1024,512,256,0,0};

    if (argc < 3)   {
        printf("useage: fft filein.wav<fft> fileout.fft<wav> [-flags]\n");
        printf("        flags (wav->FFT): -f fftSize (256 default)\n");
        printf("                 -w windowSize  -h hopSize (<= fftSize)\n");
        printf("        flags (FFT->wav): -s samplingRate (original default)\n");
        printf("        Converts .snd to .fft, and .fft to .snd\n");
        exit(0);
    }
#ifdef LITTLENDIAN
    if (argv[1][strlen(argv[1])-1]=='v')      {     // "wav/snd"
#else
    if (argv[1][strlen(argv[1])-1]=='d')      {     // "wav/snd"
#endif
        input = opensoundin(argv[1], &hdr);
        output = fopen(argv[2], "wb");
        wavein = 1;
        length = hdr.dlength / 2;
        fhdr.dlength = hdr.dlength/2;
        fhdr.srate = hdr.srate;
    }
    else {
        fillheader(&hdr,22050);
        input = fopen(argv[1], "rb");
        if (!input)     {
            printf("Can't open input file!!\n");
            exit(0);
        }
        wavein = 0;
        fread(&fhdr,4,6,input);
        fsize = fhdr.fftsize;
        wsize = fhdr.windowsize;
        hsize = fhdr.hopsize;
        hdr.dlength = fhdr.dlength*2;
#ifdef LITTLENDIAN
        hdr.flength = hdr.dlength+44;
        hdr.bytes_per_sec = fhdr.srate*2;
#endif
        hdr.srate = fhdr.srate;
        output = opensoundout(argv[2], &hdr);
    }

    if (input && output)	{ 
        if (wavein == 1)       {
            if (argc > 3)   {
            i = 3;
            while (i < argc)        {
                if (argv[i][1] == 'f') {
                    fsize = atoi(argv[i+1]);
                }
                if (argv[i][1] == 'w') {
                    wsize = atoi(argv[i+1]);
                }
                if (argv[i][1] == 'h') {
                    hsize = atoi(argv[i+1]);
                }
                i += 1;
            }
            if (wsize > fsize/2)      {
                printf("Setting Window Size to 1/2 FFT Size\n");
                wsize = fsize/2;
            }
            if (hsize > wsize/2)      {
                printf("Setting Hop Size to 1/2 Window Size\n");
                hsize = wsize/2;
            }
            fhdr.fftsize = fsize;
            fhdr.windowsize = wsize;
            fhdr.hopsize = hsize;
        }
        if (wavein = 1) {
            fwrite(&fhdr,4,6,output);
        }
        result = (float *) calloc(2,4 * fsize);
        data = (short *) malloc(4*length);
        window = (float *) malloc(4 * wsize);
        makewindow(window, wsize);
        fread(data, 2, length, input);
        time = 0;
        while (time < length)       {
            printf(".");
            for (i=0;i<wsize;i++)   { 
                result[i] = data[time+i];
            }
            dowindow(result,window,wsize);
            for (i=wsize;i<fsize;i++)       {
                result[i] = 0;
            }
            rfft(result,fsize/2,FORWARD);
            fwrite(result, 4, fsize, output);
                time += hsize;
            }
            for (i=0;i<fsize;i++) result[i] = 0;  // extra buffer
            fwrite(result, 4, fsize, output);     // for end conditions
        }
        else    {               //   wavein = 0, fft input
            if (argc > 3)   {
                if (argv[3][1] == 's') {
                    srate = atoi(argv[4]);
                    hdr.srate = srate;
#ifdef LITTLENDIAN
                    hdr.bytes_per_sec = srate*2;
#endif
                }
            }
//            fwrite(&hdr,44,1,output);
            length = fhdr.dlength;
            data = (short *) malloc(2);
            result = (float *) calloc(2,4 * (length+fsize));
            tempres = (float *) malloc(4*fsize);
            norm = 2.0f * (float) fhdr.hopsize / (float) fhdr.windowsize;
            time = 0;
            while (nread = fread(tempres,4,fsize,input))    {
                printf(".");
				if (nread < fsize)	{
					printf("Blocking Error here!! %i\n",nread);
 					for (i=nread;i<fsize;i++) 
						tempres[i] = 0;
                }
                rfft(tempres,fsize/2,INVERSE);
                for (i=0;i<fsize;i++)	{
				    result[time+i] += norm * tempres[i];
				}
				time += hsize;
			}
			max = 1.0;
			for (i=0;i<length;i++)      {
				if (fabs(result[i]) > 32000)	{
					printf("Clipping here, normalizing file\n");
					max = (float) fabs(result[i]);
				}
				if (max > 1.0) 
					max = 32000 / max;
			}
			for (i=0;i<length;i++)      {
				data[0] = (short) (result[i] * max);
				fwrite(data, 2, 1, output);
			}
		}
		printf("\n");
		fclose(input);
		free(data);
		free(result);
                if (tempres) free(tempres);
                if (window) free(window);
		fclose(output);
    }
    else    {
		printf("Can't open input (or output) file!!\n");
    }
    
}

/*    Make the window once, for efficiency */
int makewindow(float *win, long length)	{
	int i;
	double pi, phase = 0, delta;
    pi = 4.*atan(1.0);
	delta = 2 * pi / (double) length;

	for (i=0;i<length;i++)	{
		win[i] = (float) (0.5 * (1.0 - cos(phase)));
		phase += delta;
	}
	return 1;
}

int dowindow(float *fdata, float *window, long length)	{
	int i;
	for (i=0;i<length;i++)
		fdata[i] *= window[i];
	return 1;
}

/*
    These routines from the CARL software, spect.c
    Check out the CARL CMusic distribution for more
    source code
*/

#include <stdio.h>
#include <math.h>

typedef struct { float re ; float im ; } complex ;

#define CABS(x) hypot( (x).re, (x).im )

complex cadd(), csub(), cmult(), smult(), cdiv(), conjg(), csqrt() ;

extern complex zero ;
extern complex one ;
extern float synt ;
float PI ;
float TWOPI ;

/*
 * If forward is true, rfft replaces 2*N real data points in x with
 * N complex values representing the positive frequency half of their
 * Fourier spectrum, with x[1] replaced with the real part of the Nyquist
 * frequency value.  If forward is false, rfft expects x to contain a
 * positive frequency spectrum arranged as before, and replaces it with
 * 2*N real values.  N MUST be a power of 2.
 */
void rfft( float *x, long N, int forward )
{
 float c1, c2, h1r, h1i, h2r, h2i, wr, wi, wpr, wpi, temp, theta ;
 float xr, xi ;
 long i, i1, i2, i3, i4, N2p1 ;
 static int first = 1 ;
    if ( first ) {
        PI = (float) (4.*atan( 1. )) ;
        TWOPI = (float) (8.*atan( 1. )) ;
        first = 0 ;
    }
    theta = PI/N ;
    wr = 1. ;
    wi = 0. ;
    c1 = 0.5 ;
    if ( forward ) {
        c2 = -0.5 ;
        cfft( x, N, forward ) ;
        xr = x[0] ;
        xi = x[1] ;
    } else {
        c2 = 0.5 ;
        theta = -theta ;
        xr = x[1] ;
        xi = 0. ;
        x[1] = 0. ;
    }
    wpr = (float) (-2.*pow( sin( 0.5*theta ), 2. )) ;
    wpi = (float) sin( theta ) ;
    N2p1 = (N<<1) + 1 ;
    for ( i = 0 ; i <= N>>1 ; i++ ) {
        i1 = i<<1 ;
        i2 = i1 + 1 ;
        i3 = N2p1 - i2 ;
        i4 = i3 + 1 ;
        if ( i == 0 ) {
            h1r =  c1*(x[i1] + xr ) ;
            h1i =  c1*(x[i2] - xi ) ;
            h2r = -c2*(x[i2] + xi ) ;
            h2i =  c2*(x[i1] - xr ) ;
            x[i1] =  h1r + wr*h2r - wi*h2i ;
            x[i2] =  h1i + wr*h2i + wi*h2r ;
            xr =  h1r - wr*h2r + wi*h2i ;
            xi = -h1i + wr*h2i + wi*h2r ;
        } else {
            h1r =  c1*(x[i1] + x[i3] ) ;
            h1i =  c1*(x[i2] - x[i4] ) ;
            h2r = -c2*(x[i2] + x[i4] ) ;
            h2i =  c2*(x[i1] - x[i3] ) ;
            x[i1] =  h1r + wr*h2r - wi*h2i ;
            x[i2] =  h1i + wr*h2i + wi*h2r ;
            x[i3] =  h1r - wr*h2r + wi*h2i ;
            x[i4] = -h1i + wr*h2i + wi*h2r ;
        }
        wr = (temp = wr)*wpr - wi*wpi + wr ;
        wi = wi*wpr + temp*wpi + wi ;
    }
    if ( forward )
        x[1] = xr ;
    else
        cfft( x, N, forward ) ;
}

/*
 * cfft replaces float array x containing NC complex values
 * (2*NC float values alternating real, imagininary, etc.)
 * by its Fourier transform if forward is true, or by its
 * inverse Fourier transform if forward is false, using a
 * recursive Fast Fourier transform method due to Danielson
 * and Lanczos.  NC MUST be a power of 2.
 */
void cfft( float *x, long NC, int forward )
{
 float wr, wi, wpr, wpi, theta, scale ;
 long mmax, ND, m, i, j, delta ;
    ND = NC<<1 ;
    bitreverse( x, ND ) ;
    for ( mmax = 2 ; mmax < ND ; mmax = delta ) {
        delta = mmax<<1 ;
        theta = TWOPI/( forward? mmax : -mmax ) ;
        wpr = (float) (-2.*pow( sin( 0.5*theta ), 2. )) ;
        wpi = (float) sin( theta ) ;
        wr = 1. ;
        wi = 0. ;
        for ( m = 0 ; m < mmax ; m += 2 ) {
         register float rtemp, itemp ;
            for ( i = m ; i < ND ; i += delta ) {
                j = i + mmax ;
                rtemp = wr*x[j] - wi*x[j+1] ;
                itemp = wr*x[j+1] + wi*x[j] ;
                x[j] = x[i] - rtemp ;
                x[j+1] = x[i+1] - itemp ;
                x[i] += rtemp ;
                x[i+1] += itemp ;
            }
            wr = (rtemp = wr)*wpr - wi*wpi + wr ;
            wi = wi*wpr + rtemp*wpi + wi ;
        }
    }
/*
 * scale output
 */
    scale = forward ? 1./ND : 2. ;
    { register float *xi=x, *xe=x+ND ;
        while ( xi < xe )
            *xi++ *= scale ;
    }
}
/*
 * bitreverse places float array x containing N/2 complex values
 * into bit-reversed order
 */
void bitreverse( float *x, long N )
{
 float rtemp, itemp ;
 long i, j, m ;
    for ( i = j = 0 ; i < N ; i += 2, j += m ) {
        if ( j > i ) {
            rtemp = x[j] ; itemp = x[j+1] ; /* complex exchange */
            x[j] = x[i] ; x[j+1] = x[i+1] ;
            x[i] = rtemp ; x[i+1] = itemp ;
        }
        for ( m = N>>1 ; m >= 2 && j >= m ; m >>= 1 )
            j -= m ;
    }
}

