#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	OT_SAW,
    OT_RSAW,
	OT_SQUARE,
    OT_TRIANGLE,
    OT_SINE,
} oscwave_t;

typedef struct
{
	double f; // frequency
	double p; // phase
	double v; // output

    
    double triangle;
    double saw;
    double rsaw;
    double square;
    double sine;

	bool bSync;		// hardsync on/off
	float fSync;	// hardsync frequency
	float fPWM;		// pulse width
	float s_rate;

	int type; // square or saw
	
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

#ifdef __cplusplus
extern "C" {
#endif

extern osc_t* lpO[6];
extern minbleptable_t gMinBLEP;

void minBLEP_Load();
void minBLEP_Free();
float osc_Play(osc_t *lpO);
void osc_AddBLEP(osc_t *lpO, double offset, double amp);
osc_t* osc_Create(double rate);

#ifdef __cplusplus
}
#endif
