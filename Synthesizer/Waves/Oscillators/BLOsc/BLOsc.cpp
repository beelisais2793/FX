#include "SC_PlugIn.h"
#include <math.h>

/////////////////////////////////////////////////////////////////

/*
 Band Limited Oscillator based on Peter Pabon's algorithm.
 Created by So Oishi on 23/06/14.
 Copyright (c) 2014 So Oishi. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/////////////////////////////////////////////////////////////////

// BASIC ADMINISTRATION

static InterfaceTable *ft;


/////////////////////////////////////////////////////////////////

// Copied and Pasted from OscUGens.cpp

struct BufUnit : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum;
};

struct TableLookup : public BufUnit
{
	double m_cpstoinc, m_radtoinc;
	int32 mTableSize;
	int32 m_lomask;
};

/////////////////////////////////////////////////////////////////


struct BLOsc : public TableLookup
{
	int32 m_phase;
	float m_phasein;
};

extern "C"
{
	void BLOsc_Ctor(BLOsc *unit);
	void BLOsc_next_kkk(BLOsc *unit, int inNumSamples);
	void BLOsc_next_kka(BLOsc *unit, int inNumSamples);
    void BLOsc_next_kak(BLOsc *unit, int inNumSamples);
	void BLOsc_next_kaa(BLOsc *unit, int inNumSamples);
	void BLOsc_next_akk(BLOsc *unit, int inNumSamples);
	void BLOsc_next_aka(BLOsc *unit, int inNumSamples);
    void BLOsc_next_aak(BLOsc *unit, int inNumSamples);
	void BLOsc_next_aaa(BLOsc *unit, int inNumSamples);

};


//////////////////////////////////////////////////////////////////

// CONSTRUCTOR

void BLOsc_Ctor(BLOsc *unit)
{
	int tableSize2 = ft->mSineSize;
	unit->m_radtoinc = tableSize2 * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.;
	unit->m_lomask = (tableSize2 - 1) << 3;
    
    if (INRATE(0) == calc_FullRate){
		if (INRATE(3) == calc_FullRate){
            if (INRATE(4) == calc_FullRate)
                SETCALC(BLOsc_next_aaa);
            else
                SETCALC(BLOsc_next_aak);
        } else {
            if (INRATE(4) == calc_FullRate)
                SETCALC(BLOsc_next_aka);
            else
                SETCALC(BLOsc_next_akk);
        }
    } else {
		if (INRATE(3) == calc_FullRate){
            if (INRATE(4) == calc_FullRate)
                SETCALC(BLOsc_next_kaa);
            else
                SETCALC(BLOsc_next_kak);
        } else {
            if (INRATE(4) == calc_FullRate)
                SETCALC(BLOsc_next_kka);
            else
                SETCALC(BLOsc_next_kkk);
        }};
    unit->m_phase = 0;

	BLOsc_next_kkk(unit, 1);
}

//////////////////////////////////////////////////////////////////

// UGEN CALCULATION

// freq: kr, slope: kr, evenOddRatio: kr
void BLOsc_next_kkk(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float slope = ZIN0(3);
    float evenOddRatio = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1; // The highest harmonic index
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1; // The lowest even harmonic index
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1; // The highest even harmonic index
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
    float evenOddFactor = 1 - evenOddRatio;
    float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));  //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, slope: kr, evenOddRatio: ar
void BLOsc_next_kka(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float slope = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, slope: ar, evenOddRatio: kr
void BLOsc_next_kak(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float *slopein = ZIN(3);
    float evenOddRatio = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    float evenOddFactor = 1 - evenOddRatio;
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float slope = ZXP(slopein);
          float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, slope: ar, evenOddRatio: ar
void BLOsc_next_kaa(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, slope: kr, evenOddRatio: kr
void BLOsc_next_akk(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float slope = ZIN0(3);
    float evenOddRatio = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    float evenOddFactor = 1 - evenOddRatio;
    float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, slope: kr, evenOddRatio: ar
void BLOsc_next_aka(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float slope = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, slope: ar, evenOddRatio: kr
void BLOsc_next_aak(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float *slopein = ZIN(3);
    float evenOddRatio = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    float evenOddFactor = 1 - evenOddRatio;
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float slope = ZXP(slopein);
          float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, slope: ar, evenOddRatio: ar
void BLOsc_next_aaa(BLOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    int32 loHarmonics = ZIN0(1);
    int32 numHarmonics = ZIN0(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    int32 hiHarmonics = loHarmonics + numHarmonics - 1;
    int32 hiHarmonicsPlusOne = hiHarmonics + 1;
    int32 loEvenHarmonics = loHarmonics%2 == 0? loHarmonics : loHarmonics + 1;
    int32 hiEvenHarmonics = hiHarmonics%2 == 0? hiHarmonics : hiHarmonics - 1;
    int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
    int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1;
    
    LOOP1(inNumSamples,
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonics;
          int32 cosPhaseLo = phase * loHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          float ampFactor = 0.99<slope&&slope<1.01? numHarmonics - evenOddFactor * numEvenHarmonics:((pow(slope,loHarmonics) - pow(slope,hiHarmonicsPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2)));
          
          float a = pow(slope,loHarmonics) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonics) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h)))) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

////////////////////////////////////////////////////////////////////

// LOAD FUNCTION

PluginLoad(BLOsc)
{
	ft = inTable;

	DefineSimpleUnit(BLOsc);
}

////////////////////////////////////////////////////////////////////
