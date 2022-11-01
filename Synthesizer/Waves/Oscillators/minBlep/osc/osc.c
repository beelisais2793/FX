#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include "defines.h"
#include "osc.h"

/*
 * BLEP Table
 */

minbleptable_t gMinBLEP;

BOOL minBLEP_Init()
{
	// load table
	FILE *fp=fopen("minblep.mat","rb");
	unsigned int iSize;

	if (!fp) return FALSE;

	fseek(fp,0x134,SEEK_SET);

	fread(&iSize,sizeof(int),1,fp);
	gMinBLEP.c=iSize/sizeof(double);

	gMinBLEP.lpTable=(double*)malloc(iSize);
	if (!gMinBLEP.lpTable) return FALSE;

	fread(gMinBLEP.lpTable,iSize,1,fp);

	fclose(fp);

	return TRUE;
}

void minBLEP_Free()
{
	free(gMinBLEP.lpTable);
}

/*
 * OSC Init
 */
BOOL osc_Init()
{
	if (!minBLEP_Init()) return FALSE;

	return TRUE;
}

void osc_Free()
{
	minBLEP_Free();
}

double fs;

osc_t *osc_Create()
{
	osc_t *lpO=(osc_t*)malloc(sizeof(osc_t));

	// frequency
	lpO->f=440;
	lpO->p=0;
	lpO->v=0;

	// stuff
	lpO->bSync=FALSE;
	lpO->fPWM=0.5;
	lpO->type=OT_SAW;

	// buffer
	lpO->cBuffer=gMinBLEP.c/KTABLE;
	lpO->buffer=(double*)malloc(sizeof(double)*lpO->cBuffer);
	lpO->iBuffer=0;
	lpO->nInit=0;

	return lpO;
}

#define LERP(A,B,F) ((B-A)*F+A)

// add impulse into buffer
void osc_AddBLEP(osc_t *lpO, double offset, double amp)
{
int i;
double *lpOut=lpO->buffer+lpO->iBuffer;
double *lpIn=gMinBLEP.lpTable+(int)(KTABLE*offset);
double frac=fmod(KTABLE*offset,1.0);
int cBLEP=(gMinBLEP.c/KTABLE)-1;
double *lpBufferEnd=lpO->buffer+lpO->cBuffer;
double f;

	// add
	for (i=0; i<lpO->nInit; i++, lpIn+=KTABLE, lpOut++)
	{
		if (lpOut>=lpBufferEnd) lpOut=lpO->buffer;
		f=LERP(lpIn[0],lpIn[1],frac);
		*lpOut+=amp*(1-f);
	}

	// copy
	for (; i<cBLEP; i++, lpIn+=KTABLE, lpOut++)
	{
		if (lpOut>=lpBufferEnd) lpOut=lpO->buffer;
		f=LERP(lpIn[0],lpIn[1],frac);
		*lpOut=amp*(1-f);
	}

	lpO->nInit=cBLEP;
}

// play waveform
double osc_Play(osc_t *lpO)
{
double v;
double fs=lpO->f/44100;

	// create waveform
	lpO->p=lpO->p+fs;

	// add BLEP at end of waveform
	if (lpO->p>=1)
	{
		lpO->p=lpO->p-1.0;
		lpO->v=0.0f;
		osc_AddBLEP(lpO, lpO->p/fs,1.0f);
	}

	// add BLEP in middle of wavefor for squarewave
	if (!lpO->v && lpO->p>lpO->fPWM && lpO->type==OT_SQUARE)
	{
		lpO->v=1.0f;
		osc_AddBLEP(lpO, (lpO->p-lpO->fPWM)/fs,-1.0f);
	}

	// sample value
	if (lpO->type==OT_SAW)
		v=lpO->p;
	else
		v=lpO->v;

	// add BLEP buffer contents
	if (lpO->nInit)
	{
		v+=lpO->buffer[lpO->iBuffer];
		lpO->nInit--;
		if (++lpO->iBuffer>=lpO->cBuffer) lpO->iBuffer=0;
	}

	return v;
}