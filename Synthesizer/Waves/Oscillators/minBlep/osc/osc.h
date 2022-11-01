#ifndef _OSC_H_
#define _OSC_H_

#define SAMPLE_FREQUENCY 44100

typedef enum
{
	OT_SAW,
	OT_SQUARE,
} oscwave_t;

typedef struct
{
	double f; // frequency
	double p; // phase
	double v; // output

	BOOL bSync;		// hardsync on/off
	double fSync;	// hardsync frequency
	double fPWM;		// pulse width

	oscwave_t type; // square or saw
	
	double *buffer; // circular output buffer
	int cBuffer;	// buffer size
	int iBuffer;	// current buffer position
	int nInit;		// amount of initialized entries
} osc_t;

#define KTABLE 64 // BLEP table oversampling factor

typedef struct
{
	double *lpTable;
	int c;
} minbleptable_t;

BOOL osc_Init();
osc_t *osc_Create();
void osc_AddBLEP(osc_t *lpO, double offset, double amp);
double osc_Play(osc_t *lpO);
void osc_Free();

#endif