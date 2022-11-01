#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/*  	***********************  Convolution by Transform **********************************

	This is a command line c function call to implement the convolution of two sounds.
	The convolution is performed by using frequency transforms.  This can be found in
	books about transforms.  Electrical engineering books on linear system theory
	and digital signal processing also cover this.  Two fine books are:
	
		R. N. Bracewell, "The Fourier Transform and its Applications"
		Rabiner and Gold, "Theory and Application of Digital Signal Processing"
	
	You use it like this:		convolve filein1 filein2 fileout

	Fileout must be different from either of the input files.  Right now, convolution of
	two mono files yields a mono file, 16 bits only, and sample rates should be the same.
*/
	        
#include "../waveio.h"

#define PI        3.14159265358979323846
#define SQRT_TWO     1.41421356237309504880
#define TWO_PI 2*PI

int fhtConvolve(int powerOfFour, long x_length,long h_length,
    			short *x_array, short *h_array, double *y_array);

int main(int argc,char *argv[])     /* convolve two files */
{
    FILE *fileIn1, *fileIn2, *tempfile, *fileOut;
    short data;
    short *x_array,*h_array;
    double *y_array;
    long i,x_length,h_length,y_length,r_length;
	long fht_length,pof,num_frames;
    struct soundhdr hdrIn1;
    struct soundhdr hdrIn2;
    struct soundhdr tempHdr;
    struct soundhdr hdrOut;
    
    if (argc!=4) {
        fprintf(stderr,"usage: convfht filein1.wav(snd) filein2.wav(snd) fileout.wav(snd)\n");
        exit(0);
    }
    printf("Convolving %s and %s to output file %s\n",argv[1],argv[2],argv[3]);


    fileIn1 = opensoundin(argv[1],&hdrIn1);
    fileIn2 = opensoundin(argv[2],&hdrIn2);
    fileOut = opensoundout(argv[3],&hdrIn1);
    if (fileIn1 && fileIn2 && fileOut)      {         

       if (hdrIn1.dlength < hdrIn2.dlength)    {     //  make longer sound be x, shorter sound is h
            tempfile = fileIn2;
            fileIn2 = fileIn1;
            fileIn1 = tempfile;
            tempHdr = hdrIn2;
            hdrIn2 = hdrIn1;
            hdrIn1 = tempHdr;
        }
        x_length = hdrIn1.dlength/2;
        h_length = hdrIn2.dlength/2;
        r_length = x_length + h_length;
        fht_length = 1;
    	pof = 0;
        while (fht_length<(2*h_length)) {
		fht_length *= 4;
			pof += 1;
        }
        num_frames = x_length / (fht_length - h_length) + 1;
        x_length = (fht_length - h_length) * num_frames;
        printf("Transform Lengths are: %i\n",fht_length);
        x_array = (short *) calloc(2,(x_length+h_length));
        h_array = (short *) calloc(2,h_length);
        fread(x_array,2,x_length,fileIn1);
        fread(h_array,2,h_length,fileIn2);
        
        y_length = x_length + h_length;
        y_array = (double *) calloc(y_length,8);

        fhtConvolve(pof, x_length,h_length, x_array, h_array, y_array);
        for (i=0;i<r_length;i++)  {
            data = y_array[i];
            fwrite(&data,2,1,fileOut);
        }
 	    free(x_array);
            free(h_array);
	    free(y_array);
    }
    else {
       printf("File Problems\n");
    }
    closesoundin(fileIn1);
    closesoundin(fileIn2);
    closesoundout(fileOut,r_length);
    return 1;
}

int directConvolve(long x_length, long h_length, short *x_array, short *h_array, float *y_array)
{
   long i,j;
   float max = 0.0;

   printf("Total samples to compute: %i\n",x_length+h_length);

   for (i=0;i<x_length+h_length;i++) {
        if (i % 2000 == 0) printf("Sample: %i\n",i);
        y_array[i] = 0.0;                                         
        for (j=0;(i-j) > 0 && (i-j) < x_length && j< h_length;j++) {
           y_array[i] += h_array[j] * x_array[i-j];
        }
	if (fabs(y_array[i])>max) max = fabs(y_array[i]);
   }
   max = 32000 / max;
   for (i=0;i<x_length;i++) y_array[i] *= max;
   return 1;
}

int make_sines(int power_of_four,double *sine,double *cosine)
{
    int i,length,length_over_four,length_over_two,three_fourths_length;
    double freq,temp;
    length = pow(4.0, (double) power_of_four);
    length_over_four = length / 4;
    length_over_two = length_over_four + length_over_four;
    three_fourths_length = length_over_two + length_over_four;
        freq = 2.0 * PI / length;
        for (i=0;i<=length_over_four;i++) {
            temp = freq * i;
	   sine[i] = sin(temp);
        }
       cosine[0] = 1.0;
       for (i=1;i<=length_over_four;i++)	{
           sine[length-i] = -sine[i];
	  sine[length_over_two-i] = sine[i];
	  sine[length_over_two+i] = -sine[length_over_two-i];
           cosine[i] = sine[length_over_four-i];
       }
       for (i=1;i<three_fourths_length;i++)	{
	  cosine[i] = sine[length_over_four+i];
       }
   return 1;
}

int fhtConvolve(int powerOfFour, long x_length, long h_length, short *x_array, short *h_array, double *y_array)
{
   extern int make_sines(int power_of_four,double *sine,double *cosine);
   extern int fhtRX4(int powerOfFour, double *array,double *sine,double *cosine);
   double *sine,*cosine;
   int i,j,n,hop;
   double norm,*temp_xarray,*temp_harray,*temp_yarray;
   n = pow(4.0,powerOfFour);
   sine = (double *) malloc(n * 8);
   cosine = (double *) malloc(n * 8);
   printf("Now allocating %i bytes for sine and cosine tables\n",16 * n);
   make_sines(powerOfFour,sine,cosine);
   hop = n - h_length;
   temp_xarray = (double *) calloc(n,8);
   temp_harray = (double *) calloc(n,8);
   temp_yarray = (double *) calloc(n,8);
   printf("Now allocating %i bytes for temporary arrays.\n", 24 * n);
   for (i=0;i<h_length;i++) temp_harray[i] = h_array[i];
   fhtRX4(powerOfFour,temp_harray,sine,cosine);
   printf("%li to process: ",x_length);
   for (j=0;j<=x_length-hop;j+=hop)	{
  		printf("%li ",j+1);
		for (i=0;i<hop;i++) temp_xarray[i] = x_array[j + i];
		while (i<n) temp_xarray[i++] = 0.0;
		fhtRX4(powerOfFour,temp_xarray,sine,cosine);
		temp_yarray[0] = 2 * temp_xarray[0] * temp_harray[0];
		for (i=1;i<n;i++) 
			temp_yarray[i] = (temp_xarray[i]*temp_harray[i]) 
							- (temp_xarray[n-i] * temp_harray[n-i])
   							+ (temp_xarray[i]*temp_harray[n-i]) 
							+ (temp_xarray[n-i] * temp_harray[i]);
        fhtRX4(powerOfFour,temp_yarray,sine,cosine);
		for (i=0;i<n;i++) y_array[j+i] += temp_yarray[i];
    }
    printf("%li \n",j);
    norm = 0.0;
    for (i=0;i<x_length+h_length;i++) if (fabs(y_array[i]) > norm) norm = fabs(y_array[i]);
    if (norm>0) norm = 32000.0 / norm;
    for (i=0;i<x_length+h_length;i++) y_array[i] *= norm;
    free(sine);
    free(cosine);
    free(temp_xarray);
    free(temp_harray);
    free(temp_yarray);
    return 1;
}

int fhtRX4(int powerOfFour, double *array,double *sine,double *cosine)
{
    /* In place FHT of floating point data in array.
    	Size of data array must be power of four.
	Lots of sets of four inline code statements,
	so it is verbose and repetative but fast.        */

    int n=0,n4=0,nr=0,j=0,i=0,k=0,L=0,d1=0,d2=0,d3=0,d4=0,d5=1;
    int L1=0,L2=0,L3=0,L4=0,L5=0,L6=0,L7=0,L8=0;
    double r=0.0;
//    double a1=0.0,a2=0.0,a3=0.0;
    int a1=0,a2=0,a3=0;
    double s1=0.0,s2=0.0,s3=0.0,c1=0.0,c2=0.0,c3=0.0;
    double t=0.0,t1=0.0,t2 =0.0,t3=0.0,t4=0.0,t5=0.0,t6=0.0,t7=0.0,t8=0.0;
    double t9=0.0,t0=0.0;
    int n_over_d3;
    n = pow(4 , powerOfFour);
    n4 = n/4;
    r = SQRT_TWO;
    j = 1;
    i = 0;
    while (i<n-1)	{
    	i++;
	if (i<j)	{
    		t = array[j-1];
		array[j-1] = array[i-1];
		array[i-1] = t;
    	}
    	k = n4;
    	while ((3*k)<j)	{
    		j -= 3*k;
		k /= 4;
    	}
    	j += k;
    }
    for (i=0;i<n;i += 4) {
    	t1 = array[i] + array[i+1];
    	t2 = array[i] - array[i+1];
    	t3 = array[i+2] + array[i+3];
    	t4 = array[i+2] - array[i+3];
	array[i] = t1 + t3;
	array[i+1] = t1 - t3;
	array[i+2] = t2 + t4;
	array[i+3] = t2 - t4;
    }
    for (L=2;L<=powerOfFour;L++)  {
	d1 = pow(2.0 , L+L-3.0);
	d2=d1+d1;
	d3=d2+d2;
	n_over_d3 = n / 2 / d3;
	d4=d2+d3;
	d5=d3+d3;
	for (j=0;j<n;j += d5)	  {
	     t1 = array[j]+array[j+d2];
	     t2 = array[j]-array[j+d2];
	     t3 = array[j+d3]+array[j+d4];
	     t4 = array[j+d3]-array[j+d4];
	     array[j] = t1 + t3;
	     array[j+d2] = t1 - t3;
	     array[j+d3] = t2 + t4;
	     array[j+d4] = t2 - t4;
	     t1 = array[j+d1];
	     t2 = array[j+d1+d2]*r;
	     t3 = array[j+d1+d3];
	     t4 = array[j+d1+d4]*r;
	     array[j+d1] = t1 + t2 + t3;
	     array[j+d1+d2] = t1 - t3 + t4;
	     array[j+d1+d3] = t1 - t2 + t3;
	     array[j+d1+d4] = t1 - t3 - t4;
	     for (k=1;k<d1;k++)	{
	     	  L1 = j + k;
		  L2 = L1 + d2;
		  L3 = L1 + d3;
		  L4 = L1 + d4;
		  L5 = j + d2 - k;
		  L6 = L5 + d2;
		  L7 = L5 + d3;
		  L8 = L5 + d4;

/*		  a1 = PI * k / ((double) d3);
		  a2 = a1 + a1;
		  a3 = a1 + a2;
		  c1 = cos(a1);
		  c2 = cos(a2);
		  c3 = cos(a3);
		  s1 = sin(a1);
		  s2 = sin(a2);
		  s3 = sin(a3);				*/

		  a1 = (int) (k * n_over_d3) % n;
		  a2 = (a1 + a1) % n;
		  a3 = (a1 + a2) % n;
		  s1 = sine[a1];
		  s2 = sine[a2];
		  s3 = sine[a3];
		  c1 = cosine[a1];
		  c2 = cosine[a2];
		  c3 = cosine[a3];

		  t5 = array[L2]*c1 + array[L6]*s1;
		  t6 = array[L3]*c2 + array[L7]*s2;
		  t7 = array[L4]*c3 + array[L8]*s3;
		  t8 = array[L6]*c1 - array[L2]*s1;
		  t9 = array[L7]*c2 - array[L3]*s2;
		  t0 = array[L8]*c3 - array[L4]*s3;
		  t1 = array[L5] - t9;
		  t2 = array[L5] + t9;
		  t3 = -t8 - t0;
		  t4 = t5 - t7;
		  array[L5] = t1 + t4;
		  array[L6] = t2 + t3;
		  array[L7] = t1 - t4;
		  array[L8] = t2 - t3;
		  t1 = array[L1] + t6;
		  t2 = array[L1] - t6;
		  t3 = t8 - t0;
		  t4 = t5 + t7;
		  array[L1] = t1 + t4;
		  array[L2] = t2 + t3;
		  array[L3] = t1 - t4;
		  array[L4] = t2 - t3;
	     }
	}
    }		  
    return 1;
}
