#include "minblep.h"

osc_t* lpO[6];
minbleptable_t gMinBLEP;

osc_t* osc_Create(double rate)
{
	osc_t *lpO=(osc_t*)malloc(sizeof(osc_t));

	// frequency
	lpO->f=440;
	lpO->p=0;
	lpO->v=0;

	// stuff
	lpO->bSync=false;
	lpO->fPWM=0.5;
	lpO->type=OT_TRIANGLE;
	lpO->s_rate = rate;

	// buffer
	lpO->cBuffer = 1024; //gMinBLEP.c/KTABLE;
	lpO->buffer=(double*)malloc(sizeof(double)*lpO->cBuffer);
	lpO->iBuffer=0;
	lpO->nInit=0;

	return lpO;
}

#define LERP(A,B,F) ((B-A)*F+A)

//----------------------------------------------------------

// add impulse into buffer
void osc_AddBLEP(osc_t *lpO, double offset, double amp)
{
int i;
double *lpOut=lpO->buffer+lpO->iBuffer;
double *lpIn=gMinBLEP.lpTable+(uint)(KTABLE*offset);
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

//----------------------------------------------------------

float sign(float x) {
    if(x < 0) return -1;
    return 1;
}

// play waveform
float osc_Play(osc_t *lpO)
{

double v;
double fs=lpO->f / lpO->s_rate;
static double phase = 0;

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

    

    // these fruities are only positive 
    lpO->saw  = lpO->p;     
    lpO->rsaw = 1.0-lpO->saw;
    lpO->square = lpO->v;

    
    
    
	// sample value
	if (lpO->type==OT_SAW)
		v = lpO->saw;    
    else if(lpO->type == OT_RSAW)
        v = lpO->rsaw;
	else
		v = lpO->square;

	// add BLEP buffer contents
	if (lpO->nInit)
	{
		v+=lpO->buffer[lpO->iBuffer];
		lpO->nInit--;
		if (++lpO->iBuffer>=lpO->cBuffer) lpO->iBuffer=0;
	}

    // these fruites dont need to be maladjusted further
    lpO->sine  = std::sin(2*M_PI*lpO->p);
    lpO->triangle = asin(lpO->sine)/1.5;
    if(lpO->type == OT_TRIANGLE)
        return lpO->triangle;
    else if(lpO->type == OT_SINE)
        return lpO->sine;
	return (2*v-1);
}


//----------------------------------


void minBLEP_Load()
{
    FILE *fp=fopen("minblep.mat","rb");
	unsigned int iSize;

	fseek(fp,0x134,SEEK_SET);

	int err = fread(&iSize,sizeof(uint),1,fp);
	gMinBLEP.c=iSize/sizeof(double);

	gMinBLEP.lpTable=(double*)malloc(iSize);

	err = fread(gMinBLEP.lpTable,iSize,1,fp);

	fclose(fp);
}

void minBLEP_Free()
{
    free(gMinBLEP.lpTable);
}

 
