#include <stdio.h>
#include <wtypes.h>
#include <assert.h>

#include "osc.h"

#define MAX_BLEPS 4096


FILE *glpF;

void ADC_out(double x)
{
	short s;

	x*=0.13f;
	
	if(x > 1.0)
		x = 1.0;
	if(x < -1.0)
		x = -1.0;
	s = (short)(32767 * x);
	
	fwrite(&s, sizeof(s), 1, glpF);
}

/*
 * MinBLEP Stuff
 */

// input master and slave freq
void HardSync(double slavefreq, double masterfreq)
{
double phs = 0, phm = 0;
int i;
double v;
int n=44100*8;
osc_t *lpO=osc_Create();

	for (i=0; i<n; i++)
	{
		v=osc_Play(lpO);
		ADC_out(2 * v - 1);
	}
}


// entry
void main(int argc, char *argv[], char *envp[])
{
	if (!osc_Init()) return;
	
	glpF=fopen("sync.bin","wb");
	if (!glpF)
	{
		printf("Couldn't open file for writing.\n");
		return;
	}

	HardSync(150,1700);

	fclose(glpF);

	osc_Free();
}
