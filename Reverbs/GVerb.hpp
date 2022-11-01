/*
        Copyright (C) 1999 Juhana Sadeharju
                       kouhia at nic.funet.fi
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
    */

#pragma once

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <math.h>
#include <stdint.h>
#include <string.h>

#define buffer_write(a, b) a=(b)

// 16.16 fixpoint
typedef union {
	int32_t all;
	struct {
#ifdef WORDS_BIGENDIAN
		int16_t in;
		uint16_t fr;
#else
		uint16_t fr;
		int16_t in;
#endif
	} part;
} fixp16;

// 32.32 fixpoint
typedef union {
	int64_t all;
	struct {
#ifdef WORDS_BIGENDIAN
		int32_t in;
		uint32_t fr;
#else
		uint32_t fr;
		int32_t in;
#endif
	} part;
} fixp32;

/* 32 bit "pointer cast" union */
typedef union {
        float f;
        int32_t i;
} ls_pcast32;

// Sometimes it doesn't get defined, even though it eists and C99 is declared
long int lrintf (float x);

// 1.0 / ln(2)
#define LN2R 1.442695041f

/* detet floating point denormal numbers by comparing them to the smallest
 * normal, crap, but reliable */
#define DN_CHECK(x, l) if (fabs(x) < 1e-38) printf("DN: " l"\n")

// Denormalise floats, only actually needed for PIII and recent PowerPC
//#define FLUSH_TO_ZERO(fv) (((*(unsigned int*)&(fv))&0x7f800000)==0)?0.0f:(fv)

static __inline float flush_to_zero(float f)
{
	ls_pcast32 v;

	v.f = f;

	// original: return (v.i & 0x7f800000) == 0 ? 0.0f : f;
	// version from Tim Blechmann
	return (v.i & 0x7f800000) < 0x08000000 ? 0.0f : f;
}

static __inline void round_to_zero(volatile float *f)
{
	*f += 1e-18;
	*f -= 1e-18;
}

/* A set of branchless clipping operations from Laurent de Soras */

static __inline float f_max(float x, float a)
{
	x -= a;
	x += fabs(x);
	x *= 0.5;
	x += a;

	return x;
}

static __inline float f_min(float x, float b)
{
	x = b - x;
	x += fabs(x);
	x *= 0.5;
	x = b - x;

	return x;
}

static __inline float f_clamp(float x, float a, float b)
{
	const float x1 = fabs(x - a);
	const float x2 = fabs(x - b);

	x = x1 + a + b;
	x -= x2;
	x *= 0.5;

	return x;
}

// Limit a value to be l<=v<=u
#define LIMIT(v,l,u) ((v)<(l)?(l):((v)>(u)?(u):(v)))

// Truncate-to-zero modulo (ANSI C doesn't specify) will only work
// if -m < v < 2m
#define MOD(v,m) (v<0?v+m:(v>=m?v-m:v))

// Truncate-to-zero modulo (ANSI C doesn't specify) will only work
// if v > -m and v < m
#define NEG_MOD(v,m) ((v)<0?((v)+(m)):(v))

// Convert a value in dB's to a coefficent
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)
#define CO_DB(v) (20.0f * log10f(v))

// Linearly interpolate [ = a * (1 - f) + b * f]
#define LIN_INTERP(f,a,b) ((a) + (f) * ((b) - (a)))

// Cubic interpolation function
static __inline float cube_interp(const float fr, const float inm1, const float
                                in, const float inp1, const float inp2)
{
	return in + 0.5f * fr * (inp1 - inm1 +
	 fr * (4.0f * inp1 + 2.0f * inm1 - 5.0f * in - inp2 +
	 fr * (3.0f * (in - inp1) - inm1 + inp2)));
}

/* fast sin^2 aproxiamtion, adapted from jan AT rpgfan's posting to the
 * music-dsp list */
static __inline float f_sin_sq(float angle)
{
	const float asqr = angle * angle;
	float result = -2.39e-08f;

	result *= asqr;
	result += 2.7526e-06f;
	result *= asqr;
	result -= 1.98409e-04f;
	result *= asqr;
	result += 8.3333315e-03f;
	result *= asqr;
	result -= 1.666666664e-01f;
	result *= asqr;
	result += 1.0f;
	result *= angle;

	return result * result;
}

#ifdef HAVE_LRINTF

#define f_round(f) lrintf(f)

#else

// Round float to int using IEEE int* hack
static __inline int f_round(float f)
{
	ls_pcast32 p;

	p.f = f;
	p.f += (3<<22);

	return p.i - 0x4b400000;
}

#endif

// Truncate float to int
static __inline int f_trunc(float f)
{
	return f_round(floorf(f));
}

/* Andrew Simper's pow(2, x) aproximation from the music-dsp list */

#if 0

/* original */
static __inline float f_pow2(float x)
{
	long *px = (long*)(&x); // store address of float as long pointer
	const float tx = (x-0.5f) + (3<<22); // temporary value for truncation
	const long  lx = *((long*)&tx) - 0x4b400000; // integer power of 2
	const float dx = x-(float)(lx); // float remainder of power of 2

	x = 1.0f + dx*(0.6960656421638072f + // cubic apporoximation of 2^x
                   dx*(0.224494337302845f +  // for x in the range [0, 1]
                   dx*(0.07944023841053369f)));
	*px += (lx<<23); // add integer power of 2 to exponent

	return x;
}

#else

/* union version */
static __inline float f_pow2(float x)
{
	ls_pcast32 *px, tx, lx;
	float dx;

	px = (ls_pcast32 *)&x; // store address of float as long pointer
	tx.f = (x-0.5f) + (3<<22); // temporary value for truncation
	lx.i = tx.i - 0x4b400000; // integer power of 2
	dx = x - (float)lx.i; // float remainder of power of 2

	x = 1.0f + dx * (0.6960656421638072f + // cubic apporoximation of 2^x
		   dx * (0.224494337302845f +  // for x in the range [0, 1]
		   dx * (0.07944023841053369f)));
	(*px).i += (lx.i << 23); // add integer power of 2 to exponent

	return (*px).f;
}

#endif

/* Fast exponentiation function, y = e^x */
#define f_exp(x) f_pow2(x * LN2R)


typedef struct {
  int size;
  int idx;
  float *buf;
} ty_fixeddelay;

typedef struct {
  int size;
  float coeff;
  int idx;
  float *buf;
} ty_diffuser;

typedef struct {
  float damping;
  float delay;
} ty_damper;

ty_diffuser *diffuser_make(int, float);
void diffuser_free(ty_diffuser *);
void diffuser_flush(ty_diffuser *);
//float diffuser_do(ty_diffuser *, float);

ty_damper *damper_make(float);
void damper_free(ty_damper *);
void damper_flush(ty_damper *);
//void damper_set(ty_damper *, float);
//float damper_do(ty_damper *, float);

ty_fixeddelay *fixeddelay_make(int);
void fixeddelay_free(ty_fixeddelay *);
void fixeddelay_flush(ty_fixeddelay *);
//float fixeddelay_read(ty_fixeddelay *, int);
//void fixeddelay_write(ty_fixeddelay *, float);

int isprime(int);
int nearest_prime(int, float);

static __inline float diffuser_do(ty_diffuser *p, float x)
{
  float y,w;

  w = x - p->buf[p->idx]*p->coeff;
  w = flush_to_zero(w);
  y = p->buf[p->idx] + w*p->coeff;
  p->buf[p->idx] = w;
  p->idx = (p->idx + 1) % p->size;
  return(y);
}

static __inline float fixeddelay_read(ty_fixeddelay *p, int n)
{
  int i;

  i = (p->idx - n + p->size) % p->size;
  return(p->buf[i]);
}

static __inline void fixeddelay_write(ty_fixeddelay *p, float x)
{
  p->buf[p->idx] = x;
  p->idx = (p->idx + 1) % p->size;
}

static __inline void damper_set(ty_damper *p, float damping)
{ 
  p->damping = damping;
} 
  
static __inline float damper_do(ty_damper *p, float x)
{ 
  float y;
    
  y = x*(1.0-p->damping) + p->delay*p->damping;
  p->delay = y;
  return(y);
}

#define FDNORDER 4

typedef struct {
  int rate;
  float inputbandwidth;
  float taillevel;
  float earlylevel;
  ty_damper *inputdamper;
  float maxroomsize;
  float roomsize;
  float revtime;
  float maxdelay;
  float largestdelay;
  ty_fixeddelay **fdndels;
  float *fdngains;
  int *fdnlens;
  ty_damper **fdndamps; 
  float fdndamping;
  ty_diffuser **ldifs;
  ty_diffuser **rdifs;
  ty_fixeddelay *tapdelay;
  int *taps;
  float *tapgains;
  float *d;
  float *u;
  float *f;
  double alpha;
} ty_gverb;


ty_gverb *gverb_new(int, float, float, float, float, float, float, float, float);
void gverb_free(ty_gverb *);
void gverb_flush(ty_gverb *);
static void gverb_do(ty_gverb *, float, float *, float *);
static void gverb_set_roomsize(ty_gverb *, float);
static void gverb_set_revtime(ty_gverb *, float);
static void gverb_set_damping(ty_gverb *, float);
static void gverb_set_inputbandwidth(ty_gverb *, float);
static void gverb_set_earlylevel(ty_gverb *, float);
static void gverb_set_taillevel(ty_gverb *, float);

/*
 * This FDN reverb can be made smoother by setting matrix elements at the
 * diagonal and near of it to zero or nearly zero. By setting diagonals to zero
 * means we remove the effect of the parallel comb structure from the
 * reverberation.  A comb generates uniform impulse stream to the reverberation
 * impulse response, and thus it is not good. By setting near diagonal elements
 * to zero means we remove delay sequences having consequtive delays of the
 * similar lenths, when the delays are in sorted in length with respect to
 * matrix element index. The matrix described here could be generated by
 * differencing Rocchesso's circulant matrix at max diffuse value and at low
 * diffuse value (approaching parallel combs).
 *
 * Example 1:
 * Set a(k,k), for all k, equal to 0.
 *
 * Example 2:
 * Set a(k,k), a(k,k-1) and a(k,k+1) equal to 0.
 *
 * Example 3: The transition to zero gains could be smooth as well.
 * a(k,k-1) and a(k,k+1) could be 0.3, and a(k,k-2) and a(k,k+2) could
 * be 0.5, say.
 */

static __inline void gverb_fdnmatrix(float *a, float *b)
{
  const float dl0 = a[0], dl1 = a[1], dl2 = a[2], dl3 = a[3];

  b[0] = 0.5f*(+dl0 + dl1 - dl2 - dl3);
  b[1] = 0.5f*(+dl0 - dl1 - dl2 + dl3);
  b[2] = 0.5f*(-dl0 + dl1 - dl2 + dl3);
  b[3] = 0.5f*(+dl0 + dl1 + dl2 + dl3);
}

static __inline void gverb_do(ty_gverb *p, float x, float *yl, float *yr)
{
  float z;
  unsigned int i;
  float lsum,rsum,sum,sign;

    if ((x != x) || fabsf(x) > 100000.0f) {
    x = 0.0f;
  }

  z = damper_do(p->inputdamper, x);

  z = diffuser_do(p->ldifs[0],z);

  for(i = 0; i < FDNORDER; i++) {
    p->u[i] = p->tapgains[i]*fixeddelay_read(p->tapdelay,p->taps[i]);
  }
  fixeddelay_write(p->tapdelay,z);

  for(i = 0; i < FDNORDER; i++) {
    p->d[i] = damper_do(p->fdndamps[i],
			p->fdngains[i]*fixeddelay_read(p->fdndels[i],
						       p->fdnlens[i]));
  }

  sum = 0.0f;
  sign = 1.0f;
  for(i = 0; i < FDNORDER; i++) {
    sum += sign*(p->taillevel*p->d[i] + p->earlylevel*p->u[i]);
    sign = -sign;
  }
  sum += x*p->earlylevel;
  lsum = sum;
  rsum = sum;

  gverb_fdnmatrix(p->d,p->f);

  for(i = 0; i < FDNORDER; i++) {
    fixeddelay_write(p->fdndels[i],p->u[i]+p->f[i]);
  }

  lsum = diffuser_do(p->ldifs[1],lsum);
  lsum = diffuser_do(p->ldifs[2],lsum);
  lsum = diffuser_do(p->ldifs[3],lsum);
  rsum = diffuser_do(p->rdifs[1],rsum);
  rsum = diffuser_do(p->rdifs[2],rsum);
  rsum = diffuser_do(p->rdifs[3],rsum);

  *yl = lsum;
  *yr = rsum;
}

static __inline void gverb_set_roomsize(ty_gverb *p, const float a)
{
  unsigned int i;

    if (a <= 1.0 || (a != a)) {
    p->roomsize = 1.0;
  } else {
    p->roomsize = a;
  }
  p->largestdelay = p->rate * p->roomsize * 0.00294f;

  p->fdnlens[0] = f_round(1.000000f*p->largestdelay);
  p->fdnlens[1] = f_round(0.816490f*p->largestdelay);
  p->fdnlens[2] = f_round(0.707100f*p->largestdelay);
  p->fdnlens[3] = f_round(0.632450f*p->largestdelay);
  for(i = 0; i < FDNORDER; i++) {
    p->fdngains[i] = -powf((float)p->alpha, p->fdnlens[i]);
  }

  p->taps[0] = 5+f_round(0.410f*p->largestdelay);
  p->taps[1] = 5+f_round(0.300f*p->largestdelay);
  p->taps[2] = 5+f_round(0.155f*p->largestdelay);
  p->taps[3] = 5+f_round(0.000f*p->largestdelay);

  for(i = 0; i < FDNORDER; i++) {
    p->tapgains[i] = powf((float)p->alpha, p->taps[i]);
  }

}

static __inline void gverb_set_revtime(ty_gverb *p,float a)
{
  float ga,gt;
  double n;
  unsigned int i;

  p->revtime = a;

  ga = 60.0;
  gt = p->revtime;
  ga = powf(10.0f,-ga/20.0f);
  n = p->rate*gt;
  p->alpha = (double)powf(ga,1.0f/n);

  for(i = 0; i < FDNORDER; i++) {
    p->fdngains[i] = -powf((float)p->alpha, p->fdnlens[i]);
  }

}

static __inline void gverb_set_damping(ty_gverb *p,float a)
{
  unsigned int i;

  p->fdndamping = a;
  for(i = 0; i < FDNORDER; i++) {
    damper_set(p->fdndamps[i],p->fdndamping);
  }
}

static __inline void gverb_set_inputbandwidth(ty_gverb *p,float a)
{
  p->inputbandwidth = a;
  damper_set(p->inputdamper,1.0 - p->inputbandwidth);
}

static __inline void gverb_set_earlylevel(ty_gverb *p,float a)
{
  p->earlylevel = a;
}

static __inline void gverb_set_taillevel(ty_gverb *p,float a)
{
  p->taillevel = a;
}



ty_gverb *gverb_new(int srate, float maxroomsize, float roomsize,
		    float revtime,
		    float damping, float spread,
		    float inputbandwidth, float earlylevel,
		    float taillevel)
{
  ty_gverb *p;
  float ga,gb,gt;
  int i,n;
  float r;
  float diffscale;
  int a,b,c,cc,d,dd,e;
  float spread1,spread2;

  p = (ty_gverb *)malloc(sizeof(ty_gverb));
  p->rate = srate;
  p->fdndamping = damping;
  p->maxroomsize = maxroomsize;
  p->roomsize = roomsize;
  p->revtime = revtime;
  p->earlylevel = earlylevel;
  p->taillevel = taillevel;

  p->maxdelay = p->rate*p->maxroomsize/340.0;
  p->largestdelay = p->rate*p->roomsize/340.0;


  /* Input damper */

  p->inputbandwidth = inputbandwidth;
  p->inputdamper = damper_make(1.0 - p->inputbandwidth);


  /* FDN section */


  p->fdndels = (ty_fixeddelay **)calloc(FDNORDER, sizeof(ty_fixeddelay *));
  for(i = 0; i < FDNORDER; i++) {
    p->fdndels[i] = fixeddelay_make((int)p->maxdelay+1000);
  }
  p->fdngains = (float *)calloc(FDNORDER, sizeof(float));
  p->fdnlens = (int *)calloc(FDNORDER, sizeof(int));

  p->fdndamps = (ty_damper **)calloc(FDNORDER, sizeof(ty_damper *));
  for(i = 0; i < FDNORDER; i++) {
    p->fdndamps[i] = damper_make(p->fdndamping);
  }

  ga = 60.0;
  gt = p->revtime;
  ga = powf(10.0f,-ga/20.0f);
  n = p->rate*gt;
  p->alpha = pow((double)ga, 1.0/(double)n);

  gb = 0.0;
  for(i = 0; i < FDNORDER; i++) {
    if (i == 0) gb = 1.000000*p->largestdelay;
    if (i == 1) gb = 0.816490*p->largestdelay;
    if (i == 2) gb = 0.707100*p->largestdelay;
    if (i == 3) gb = 0.632450*p->largestdelay;

#if 0
    p->fdnlens[i] = nearest_prime((int)gb, 0.5);
#else
    p->fdnlens[i] = f_round(gb);
#endif
    p->fdngains[i] = -powf((float)p->alpha,p->fdnlens[i]);
  }

  p->d = (float *)calloc(FDNORDER, sizeof(float));
  p->u = (float *)calloc(FDNORDER, sizeof(float));
  p->f = (float *)calloc(FDNORDER, sizeof(float));

  /* Diffuser section */

  diffscale = (float)p->fdnlens[3]/(210+159+562+410);
  spread1 = spread;
  spread2 = 3.0*spread;

  b = 210;
  r = 0.125541;
  a = spread1*r;
  c = 210+159+a;
  cc = c-b;
  r = 0.854046;
  a = spread2*r;
  d = 210+159+562+a;
  dd = d-c;
  e = 1341-d;

  p->ldifs = (ty_diffuser **)calloc(4, sizeof(ty_diffuser *));
  p->ldifs[0] = diffuser_make((int)(diffscale*b),0.75);
  p->ldifs[1] = diffuser_make((int)(diffscale*cc),0.75);
  p->ldifs[2] = diffuser_make((int)(diffscale*dd),0.625);
  p->ldifs[3] = diffuser_make((int)(diffscale*e),0.625);

  b = 210;
  r = -0.568366;
  a = spread1*r;
  c = 210+159+a;
  cc = c-b;
  r = -0.126815;
  a = spread2*r;
  d = 210+159+562+a;
  dd = d-c;
  e = 1341-d;

  p->rdifs = (ty_diffuser **)calloc(4, sizeof(ty_diffuser *));
  p->rdifs[0] = diffuser_make((int)(diffscale*b),0.75);
  p->rdifs[1] = diffuser_make((int)(diffscale*cc),0.75);
  p->rdifs[2] = diffuser_make((int)(diffscale*dd),0.625);
  p->rdifs[3] = diffuser_make((int)(diffscale*e),0.625);



  /* Tapped delay section */

  p->tapdelay = fixeddelay_make(44000);
  p->taps = (int *)calloc(FDNORDER, sizeof(int));
  p->tapgains = (float *)calloc(FDNORDER, sizeof(float));

  p->taps[0] = 5+0.410*p->largestdelay;
  p->taps[1] = 5+0.300*p->largestdelay;
  p->taps[2] = 5+0.155*p->largestdelay;
  p->taps[3] = 5+0.000*p->largestdelay;

  for(i = 0; i < FDNORDER; i++) {
    p->tapgains[i] = pow(p->alpha,(double)p->taps[i]);
  }

  return(p);
}

void gverb_free(ty_gverb *p)
{
  int i;

  damper_free(p->inputdamper);
  for(i = 0; i < FDNORDER; i++) {
    fixeddelay_free(p->fdndels[i]);
    damper_free(p->fdndamps[i]);
    diffuser_free(p->ldifs[i]);
    diffuser_free(p->rdifs[i]);
  }
  free(p->fdndels);
  free(p->fdngains);
  free(p->fdnlens);
  free(p->fdndamps);
  free(p->d);
  free(p->u);
  free(p->f);
  free(p->ldifs);
  free(p->rdifs);
  free(p->taps);
  free(p->tapgains);
  fixeddelay_free(p->tapdelay);
  free(p);
}

void gverb_flush(ty_gverb *p)
{
  int i;

  damper_flush(p->inputdamper);
  for(i = 0; i < FDNORDER; i++) {
    fixeddelay_flush(p->fdndels[i]);
    damper_flush(p->fdndamps[i]);
    diffuser_flush(p->ldifs[i]);
    diffuser_flush(p->rdifs[i]);
  }
  memset(p->d, 0, FDNORDER * sizeof(float));
  memset(p->u, 0, FDNORDER * sizeof(float));
  memset(p->f, 0, FDNORDER * sizeof(float));
  fixeddelay_flush(p->tapdelay);
}

#define TRUE 1
#define FALSE 0

ty_diffuser *diffuser_make(int size, float coeff)
{
  ty_diffuser *p;
  int i;

  p = (ty_diffuser *)malloc(sizeof(ty_diffuser));
  p->size = size;
  p->coeff = coeff;
  p->idx = 0;
  p->buf = (float *)malloc(size*sizeof(float));
  for (i = 0; i < size; i++) p->buf[i] = 0.0;
  return(p);
}

void diffuser_free(ty_diffuser *p)
{
  free(p->buf);
  free(p);
}

void diffuser_flush(ty_diffuser *p)
{
  memset(p->buf, 0, p->size * sizeof(float));
}

ty_damper *damper_make(float damping)
{
  ty_damper *p;

  p = (ty_damper *)malloc(sizeof(ty_damper));
  p->damping = damping;
  p->delay = 0.0f;
  return(p);
}

void damper_free(ty_damper *p)
{
  free(p);
}

void damper_flush(ty_damper *p)
{
  p->delay = 0.0f;
}

ty_fixeddelay *fixeddelay_make(int size)
{
  ty_fixeddelay *p;
  int i;

  p = (ty_fixeddelay *)malloc(sizeof(ty_fixeddelay));
  p->size = size;
  p->idx = 0;
  p->buf = (float *)malloc(size*sizeof(float));
  for (i = 0; i < size; i++) p->buf[i] = 0.0;
  return(p);
}

void fixeddelay_free(ty_fixeddelay *p)
{
  free(p->buf);
  free(p);
}

void fixeddelay_flush(ty_fixeddelay *p)
{
  memset(p->buf, 0, p->size * sizeof(float));
}

int isprime(int n)
{
  unsigned int i;
  const unsigned int lim = (int)sqrtf((float)n);

  if (n == 2) return(TRUE);
  if ((n & 1) == 0) return(FALSE);
  for(i = 3; i <= lim; i += 2)
    if ((n % i) == 0) return(FALSE);
  return(TRUE);
}

int nearest_prime(int n, float rerror)
     /* relative error; new prime will be in range
      * [n-n*rerror, n+n*rerror];
      */
{
  int bound,k;

  if (isprime(n)) return(n);
  /* assume n is large enough and n*rerror enough smaller than n */
  bound = n*rerror;
  for(k = 1; k <= bound; k++) {
    if (isprime(n+k)) return(n+k);
    if (isprime(n-k)) return(n-k);
  }
  return(-1);
}

