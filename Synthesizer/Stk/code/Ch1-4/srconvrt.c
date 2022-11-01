#include <stdio.h>
#include <math.h>
#include <string.h>
     #include <time.h>
     #include <sys/types.h>
     #include <sys/timeb.h>

/*      This is a command line c function call to implement
        time-varying sampling rate conversion on a raw soundfile.
        Originally written by Perry R. Cook, Stanford CCRMA 1991-2.
        Feel free to use this code as tutorial or application.
        Credit me if you don't change enough to make it your own.

        Updated 2001 for Princeton COS 325 Class.
        Updated 2002 for AK Peters Synthesis Book.

	The conversion is performed by sinc interpolation
	(ideal low-pass filter response).  One method uses
	sin() function calls from the floating point library,
	and the other method uses linearly interpolated table
        lookup on a sinc function as described in:
		J. O. Smith and P. Gossett,
		"A Flexible Sampling-Rate Conversion Method," 
		Proc. of the IEEE Conference on Acoustics,
		Speech, and Signal Processing, 
		San Diego, CA, March, 1984.
	The basic parameters select how wide the sinc function
	for interpolation is (how many neighboring samples are
	used), whether to use a table, and how finely the sinc
	is sampled in forming the table.  Usage is:
	
                srconvert factor [-f finalfactor] [-d or -l] [-s rate] filein fileout

	The factor which specifies the conversion is a floating
	point number expressing the ratio of the old sampling
	rate to the new one.  The sampling rate in the soundfile
	header is left alone, so this operation as implemented 
	causes pitch-shifting, and the factor is the ratio of
	the processed pitch to the original.  The factor can
	also be viewed as the new sampling period, relative
	to the original sampling period.
	
	The real 'inner loop', where the work gets done is
	enclosed in asterisk lines.  If the sound file format
	and number of channels is known, the unneeded code
	could be removed, yielding about 80 lines for the
	entire sampling rate conversion routine, the function
	which fills the sinc table, the linear interpolation
	function, and the sinc table access function.
*/
	
#define TRUE 1
#define FALSE 0

#define WIDTH 64                /* this controls the number of neighboring samples
				   which are used to interpolate the new samples.  The
				   processing time is linearly related to this width */
#define DELAY_SIZE 140

#define USE_TABLE TRUE          /* this controls whether a linearly interpolated lookup
				   table is used for sinc function calculation, or the
				   sinc is calculated by floating point trig function calls.  */
#define USE_INTERP TRUE        /*  this controls whether the table is linear
				interpolated or not.  If you're not using the
				table, this has no effect         */

#define SAMPLES_PER_ZERO_CROSSING 32    /* this defines how finely the sinc function 
					   is sampled for storage in the table  */

float sinc_table[WIDTH * SAMPLES_PER_ZERO_CROSSING] = { 0.0 };

int delay_buffer[3*WIDTH] = { 0 };

int gimme_data(long j)
{
     return delay_buffer[(int) j + WIDTH];
}

void new_data(int data)
{
    int i;
    for (i=0;i<DELAY_SIZE-5;i++)
	delay_buffer[i] = delay_buffer[i+1];
    delay_buffer[DELAY_SIZE-5] = data;
}

#include "../waveio.h"

#define DROPMODE 0
#define LINEARMODE 1
#define SINCMODE 2

void main(int argc,char *argv[])     /* sampling rate convert file by factor */
{
    extern double sinc(double x);
    extern void make_sinc();
    double one_over_factor,delta_factor,final_factor,initial_factor;
    char *file_name_in,*file_name_out;
    struct soundhdr hdr;
    float srateOut = -1;   /* default is input, unless gets set by arg */ 
    double alpha;
    int mode;
    double temp1=0.0,temp3,time_now=0.0,factor;
    short data_in=0,data_out;
    long total_written=0,j;
    long left_limit,right_limit;
    long num_samples;
    FILE *sound_in,*sound_out;
    long int_time=0,last_time=0;
    int notDone;
    
    if (argc < 4) {
        fprintf(stderr,"usage: srconvrt factor [-f finalfactor] [-d or -l] [-s rate] filein fileout\n");
        fprintf(stderr,"      -d or -l causes drop sample or linear (default is sinc)\n");
        fprintf(stderr,"      -s sets output file sample rate (default is input rate)\n");
        exit(0);
    }
    file_name_in = argv[argc-2];
    file_name_out = argv[argc-1];
    initial_factor = atof(argv[1]);
    final_factor = initial_factor;
    j = 2;
    mode = SINCMODE;
    while (j < argc-2)  {
        if (!strcmp(argv[j],"-f"))       {
            final_factor = atof(argv[j+1]);
        }
        if (!strcmp(argv[j],"-s"))       {
            srateOut = atof(argv[j+1]);
        }
        if (!strcmp(argv[j],"-d")) mode = DROPMODE;
        if (!strcmp(argv[j],"-l")) mode = LINEARMODE;
        j++;
    }

    if (initial_factor<0.01 || initial_factor>100 || final_factor<0.01 || final_factor>100 ) {
	fprintf(stderr,"One of your factors is a little extreme!!!\n");
//        exit(0);        But still go ahead and do it
    }
    printf("Converting from: %f to: %f  file: %s to file: %s\n",initial_factor,final_factor,file_name_in,file_name_out);
    sound_in = opensoundin(file_name_in,&hdr);
    if (!sound_in)     {
        printf("Input File Problems\n");
        exit(0);
    }
    num_samples = hdr.dlength / 2;
    if (srateOut > 0)   {
        hdr.srate = srateOut;
#ifdef LITTLENDIAN
        hdr.bytes_per_sec = hdr.srate * 2;
#endif
    }
    hdr.dlength = num_samples * (initial_factor + final_factor);
#ifdef LITTLENDIAN
    hdr.flength = hdr.dlength + 44;
#endif

    sound_out = opensoundout(file_name_out,&hdr);
    if (!sound_out)     {
        printf("Output File Problems\n");
        fclose(sound_in);
        exit(0);
    }

    if(USE_TABLE) make_sinc();
    factor = initial_factor;
    printf("%li Samples to process\n",num_samples);
    delta_factor = (final_factor -initial_factor) / (double) num_samples;

/*  Everything up to here was setup     */
/*  Here comes the real work.  True interpolation of sampled data
    signals involves ideal low-pass filtering in the frequency
    domain, which corresponds to convolution with a sinc function
    in the time domain (consult any text on Fourier transforms). */

/*      There are essentially two cases, one where the conversion factor
        is less than one, and the sinc table is used to yield a sound
	which is band limited to the 1/2 the new sampling rate (we don't
	want to create bandwidth where there was none).  The other case
	is where the conversion factor is greater than one and we 'warp'
	the sinc table to make the final cutoff equal to the original sampling
	rate /2.  Warping the sinc table is based on the similarity theorem
	of the time and frequency domain, stretching the time domain (sinc
	table) causes shrinking in the frequency domain (frequency response
	of ideal low-pass filter.                          */

/*     I've also added here the capability to do linear and
        drop-sample interpolation, mostly to show how bad they
        are compared to sinc interpolation              */

       notDone = 1;    
       while (notDone)      {
		temp1 = 0.0;
            if (mode == SINCMODE)       {
		left_limit = time_now - WIDTH + 1;      /* leftmost neighboring sample used for interp.*/
		right_limit = time_now + WIDTH; /* rightmost leftmost neighboring sample used for interp.*/
		if (left_limit<0) left_limit = 0;
		if (right_limit>num_samples) right_limit = num_samples;
		if (factor<1.0) {
		    for (j=left_limit;j<right_limit;j++)
			temp1 += gimme_data(j-int_time) * 
			      sinc(time_now - (double) j);
		    data_out = temp1;
                }
		else    {
		    one_over_factor = 1.0 / factor;
		    for (j=left_limit;j<right_limit;j++)
			temp1 += gimme_data(j-int_time) * one_over_factor *
			      sinc(one_over_factor * (time_now - (double) j));
		    data_out = (int) temp1;
		}
            }
            else if (mode == DROPMODE)  {
                data_out = data_in;
            }
            else if (mode == LINEARMODE)        {
                alpha = time_now - (double) int_time;
                data_out = (delay_buffer[DELAY_SIZE-5] * alpha)
                         + (delay_buffer[DELAY_SIZE-6] * (1.0 - alpha));
            }

            fwrite(&data_out,2,1,sound_out);
            total_written++;
            time_now += factor;
            last_time = int_time;
            int_time = time_now;
            while(last_time<int_time)      {
                notDone = fread(&data_in,2,1,sound_in);
                new_data(data_in);
                last_time += 1;
            }
//                if (!(int_time % 1000)) printf("Sample # %li\n",int_time);
                if (!(int_time % 1000)) printf(".",int_time);
            factor  = initial_factor + (time_now * delta_factor);
	}
    printf("%li\n",total_written);
    closesoundin(sound_in);
    closesoundout(sound_out,total_written);

    return;
}

#define PI 3.14159263

void make_sinc()
{
    int i;
    double temp,win_freq,win;
    win_freq = PI / WIDTH / SAMPLES_PER_ZERO_CROSSING;
    sinc_table[0] = 1.0;
    for (i=1;i<WIDTH * SAMPLES_PER_ZERO_CROSSING;i++)   {
	temp = (double) i * PI / SAMPLES_PER_ZERO_CROSSING;
	sinc_table[i] = sin(temp) / temp;
	win = 0.5 + 0.5 * cos(win_freq * i);
	sinc_table[i] *= win;
    }
}

double linear_interp(double first_number,double second_number,double fraction)
{
    return (first_number + ((second_number - first_number) * fraction));
}

double t_sinc(double x)
{
    int low;
    double temp,delta;
    if (fabs(x)>=WIDTH-1)
	return 0.0;
    else {
	temp = fabs(x) * (double) SAMPLES_PER_ZERO_CROSSING;
	low = temp;          /* these are interpolation steps */
	if (USE_INTERP) {
	    delta = temp - low;  /* and can be ommited if desired */
	    return linear_interp(sinc_table[low],sinc_table[low + 1],delta);
	}
	else return sinc_table[low];
    }
}

double sinc(double x)
{
    double temp;
    if(USE_TABLE) return t_sinc(x);
    else        {
	if (x==0.0) return 1.0;
	else {
	    temp = PI * x;
	    return sin(temp) / (temp);
	}
    }      
}
