// https://github.com/jpcima/HybridReverb2
#pragma once

#include <fftw3.h>


typedef struct str_HConvSingle
{
	int step;			// processing step counter
	int maxstep;			// number of processing steps per audio frame
	int mixpos;			// current frame index
	int framelength;		// number of samples per audio frame
	int *steptask;			// processing tasks per step
	float *dft_time;		// DFT buffer (time domain)
	fftwf_complex *dft_freq;	// DFT buffer (frequency domain)
	float *in_freq_real;		// input buffer (frequency domain)
	float *in_freq_imag;		// input buffer (frequency domain)
	int num_filterbuf;		// number of filter segments
	float **filterbuf_freq_real;	// filter segments (frequency domain)
	float **filterbuf_freq_imag;	// filter segments (frequency domain)
	int num_mixbuf;			// number of mixing segments		
	float **mixbuf_freq_real;	// mixing segments (frequency domain)
	float **mixbuf_freq_imag;	// mixing segments (frequency domain)
	float *history_time;		// history buffer (time domain)
	fftwf_plan fft;			// FFT transformation plan
	fftwf_plan ifft;		// IFFT transformation plan
} HConvSingle;


typedef struct str_HConvDual
{
	int step;		// processing step counter
	int maxstep;		// number of processing steps per long audio frame
	int flen_long;		// number of samples per long audio frame
	int flen_short;		// number of samples per short audio frame
	float *in_long;		// input buffer (long frame)
	float *out_long;	// output buffer (long frame)
	HConvSingle *f_long;	// convolution filter (long segments)
	HConvSingle *f_short;	// convolution filter (short segments)
} HConvDual;


typedef struct str_HConvTripple
{
	int step;		// processing step counter
	int maxstep;		// number of processing steps per long audio frame
	int flen_medium;	// number of samples per long audio frame
	int flen_short;		// number of samples per short audio frame
	float *in_medium;	// input buffer (long frame)
	float *out_medium;	// output buffer (long frame)
	HConvDual *f_medium;	// convolution filter (long segments)
	HConvSingle *f_short;	// convolution filter (short segments)
} HConvTripple;


/* single filter functions */
double getProcTime(int flen, int num, double dur);
void hcPutSingle(HConvSingle *filter, float *x);
void hcProcessSingle(HConvSingle *filter);
void hcGetSingle(HConvSingle *filter, float *y);
void hcGetAddSingle(HConvSingle *filter, float *y);
void hcInitSingle(HConvSingle *filter, float *h, int hlen, int flen, int steps);
void hcCloseSingle(HConvSingle *filter);

/* dual filter functions */
void hcBenchmarkDual(int sflen, int lflen);
void hcProcessDual(HConvDual *filter, float *in, float *out);
void hcProcessAddDual(HConvDual *filter, float *in, float *out);
void hcInitDual(HConvDual *filter, float *h, int hlen, int sflen, int lflen);
void hcCloseDual(HConvDual *filter);

/* tripple filter functions */
void hcBenchmarkTripple(int sflen, int mflen, int lflen);
void hcProcessTripple(HConvTripple *filter, float *in, float *out);
void hcProcessAddTripple(HConvTripple *filter, float *in, float *out);
void hcInitTripple(HConvTripple *filter, float *h, int hlen, int sflen, int mflen, int lflen);
void hcCloseTripple(HConvTripple *filter);


#ifdef HYBRIDCONV_SSE
#include <xmmintrin.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <math.h>
#include <fftw3.h>
#include "libHybridConv.h"


double hcTime(void)
{
#ifdef _WIN32
	DWORD t;

	t = GetTickCount();
	return (double)t * 0.001;
#else
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec * 0.000001;
#endif
}

////////////////////////////////////////////////////////////////

double getProcTime(int flen, int num, double dur)
{
	HConvSingle filter;
	float *x;
	float *h;
	float *y;
	int xlen, hlen, ylen;
	int size, n;
	int pos;
	double t_start, t_diff;
	double counter = 0.0;
	double proc_time;
	double lin, mul;

	xlen = 2048*2048;
	size = sizeof(float) * xlen;
	x = (float *)fftwf_malloc(size);
	lin = pow(10.0, -100.0 / 20.0);	// 0.00001 = -100dB
	mul = pow(lin, 1.0 / (double)xlen);
	x[0] = 1.0;
	for (n = 1; n < xlen; n++)
		x[n] = -mul * x[n - 1];

	hlen = flen * num;
	size = sizeof(float) * hlen;
	h = (float *)fftwf_malloc(size);
	lin = pow(10.0, -60.0 / 20.0);	// 0.001 = -60dB
	mul = pow(lin, 1.0 / (double)hlen);
	h[0] = 1.0;
	for (n = 1; n < hlen; n++)
		h[n] = mul * h[n - 1];

	ylen = flen;
	size = sizeof(float) * ylen;
	y = (float *)fftwf_malloc(size);

	hcInitSingle(&filter, h, hlen, flen, 1);

	t_diff = 0.0;
	t_start = hcTime();
	pos = 0;
	while (t_diff < dur)
	{
		hcPutSingle(&filter, &x[pos]);
		hcProcessSingle(&filter);
		hcGetSingle(&filter, y);
		pos += flen;
		if (pos >= xlen)
			pos = 0;
		counter += 1.0;
		t_diff = hcTime() - t_start;
	}
	proc_time = t_diff / counter;
	printf("Processing time: %7.3f us\n", 1000000.0 * proc_time);

	hcCloseSingle(&filter);
	fftwf_free(x);
	fftwf_free(h);
	fftwf_free(y);

        return proc_time;
}



void hcPutSingle(HConvSingle *filter, float *x)
{
	int j, flen, size;

	flen = filter->framelength;
	size = sizeof(float) * flen;
	memcpy(filter->dft_time, x, size);
	memset(&(filter->dft_time[flen]), 0, size);
	fftwf_execute(filter->fft);
	for (j = 0; j < flen + 1; j++)
	{
		filter->in_freq_real[j] = filter->dft_freq[j][0];
		filter->in_freq_imag[j] = filter->dft_freq[j][1];
	}
}


void hcProcessSingle(HConvSingle *filter)
{
#if 0
	int s, n, start, stop, flen;
	float *x_real;
	float *x_imag;
	float *h_real;
	float *h_imag;
	float *y_real;
	float *y_imag;

	flen = filter->framelength;
	x_real = filter->in_freq_real;
	x_imag = filter->in_freq_imag;
	start = filter->steptask[filter->step];
	stop  = filter->steptask[filter->step + 1];
	for (s = start; s < stop; s++)
	{
		n = (s + filter->mixpos) % filter->num_mixbuf;
		y_real = filter->mixbuf_freq_real[n];
		y_imag = filter->mixbuf_freq_imag[n];
		h_real = filter->filterbuf_freq_real[s];
		h_imag = filter->filterbuf_freq_imag[s];
		for (n = 0; n < flen + 1; n++)
		{
			y_real[n] += x_real[n] * h_real[n] -
			             x_imag[n] * h_imag[n];
			y_imag[n] += x_real[n] * h_imag[n] +
			             x_imag[n] * h_real[n];
		}
	}
	filter->step = (filter->step + 1) % filter->maxstep;
#endif

	int s, n, start, stop, flen;
#ifdef HYBRIDCONV_SSE
	int flen4;
	__m128 *x4_real;
	__m128 *x4_imag;
	__m128 *h4_real;
	__m128 *h4_imag;
	__m128 *y4_real;
	__m128 *y4_imag;
#endif
	float *x_real;
	float *x_imag;
	float *h_real;
	float *h_imag;
	float *y_real;
	float *y_imag;

	flen = filter->framelength;
	x_real = filter->in_freq_real;
	x_imag = filter->in_freq_imag;
#ifdef HYBRIDCONV_SSE
	x4_real = (__m128*)x_real;
	x4_imag = (__m128*)x_imag;
#endif
	start = filter->steptask[filter->step];
	stop  = filter->steptask[filter->step + 1];
	for (s = start; s < stop; s++)
	{
		n = (s + filter->mixpos) % filter->num_mixbuf;
		y_real = filter->mixbuf_freq_real[n];
		y_imag = filter->mixbuf_freq_imag[n];
		h_real = filter->filterbuf_freq_real[s];
		h_imag = filter->filterbuf_freq_imag[s];
#ifdef HYBRIDCONV_SSE
		y4_real = (__m128*)y_real;
		y4_imag = (__m128*)y_imag;
		h4_real = (__m128*)h_real;
		h4_imag = (__m128*)h_imag;
		flen4 = flen / 4;
		for (n = 0; n < flen4; n++)
		{
			__m128 a = _mm_mul_ps(x4_real[n], h4_real[n]);
			__m128 b = _mm_mul_ps(x4_imag[n], h4_imag[n]);
			__m128 c = _mm_sub_ps(a, b);
			y4_real[n] = _mm_add_ps(y4_real[n], c);
			a = _mm_mul_ps(x4_real[n], h4_imag[n]);
			b = _mm_mul_ps(x4_imag[n], h4_real[n]);
			c = _mm_add_ps(a, b);
			y4_imag[n] = _mm_add_ps(y4_imag[n], c);
		}
#else
		for (n = 0; n < flen; n++)
		{
			y_real[n] += x_real[n] * h_real[n] -
			             x_imag[n] * h_imag[n];
			y_imag[n] += x_real[n] * h_imag[n] +
			             x_imag[n] * h_real[n];
		}
#endif
		y_real[flen] += x_real[flen] * h_real[flen] -
		                x_imag[flen] * h_imag[flen];
		y_imag[flen] += x_real[flen] * h_imag[flen] +
		                x_imag[flen] * h_real[flen];
	}
	filter->step = (filter->step + 1) % filter->maxstep;
}


void hcGetSingle(HConvSingle *filter, float *y)
{
	int flen, mpos;
	float *out;
	float *hist;
	int size, n, j;

	flen = filter->framelength;
	mpos = filter->mixpos;
	out  = filter->dft_time;
	hist = filter->history_time;
	for (j = 0; j < flen + 1; j++)
	{
		filter->dft_freq[j][0] = filter->mixbuf_freq_real[mpos][j];
		filter->dft_freq[j][1] = filter->mixbuf_freq_imag[mpos][j];
		filter->mixbuf_freq_real[mpos][j] = 0.0;
		filter->mixbuf_freq_imag[mpos][j] = 0.0;
	}
	fftwf_execute(filter->ifft);
	for (n = 0; n < flen; n++)
	{
		y[n] = out[n] + hist[n];
	}
	size = sizeof(float) * flen;
	memcpy(hist, &(out[flen]), size);
	filter->mixpos = (filter->mixpos + 1) % filter->num_mixbuf;
}


void hcGetAddSingle(HConvSingle *filter, float *y)
{
	int flen, mpos;
	float *out;
	float *hist;
	int size, n, j;

	flen = filter->framelength;
	mpos = filter->mixpos;
	out  = filter->dft_time;
	hist = filter->history_time;
	for (j = 0; j < flen + 1; j++)
	{
		filter->dft_freq[j][0] = filter->mixbuf_freq_real[mpos][j];
		filter->dft_freq[j][1] = filter->mixbuf_freq_imag[mpos][j];
		filter->mixbuf_freq_real[mpos][j] = 0.0;
		filter->mixbuf_freq_imag[mpos][j] = 0.0;
	}
	fftwf_execute(filter->ifft);
	for (n = 0; n < flen; n++)
	{
		y[n] += out[n] + hist[n];
	}
	size = sizeof(float) * flen;
	memcpy(hist, &(out[flen]), size);
	filter->mixpos = (filter->mixpos + 1) % filter->num_mixbuf;
}


void hcInitSingle(HConvSingle *filter, float *h, int hlen, int flen, int steps)
{
	int i, j, size, num, pos;
	float gain;

	// processing step counter
	filter->step = 0;

	// number of processing steps per audio frame
	filter->maxstep = steps;

	// current frame index
	filter->mixpos = 0;

	// number of samples per audio frame
	filter->framelength = flen;

	// DFT buffer (time domain)
	size = sizeof(float) * 2 * flen;
	filter->dft_time = (float *)fftwf_malloc(size);

	// DFT buffer (frequency domain)
	size = sizeof(fftwf_complex) * (flen + 1);
	filter->dft_freq = (fftwf_complex*)fftwf_malloc(size);

	// input buffer (frequency domain)
	size = sizeof(float) * (flen + 1);
	filter->in_freq_real = (float*)fftwf_malloc(size);
	filter->in_freq_imag = (float*)fftwf_malloc(size);

	// number of filter segments
	filter->num_filterbuf = (hlen + flen - 1) / flen;

	// processing tasks per step
	size = sizeof(int) * (steps + 1);
	filter->steptask = (int *)malloc(size);
	num = filter->num_filterbuf / steps;
	for (i = 0; i <= steps; i++)
		filter->steptask[i] = i * num;
	if (filter->steptask[1] == 0)
		pos = 1;
	else
		pos = 2;
	num = filter->num_filterbuf % steps;
	for (j = pos; j < pos + num; j++)
	{
		for (i = j; i <= steps; i++)
			filter->steptask[i]++;
	}

	// filter segments (frequency domain)
	size = sizeof(float*) * filter->num_filterbuf;
	filter->filterbuf_freq_real = (float**)fftwf_malloc(size);
	filter->filterbuf_freq_imag = (float**)fftwf_malloc(size);
	for (i = 0; i < filter->num_filterbuf; i++)
	{
		size = sizeof(float) * (flen + 1);
		filter->filterbuf_freq_real[i] = (float*)fftwf_malloc(size);
		filter->filterbuf_freq_imag[i] = (float*)fftwf_malloc(size);
	}

	// number of mixing segments
	filter->num_mixbuf = filter->num_filterbuf + 1;

	// mixing segments (frequency domain)
	size = sizeof(float*) * filter->num_mixbuf;
	filter->mixbuf_freq_real = (float**)fftwf_malloc(size);
	filter->mixbuf_freq_imag = (float**)fftwf_malloc(size);
	for (i = 0; i < filter->num_mixbuf; i++)
	{
		size = sizeof(float) * (flen + 1);
		filter->mixbuf_freq_real[i] = (float*)fftwf_malloc(size);
		filter->mixbuf_freq_imag[i] = (float*)fftwf_malloc(size);
		memset(filter->mixbuf_freq_real[i], 0, size);
		memset(filter->mixbuf_freq_imag[i], 0, size);
	}

	// history buffer (time domain)
	size = sizeof(float) * flen;
	filter->history_time = (float *)fftwf_malloc(size);
	memset(filter->history_time, 0, size);

	// FFT transformation plan
	filter->fft = fftwf_plan_dft_r2c_1d(2 * flen, filter->dft_time, filter->dft_freq, FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);

	// IFFT transformation plan
	filter->ifft = fftwf_plan_dft_c2r_1d(2 * flen, filter->dft_freq, filter->dft_time, FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);

	// generate filter segments
	gain = 0.5f / flen;
	size = sizeof(float) * 2 * flen;
	memset(filter->dft_time, 0, size);
	for (i = 0; i < filter->num_filterbuf - 1; i++)
	{
		for (j = 0; j < flen; j++)
			filter->dft_time[j] = gain * h[i * flen + j];
		fftwf_execute(filter->fft);
		for (j = 0; j < flen + 1; j++)
		{
			filter->filterbuf_freq_real[i][j] = filter->dft_freq[j][0];
			filter->filterbuf_freq_imag[i][j] = filter->dft_freq[j][1];
		}
	}
	for (j = 0; j < hlen - i * flen; j++)
		filter->dft_time[j] = gain * h[i * flen + j];
	size = sizeof(float) * ((i + 1) * flen - hlen);
	memset(&(filter->dft_time[hlen - i * flen]), 0, size);
	fftwf_execute(filter->fft);
	for (j = 0; j < flen + 1; j++)
	{
		filter->filterbuf_freq_real[i][j] = filter->dft_freq[j][0];
		filter->filterbuf_freq_imag[i][j] = filter->dft_freq[j][1];
	}
}


void hcCloseSingle(HConvSingle *filter)
{
	int i;

	fftwf_destroy_plan(filter->ifft);
	fftwf_destroy_plan(filter->fft);
	fftwf_free(filter->history_time);
	for (i = 0; i < filter->num_mixbuf; i++)
	{
		fftwf_free(filter->mixbuf_freq_real[i]);
		fftwf_free(filter->mixbuf_freq_imag[i]);
	}
	fftwf_free(filter->mixbuf_freq_real);
	fftwf_free(filter->mixbuf_freq_imag);
	for (i = 0; i < filter->num_filterbuf; i++)
	{
		fftwf_free(filter->filterbuf_freq_real[i]);
		fftwf_free(filter->filterbuf_freq_imag[i]);
	}
	fftwf_free(filter->filterbuf_freq_real);
	fftwf_free(filter->filterbuf_freq_imag);
	fftwf_free(filter->in_freq_real);
	fftwf_free(filter->in_freq_imag);
	fftwf_free(filter->dft_freq);
	fftwf_free(filter->dft_time);
	free(filter->steptask);
	memset(filter, 0, sizeof(HConvSingle));
}


////////////////////////////////////////////////////////////////


void hcBenchmarkDual(int sflen, int lflen)
{
	HConvDual filter;
	float *x;
	float *h;
	float *y;
	int xlen, hlen, ylen;
	int size, n;
	int pos;
	double t_start, t_diff;
	double counter = 0.0;
	double signal_time;
	double cpu_load;
	double lin, mul;

//	xlen = sflen;
	xlen = 2048*2048;
	size = sizeof(float) * xlen;
	x = (float *)fftwf_malloc(size);
	lin = pow(10.0, -100.0 / 20.0);	// 0.00001 = -100dB
	mul = pow(lin, 1.0 / (double)xlen);
	x[0] = 1.0;
	for (n = 1; n < xlen; n++)
		x[n] = -mul * x[n - 1];

	hlen = 96000;
	size = sizeof(float) * hlen;
	h = (float *)fftwf_malloc(size);
	lin = pow(10.0, -60.0 / 20.0);	// 0.001 = -60dB
	mul = pow(lin, 1.0 / (double)hlen);
	h[0] = 1.0;
	for (n = 1; n < hlen; n++)
		h[n] = mul * h[n - 1];

	ylen = sflen;
	size = sizeof(float) * ylen;
	y = (float *)fftwf_malloc(size);

	hcInitDual(&filter, h, hlen, sflen, lflen);

	t_diff = 0.0;
	t_start = hcTime();
	pos = 0;
	while (t_diff < 10.0)
	{
		hcProcessDual(&filter, &(x[pos]), y);
		pos += sflen;
		if (pos >= xlen)
			pos = 0;
		counter += 1.0;
		t_diff = hcTime() - t_start;
	}
	signal_time = counter * sflen / 48000.0;
	cpu_load = 100.0 * t_diff / signal_time;
	printf("Estimated CPU load: %5.2f %%\n", cpu_load);

	hcCloseDual(&filter);
	fftwf_free(x);
	fftwf_free(h);
	fftwf_free(y);
}


void hcProcessDual(HConvDual *filter, float *in, float *out)
{
	int lpos, size, i;

	// convolution with short segments
	hcPutSingle(filter->f_short, in);
	hcProcessSingle(filter->f_short);
	hcGetSingle(filter->f_short, out);

	// add contribution from last long frame
	lpos = filter->step * filter->flen_short;
	for (i = 0; i < filter->flen_short; i++)
		out[i] += filter->out_long[lpos + i];

	// convolution with long segments
	if (filter->step == 0)
		hcPutSingle(filter->f_long, filter->in_long);
	hcProcessSingle(filter->f_long);
	if (filter->step == filter->maxstep - 1)
		hcGetSingle(filter->f_long, filter->out_long);

	// add current frame to long input buffer
	lpos = filter->step * filter->flen_short;
	size = sizeof(float) * filter->flen_short;
	memcpy(&(filter->in_long[lpos]), in, size);

	// increase step counter
	filter->step = (filter->step + 1) % filter->maxstep;
}


void hcProcessAddDual(HConvDual *filter, float *in, float *out)
{
	int lpos, size, i;

	// convolution with short segments
	hcPutSingle(filter->f_short, in);
	hcProcessSingle(filter->f_short);
	hcGetAddSingle(filter->f_short, out);

	// add contribution from last long frame
	lpos = filter->step * filter->flen_short;
	for (i = 0; i < filter->flen_short; i++)
		out[i] += filter->out_long[lpos + i];

	// convolution with long segments
	if (filter->step == 0)
		hcPutSingle(filter->f_long, filter->in_long);
	hcProcessSingle(filter->f_long);
	if (filter->step == filter->maxstep - 1)
		hcGetSingle(filter->f_long, filter->out_long);

	// add current frame to long input buffer
	lpos = filter->step * filter->flen_short;
	size = sizeof(float) * filter->flen_short;
	memcpy(&(filter->in_long[lpos]), in, size);

	// increase step counter
	filter->step = (filter->step + 1) % filter->maxstep;
}


void hcInitDual(HConvDual *filter, float *h, int hlen, int sflen, int lflen)
{
	int size;
	float *h2 = NULL;
	int h2len;

	// sanity check: minimum impulse response length
	h2len = 2 * lflen + 1;
	if (hlen < h2len)
	{
		size = sizeof(float) * h2len;
		h2 = (float*)fftwf_malloc(size);
		memset(h2, 0, size);
		size = sizeof(float) * hlen;
		memcpy(h2, h, size);
		h = h2;
		hlen = h2len;
	}

	// processing step counter
	filter->step = 0;

	// number of processing steps per long audio frame
	filter->maxstep = lflen / sflen;

	// number of samples per long audio frame
	filter->flen_long = lflen;

	// number of samples per short audio frame
	filter->flen_short = sflen;

	// input buffer (long frame)
	size = sizeof(float) * lflen;
	filter->in_long = (float *)fftwf_malloc(size);
	memset(filter->in_long, 0, size);

	// output buffer (long frame)
	size = sizeof(float) * lflen;
	filter->out_long = (float *)fftwf_malloc(size);
	memset(filter->out_long, 0, size);

	// convolution filter (short segments)
	size = sizeof(HConvSingle);
	filter->f_short = (HConvSingle *)malloc(size);
	hcInitSingle(filter->f_short, h, 2 * lflen, sflen, 1);

	// convolution filter (long segments)
	size = sizeof(HConvSingle);
	filter->f_long = (HConvSingle *)malloc(size);
	hcInitSingle(filter->f_long, &(h[2 * lflen]), hlen - 2 * lflen, lflen, lflen / sflen);

	if (h2 != NULL)
	{
		fftwf_free(h2);
	}
}


void hcCloseDual(HConvDual *filter)
{
	hcCloseSingle(filter->f_short);
	free(filter->f_short);
	hcCloseSingle(filter->f_long);
	free(filter->f_long);
	fftwf_free(filter->out_long);
	fftwf_free(filter->in_long);
	memset(filter, 0, sizeof(HConvDual));
}


////////////////////////////////////////////////////////////////


void hcBenchmarkTripple(int sflen, int mflen, int lflen)
{
	HConvTripple filter;
	float *x;
	float *h;
	float *y;
	int xlen, hlen, ylen;
	int size, n;
	int pos;
	double t_start, t_diff;
	double counter = 0.0;
	double signal_time;
	double cpu_load;
	double lin, mul;

//	xlen = sflen;
	xlen = 2048*2048;
	size = sizeof(float) * xlen;
	x = (float *)fftwf_malloc(size);
	lin = pow(10.0, -100.0 / 20.0);	// 0.00001 = -100dB
	mul = pow(lin, 1.0 / (double)xlen);
	x[0] = 1.0;
	for (n = 1; n < xlen; n++)
		x[n] = -mul * x[n - 1];

	hlen = 96000;
	size = sizeof(float) * hlen;
	h = (float *)fftwf_malloc(size);
	lin = pow(10.0, -60.0 / 20.0);	// 0.001 = -60dB
	mul = pow(lin, 1.0 / (double)hlen);
	h[0] = 1.0;
	for (n = 1; n < hlen; n++)
		h[n] = mul * h[n - 1];

	ylen = sflen;
	size = sizeof(float) * ylen;
	y = (float *)fftwf_malloc(size);

	hcInitTripple(&filter, h, hlen, sflen, mflen, lflen);

	t_diff = 0.0;
	t_start = hcTime();
	size = mflen / sflen;
	pos = 0;
	while (t_diff < 10.0)
	{
		for (n = 0; n < size; n++)
		{
//			hcProcessTripple(&filter, x, y);
			hcProcessTripple(&filter, &(x[pos]), y);
			pos += sflen;
			if (pos >= xlen)
				pos = 0;
		}
		counter += 1.0;
		t_diff = hcTime() - t_start;
	}
	signal_time = counter * (double)mflen / 48000.0;
	cpu_load = 100.0 * t_diff / signal_time;
	printf("Estimated CPU load: %5.2f %%\n", cpu_load);

	hcCloseTripple(&filter);
	fftwf_free(x);
	fftwf_free(h);
	fftwf_free(y);
}


void hcProcessTripple(HConvTripple *filter, float *in, float *out)
{
	int lpos, size, i;

	// convolution with short segments
	hcPutSingle(filter->f_short, in);
	hcProcessSingle(filter->f_short);
	hcGetSingle(filter->f_short, out);

	// add contribution from last medium frame
	lpos = filter->step * filter->flen_short;
	for (i = 0; i < filter->flen_short; i++)
		out[i] += filter->out_medium[lpos + i];

	// add current frame to medium input buffer
	lpos = filter->step * filter->flen_short;
	size = sizeof(float) * filter->flen_short;
	memcpy(&(filter->in_medium[lpos]), in, size);

	// convolution with medium segments
	if (filter->step == filter->maxstep - 1)
		hcProcessDual(filter->f_medium,
		              filter->in_medium,
		              filter->out_medium);

	// increase step counter
	filter->step = (filter->step + 1) % filter->maxstep;
}


void hcProcessAddTripple(HConvTripple *filter, float *in, float *out)
{
	int lpos, size, i;

	// convolution with short segments
	hcPutSingle(filter->f_short, in);
	hcProcessSingle(filter->f_short);
	hcGetAddSingle(filter->f_short, out);

	// add contribution from last medium frame
	lpos = filter->step * filter->flen_short;
	for (i = 0; i < filter->flen_short; i++)
		out[i] += filter->out_medium[lpos + i];

	// add current frame to medium input buffer
	lpos = filter->step * filter->flen_short;
	size = sizeof(float) * filter->flen_short;
	memcpy(&(filter->in_medium[lpos]), in, size);

	// convolution with medium segments
	if (filter->step == filter->maxstep - 1)
		hcProcessDual(filter->f_medium,
		              filter->in_medium,
		              filter->out_medium);

	// increase step counter
	filter->step = (filter->step + 1) % filter->maxstep;
}


void hcInitTripple(HConvTripple *filter, float *h, int hlen, int sflen, int mflen, int lflen)
{
	int size;
	float *h2 = NULL;
	int h2len;

	// sanity check: minimum impulse response length
	h2len = mflen + 2 * lflen + 1;
	if (hlen < h2len)
	{
		size = sizeof(float) * h2len;
		h2 = (float*)fftwf_malloc(size);
		memset(h2, 0, size);
		size = sizeof(float) * hlen;
		memcpy(h2, h, size);
		h = h2;
		hlen = h2len;
	}

	// processing step counter
	filter->step = 0;

	// number of processing steps per medium audio frame
	filter->maxstep = mflen / sflen;

	// number of samples per medium audio frame
	filter->flen_medium = mflen;

	// number of samples per short audio frame
	filter->flen_short = sflen;

	// input buffer (medium frame)
	size = sizeof(float) * mflen;
	filter->in_medium = (float *)fftwf_malloc(size);
	memset(filter->in_medium, 0, size);

	// output buffer (medium frame)
	size = sizeof(float) * mflen;
	filter->out_medium = (float *)fftwf_malloc(size);
	memset(filter->out_medium, 0, size);

	// convolution filter (short segments)
	size = sizeof(HConvSingle);
	filter->f_short = (HConvSingle *)malloc(size);
	hcInitSingle(filter->f_short, h, mflen, sflen, 1);

	// convolution filter (medium segments)
	size = sizeof(HConvDual);
	filter->f_medium = (HConvDual *)malloc(size);
	hcInitDual(filter->f_medium, &(h[mflen]), hlen - mflen, mflen, lflen);

	if (h2 != NULL)
	{
		fftwf_free(h2);
	}
}


void hcCloseTripple(HConvTripple *filter)
{
	hcCloseSingle(filter->f_short);
	free(filter->f_short);
	hcCloseDual(filter->f_medium);
	free(filter->f_medium);
	fftwf_free(filter->out_medium);
	fftwf_free(filter->in_medium);
	memset(filter, 0, sizeof(HConvTripple));
}


class HybridConvolver
{
public:
    HybridConvolver();
    virtual ~HybridConvolver() {}

    virtual void process(const float** inputs,
                         float** outputs,
                         int numOutput,
                         int numSamples) = 0;
    void process(AudioSampleBuffer& buffer);

protected:
    std::unique_ptr<float[]> m_inbuf;
    std::unique_ptr<float[]> m_outbuf;
    int m_bufpos = 0;
    int m_lenShort = 256;
    int m_lenMedium = 1024;
    int m_lenLong = 4096;
    int m_numFilter = 0;
    std::unique_ptr<int[]> m_routeIn;
    std::unique_ptr<int[]> m_routeOut;

};

HybridConvolver::HybridConvolver()
{
}

void HybridConvolver::process(AudioSampleBuffer& buffer)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    const float **inputs = buffer.getArrayOfReadPointers();
    float **outputs = buffer.getArrayOfWritePointers();

    process(inputs, outputs, numChannels, numSamples);
}

class HybridConvolverDual : public HybridConvolver
{
public:
    HybridConvolverDual(int lenShort,
                        int lenMedium,
                        SampleData *impulses);
    ~HybridConvolverDual(void);

    void process(const float** inputs,
                 float** outputs,
                 int numOutput,
                 int numSamples) override;

private:
    std::unique_ptr<HConvDual[]> m_filter;

};

#ifndef NO_OMP
#include <omp.h>
#endif

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif


HybridConvolverDual::HybridConvolverDual(int lenShort,
                                         int lenMedium,
                                         SampleData *impulses)
{
    m_bufpos    = 0;
    m_lenShort  = lenShort;
    m_lenMedium = lenMedium;
    m_numFilter = 0;

    for (int i = 0; i < 4; i++)
        if (impulses->hasData(i) == true)
            m_numFilter++;

    m_inbuf.reset(new float[m_numFilter * lenShort]());
    m_outbuf.reset(new float[m_numFilter * lenShort]());

    m_filter.reset(new HConvDual[m_numFilter]);

    m_routeIn.reset(new int[m_numFilter]);
    m_routeOut.reset(new int[m_numFilter]);

    int hlen = impulses->getDataLen();
    int k = 0;
    for (int i = 0; i < 4; i++)
    {
        if (impulses->hasData(i) == true)
        {
            m_routeIn[k]  = (i / 2) & 1;
            m_routeOut[k] = i & 1;
            hcInitDual(&(m_filter[k]), impulses->getData(i), hlen, m_lenShort, m_lenMedium);
            m_filter[k].step = k % m_filter[k].maxstep;
            k++;
        }
    }
}

HybridConvolverDual::~HybridConvolverDual(void)
{
    for (int i = 0; i < m_numFilter; i++)
    {
        hcCloseDual(&(m_filter[i]));
    }
}


void HybridConvolverDual::process(const float** inputs, float** outputs, int numOutput, int numSamples)
{
    int pos, new_pos;

    // allocate buffer for SMP processing on the stack
    float **outputSMP = (float**)alloca(m_numFilter * sizeof(float*));
    for (int f = 0; f < m_numFilter; f++)
    {
        outputSMP[f] = (float*)alloca(numSamples * sizeof(float));;
    }

#ifndef NO_OMP
    omp_set_num_threads(m_numFilter);
#pragma omp parallel for private(pos)
#endif
    for (int f = 0; f < m_numFilter; f++)
    {
        pos = m_bufpos;
        const float *in  = inputs[m_routeIn[f]];
        float *out = outputSMP[f];
        float *buf_in  = &(m_inbuf[f * m_lenShort]);
        float *buf_out = &(m_outbuf[f * m_lenShort]);
        for (int s = 0; s < numSamples; s++)
        {
            buf_in[pos] = in[s];
            out[s] = buf_out[pos];
            pos++;
            if (pos == m_lenShort)
            {
                hcProcessDual(&(m_filter[f]), buf_in, buf_out);
                pos = 0;
            }
        }
        new_pos = pos;
    }

    m_bufpos = new_pos;

    // clear output buffers
    for (int o = 0; o < numOutput; o++)
        memset(outputs[o], 0, numSamples * sizeof(float));

    for (int f = 0; f < m_numFilter; f++)
        for (int s = 0; s < numSamples; s++)
            outputs[m_routeOut[f]][s] += outputSMP[f][s];
}

class HybridConvolverSingle : public HybridConvolver
{
public:
    HybridConvolverSingle(int lenShort,
                          SampleData *impulses);
    ~HybridConvolverSingle(void);

    void process(const float** inputs,
                 float** outputs,
                 int numOutput,
                 int numSamples) override;

private:
    std::unique_ptr<HConvSingle[]> m_filter;

};

HybridConvolverSingle::HybridConvolverSingle(int lenShort,
                                             SampleData *impulses)
{
    m_bufpos    = 0;
    m_lenShort  = lenShort;
    m_numFilter = 0;

    for (int i = 0; i < 4; i++)
        if (impulses->hasData(i) == true)
            m_numFilter++;

    m_inbuf.reset(new float[m_numFilter * lenShort]());
    m_outbuf.reset(new float[m_numFilter * lenShort]());

    m_filter.reset(new HConvSingle[m_numFilter]);

    m_routeIn.reset(new int[m_numFilter]);
    m_routeOut.reset(new int[m_numFilter]);

    int hlen = impulses->getDataLen();
    int k = 0;
    for (int i = 0; i < 4; i++)
    {
        if (impulses->hasData(i) == true)
        {
            m_routeIn[k]  = (i / 2) & 1;
            m_routeOut[k] = i & 1;
            hcInitSingle(&(m_filter[k]), impulses->getData(i), hlen, m_lenShort, 1);
            k++;
        }
    }
}

HybridConvolverSingle::~HybridConvolverSingle(void)
{
    for (int i = 0; i < m_numFilter; i++)
    {
        hcCloseSingle(&(m_filter[i]));
    }
}


void HybridConvolverSingle::process(const float** inputs, float** outputs, int numOutput, int numSamples)
{
    int pos, new_pos;

    // allocate buffer for SMP processing on the stack
    float **outputSMP = (float**)alloca(m_numFilter * sizeof(float*));
    for (int f = 0; f < m_numFilter; f++)
    {
        outputSMP[f] = (float*)alloca(numSamples * sizeof(float));;
    }

#ifndef NO_OMP
    omp_set_num_threads(m_numFilter);
#pragma omp parallel for private(pos)
#endif
    for (int f = 0; f < m_numFilter; f++)
    {
        pos = m_bufpos;
        const float *in  = inputs[m_routeIn[f]];
        float *out = outputSMP[f];
        float *buf_in  = &(m_inbuf[f * m_lenShort]);
        float *buf_out = &(m_outbuf[f * m_lenShort]);
        for (int s = 0; s < numSamples; s++)
        {
            buf_in[pos] = in[s];
            out[s] = buf_out[pos];
            pos++;
            if (pos == m_lenShort)
            {
                hcPutSingle(&(m_filter[f]), buf_in);
                hcProcessSingle(&(m_filter[f]));
                hcGetSingle(&(m_filter[f]), buf_out);
                pos = 0;
            }
        }
        new_pos = pos;
    }

    m_bufpos = new_pos;

    // clear output buffers
    for (int o = 0; o < numOutput; o++)
        memset(outputs[o], 0, numSamples * sizeof(float));

    for (int f = 0; f < m_numFilter; f++)
        for (int s = 0; s < numSamples; s++)
            outputs[m_routeOut[f]][s] += outputSMP[f][s];
}

class HybridConvolverTripple : public HybridConvolver
{
public:
    HybridConvolverTripple(int lenShort,
                           int lenMedium,
                           int lenLong,
                           SampleData *impulses);
    ~HybridConvolverTripple(void);

    void process(const float** inputs,
                 float** outputs,
                 int numOutput,
                 int numSamples) override;

private:
    std::unique_ptr<HConvTripple[]> m_filter;

};

HybridConvolverTripple::HybridConvolverTripple(int lenShort,
                                               int lenMedium,
                                               int lenLong,
                                               SampleData *impulses)
{
    m_bufpos    = 0;
    m_lenShort  = lenShort;
    m_lenMedium = lenMedium;
    m_lenLong   = lenLong;
    m_numFilter = 0;

    for (int i = 0; i < 4; i++)
        if (impulses->hasData(i) == true)
            m_numFilter++;

    m_inbuf.reset(new float[m_numFilter * lenShort]());
    m_outbuf.reset(new float[m_numFilter * lenShort]());

    m_filter.reset(new HConvTripple[m_numFilter]);

    m_routeIn.reset(new int[m_numFilter]);
    m_routeOut.reset(new int[m_numFilter]);

    int hlen = impulses->getDataLen();
    int k = 0;
    for (int i = 0; i < 4; i++)
    {
        if (impulses->hasData(i) == true)
        {
            m_routeIn[k]  = (i / 2) & 1;
            m_routeOut[k] = i & 1;
            hcInitTripple(&(m_filter[k]), impulses->getData(i), hlen, m_lenShort, m_lenMedium, m_lenLong);
            m_filter[k].step = k % m_filter[k].maxstep;
            k++;
        }
    }
}


HybridConvolverTripple::~HybridConvolverTripple(void)
{
    for (int i = 0; i < m_numFilter; i++)
    {
        hcCloseTripple(&(m_filter[i]));
    }
}


void HybridConvolverTripple::process(const float** inputs, float** outputs, int numOutput, int numSamples)
{
    int pos, new_pos;

    // allocate buffer for SMP processing on the stack
    float **outputSMP = (float**)alloca(m_numFilter * sizeof(float*));
    for (int f = 0; f < m_numFilter; f++)
    {
        outputSMP[f] = (float*)alloca(numSamples * sizeof(float));;
    }

#ifndef NO_OMP
    omp_set_num_threads(m_numFilter);
#pragma omp parallel for private(pos)
#endif
    for (int f = 0; f < m_numFilter; f++)
    {
        pos = m_bufpos;
        const float *in  = inputs[m_routeIn[f]];
        float *out = outputSMP[f];
        float *buf_in  = &(m_inbuf[f * m_lenShort]);
        float *buf_out = &(m_outbuf[f * m_lenShort]);
        for (int s = 0; s < numSamples; s++)
        {
            buf_in[pos] = in[s];
            out[s] = buf_out[pos];
            pos++;
            if (pos == m_lenShort)
            {
                hcProcessTripple(&(m_filter[f]), buf_in, buf_out);
                pos = 0;
            }
        }
        new_pos = pos;
    }

    m_bufpos = new_pos;

    // clear output buffers
    for (int o = 0; o < numOutput; o++)
        memset(outputs[o], 0, numSamples * sizeof(float));

    for (int f = 0; f < m_numFilter; f++)
        for (int s = 0; s < numSamples; s++)
            outputs[m_routeOut[f]][s] += outputSMP[f][s];
}
