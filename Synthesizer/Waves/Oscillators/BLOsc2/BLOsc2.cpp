#include "SC_PlugIn.h"
#include <math.h>

/////////////////////////////////////////////////////////////////

/*
 Band Limited Oscillator 2 based on Peter Pabon's algorithm.
 Created by So Oishi on 02/11/14.
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


struct BLOsc2 : public TableLookup
{
    int32 m_phase;
    float m_phasein;
};

extern "C"
{
    void BLOsc2_Ctor(BLOsc2 *unit);
    void BLOsc2_next_kkkkk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkkka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkkak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkkaa(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkakk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkaka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkaak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kkaaa(BLOsc2 *unit, int inNumSamples);

    void BLOsc2_next_kakkk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kakka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kakak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kakaa(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kaakk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kaaka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kaaak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_kaaaa(BLOsc2 *unit, int inNumSamples);

    void BLOsc2_next_akkkk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akkka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akkak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akkaa(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akakk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akaka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akaak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_akaaa(BLOsc2 *unit, int inNumSamples);
    
    void BLOsc2_next_aakkk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aakka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aakak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aakaa(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aaakk(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aaaka(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aaaak(BLOsc2 *unit, int inNumSamples);
    void BLOsc2_next_aaaaa(BLOsc2 *unit, int inNumSamples);
};


//////////////////////////////////////////////////////////////////

// CONSTRUCTOR

void BLOsc2_Ctor(BLOsc2 *unit)
{
    int tableSize2 = ft->mSineSize;
    unit->m_radtoinc = tableSize2 * (rtwopi * 65536.);
    unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.;
    unit->m_lomask = (tableSize2 - 1) << 3;
    
    if (INRATE(0) == calc_FullRate){
        if (INRATE(1) == calc_FullRate){
            if (INRATE(2) == calc_FullRate){
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_aaaaa);
                    else
                        SETCALC(BLOsc2_next_aaaak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_aaaka);
                    else
                        SETCALC(BLOsc2_next_aaakk);}
            } else {
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_aakaa);
                    else
                        SETCALC(BLOsc2_next_aakak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_aakka);
                    else
                        SETCALC(BLOsc2_next_aakkk);}}
        } else {
            if (INRATE(2) == calc_FullRate){
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_akaaa);
                    else
                        SETCALC(BLOsc2_next_akaak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_akaka);
                    else
                        SETCALC(BLOsc2_next_akakk);}
            } else {
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_akkaa);
                    else
                        SETCALC(BLOsc2_next_akkak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_akkka);
                    else
                        SETCALC(BLOsc2_next_akkkk);}}};
    } else {
        if (INRATE(1) == calc_FullRate){
            if (INRATE(2) == calc_FullRate){
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kaaaa);
                    else
                        SETCALC(BLOsc2_next_kaaak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kaaka);
                    else
                        SETCALC(BLOsc2_next_kaakk);}
            } else {
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kakaa);
                    else
                        SETCALC(BLOsc2_next_kakak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kakka);
                    else
                        SETCALC(BLOsc2_next_kakkk);}}
        } else {
            if (INRATE(2) == calc_FullRate){
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kkaaa);
                    else
                        SETCALC(BLOsc2_next_kkaak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kkaka);
                    else
                        SETCALC(BLOsc2_next_kkakk);}
            } else {
                if (INRATE(3) == calc_FullRate){
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kkkaa);
                    else
                        SETCALC(BLOsc2_next_kkkak);
                } else {
                    if (INRATE(4) == calc_FullRate)
                        SETCALC(BLOsc2_next_kkkka);
                    else
                        SETCALC(BLOsc2_next_kkkkk);}}};
    };
    
    unit->m_phase = 0;
    
    BLOsc2_next_kkkkk(unit, 1);
}
//////////////////////////////////////////////////////////////////

// UGEN CALCULATION

// freq: kr, loHarmonics: kr, hiHarmonics: kr, slope: kr, evenOddRatio: kr
void BLOsc2_next_kkkkk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: kr, slope: kr, evenOddRatio: ar
void BLOsc2_next_kkkka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: kr, slope: ar, evenOddRatio: kr
void BLOsc2_next_kkkak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: kr, slope: ar, evenOddRatio: ar
void BLOsc2_next_kkkaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: ar, slope: kr, evenOddRatio: kr
void BLOsc2_next_kkakk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: ar, slope: kr, evenOddRatio: ar
void BLOsc2_next_kkaka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: ar, slope: ar, evenOddRatio: kr
void BLOsc2_next_kkaak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: kr, hiHarmonics: ar, slope: ar, evenOddRatio: ar
void BLOsc2_next_kkaaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: kr, slope: kr, evenOddRatio: kr
void BLOsc2_next_kakkk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: kr, slope: kr, evenOddRatio: ar
void BLOsc2_next_kakka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: kr, slope: ar, evenOddRatio: kr
void BLOsc2_next_kakak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: kr, slope: ar, evenOddRatio: ar
void BLOsc2_next_kakaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: ar, slope: kr, evenOddRatio: kr
void BLOsc2_next_kaakk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: ar, slope: kr, evenOddRatio: ar
void BLOsc2_next_kaaka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: ar, slope: ar, evenOddRatio: kr
void BLOsc2_next_kaaak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: kr, loHarmonics: ar, hiHarmonics: ar, slope: ar, evenOddRatio: ar
void BLOsc2_next_kaaaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float freqin = ZIN0(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    int32 phaseinc = (int32)(unit->m_cpstoinc * freqin);
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += phaseinc;
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: kr, slope: kr, evenOddRatio: kr
void BLOsc2_next_akkkk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: kr, slope: kr, evenOddRatio: ar
void BLOsc2_next_akkka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: kr, slope: ar, evenOddRatio: kr
void BLOsc2_next_akkak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: kr, slope: ar, evenOddRatio: ar
void BLOsc2_next_akkaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: ar, slope: kr, evenOddRatio: kr
void BLOsc2_next_akakk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: ar, slope: kr, evenOddRatio: ar
void BLOsc2_next_akaka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: ar, slope: ar, evenOddRatio: kr
void BLOsc2_next_akaak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: kr, hiHarmonics: ar, slope: ar, evenOddRatio: ar
void BLOsc2_next_akaaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float loHarmonicsin = ZIN0(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = loHarmonicsin;
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: kr, slope: kr, evenOddRatio: kr
void BLOsc2_next_aakkk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: kr, slope: kr, evenOddRatio: ar
void BLOsc2_next_aakka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: kr, slope: ar, evenOddRatio: kr
void BLOsc2_next_aakak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: kr, slope: ar, evenOddRatio: ar
void BLOsc2_next_aakaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float hiHarmonicsin = ZIN0(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = hiHarmonicsin;
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: ar, slope: kr, evenOddRatio: kr
void BLOsc2_next_aaakk(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: ar, slope: kr, evenOddRatio: ar
void BLOsc2_next_aaaka(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float slopein = ZIN0(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = slopein;
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: ar, slope: ar, evenOddRatio: kr
void BLOsc2_next_aaaak(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float evenOddRatioin = ZIN0(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;
          
          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = evenOddRatioin;
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.
          
          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}

// freq: ar, loHarmonics: ar, hiHarmonics: ar, slope: ar, evenOddRatio: ar
void BLOsc2_next_aaaaa(BLOsc2 *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    float *freqin = ZIN(0);
    float *loHarmonicsin = ZIN(1);
    float *hiHarmonicsin = ZIN(2);
    float *slopein = ZIN(3);
    float *evenOddRatioin = ZIN(4);
    
    float *table0 = ft->mSineWavetable;
    float *table1 = table0 + 1;
    
    int32 phase = unit->m_phase;
    int32 lomask = unit->m_lomask;
    
    float cpstoinc = unit->m_cpstoinc;
    float radtoinc = unit->m_radtoinc;
    
    
    LOOP1(inNumSamples,
          float loHarmonics = ZXP(loHarmonicsin);
          int32 loHarmonicsInt = ceil(loHarmonics);
          int32 loHarmonicsIntMinusOne = loHarmonicsInt - 1;
          float loHarmonicsFrac = loHarmonicsInt - loHarmonics;

          float hiHarmonics = ZXP(hiHarmonicsin);
          int32 hiHarmonicsInt = floor(hiHarmonics);
          float hiHarmonicsFrac = hiHarmonics - hiHarmonicsInt;
          
          int32 hiHarmonicsIntPlusOne = hiHarmonicsInt + 1;
          int32 loEvenHarmonics = loHarmonicsInt%2 == 0? loHarmonicsInt : loHarmonicsInt + 1; // The lowest even harmonic index
          int32 hiEvenHarmonics = hiHarmonicsInt%2 == 0? hiHarmonicsInt : hiHarmonicsInt - 1; // The highest even harmonic index
          int32 hiEvenHarmonicsPlusTwo = hiEvenHarmonics + 2;
          int32 numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2 + 1; //The total number of even harmonics
          
          float slope = ZXP(slopein);
          float evenOddRatio = ZXP(evenOddRatioin);
          float evenOddFactor = 1 - evenOddRatio;
          
          float fundamentalAdjust = loHarmonicsInt%2 == 0? loHarmonicsFrac : evenOddRatio * loHarmonicsFrac;
          float extraHarmonicsAdjust = hiHarmonicsInt%2 == 0? hiHarmonicsFrac : evenOddRatio * hiHarmonicsFrac;
          
          float ampFactor = 0.99<slope&&slope<1.01? (hiHarmonicsInt - loHarmonicsInt + 1) - evenOddFactor * numEvenHarmonics + fundamentalAdjust + extraHarmonicsAdjust:((pow(slope,loHarmonicsInt) - pow(slope,hiHarmonicsIntPlusOne)) / (1 - slope)) - (evenOddFactor * (pow(slope, loEvenHarmonics) - pow(slope, hiEvenHarmonicsPlusTwo)) / (1 - pow(slope, 2))) + fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) + extraHarmonicsAdjust * pow(slope, hiHarmonicsIntPlusOne);
          //ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope falls between 0.99 and 1.01.

          int32 sinPhaseBase = phase;
          int32 cosPhaseBase = phase + (int32)(radtoinc * pi2);
          int32 sinPhaseLo = phase * loHarmonicsInt;
          int32 sinPhaseLoMinusOne = phase * loHarmonicsIntMinusOne;
          int32 cosPhaseLo = phase * loHarmonicsInt + (int32)(radtoinc * pi2);
          int32 sinPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne;
          int32 cosPhaseHiPlusOne = phase * hiHarmonicsIntPlusOne + (int32)(radtoinc * pi2);
          int32 sinPhase2 = phase * 2;
          int32 cosPhase2 = phase * 2 + (int32)(radtoinc * pi2);
          int32 sinPhaseLoEven = phase * loEvenHarmonics;
          int32 cosPhaseLoEven = phase * loEvenHarmonics + (int32)(radtoinc * pi2);
          int32 sinPhaseHiEvenPlusTwo = phase * hiEvenHarmonicsPlusTwo;
          int32 cosPhaseHiEvenPlusTWo = phase * hiEvenHarmonicsPlusTwo + (int32)(radtoinc * pi2);
          
          float a = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, cosPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, cosPhaseHiPlusOne, lomask);
          float b = pow(slope,loHarmonicsInt) * lookupi1(table0, table1, sinPhaseLo, lomask) - pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          float c =  1 - slope * lookupi1(table0, table1, cosPhaseBase, lomask);
          float d = - slope * lookupi1(table0, table1, sinPhaseBase, lomask);
          
          float e = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, cosPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, cosPhaseHiEvenPlusTWo, lomask));
          float f = evenOddFactor * (pow(slope,loEvenHarmonics) * lookupi1(table0, table1, sinPhaseLoEven, lomask) - pow(slope,hiEvenHarmonicsPlusTwo) * lookupi1(table0, table1, sinPhaseHiEvenPlusTwo, lomask));
          float g =  1 - pow(slope, 2) * lookupi1(table0, table1, cosPhase2, lomask);
          float h = - pow(slope, 2) * lookupi1(table0, table1, sinPhase2, lomask);
          float fractionalFundamental = fundamentalAdjust * pow(slope,loHarmonicsIntMinusOne) * lookupi1(table0, table1, sinPhaseLoMinusOne, lomask);
          float fractionalExtraHarmonics = extraHarmonicsAdjust * pow(slope,hiHarmonicsIntPlusOne) * lookupi1(table0, table1, sinPhaseHiPlusOne, lomask);
          
          float z = ((((b*c) - (a*d))  / ((c*c) + (d*d))) - (((f*g) - (e*h))  / ((g*g) + (h*h))) + fractionalFundamental + fractionalExtraHarmonics) / ampFactor;
          phase += (int32)(cpstoinc * ZXP(freqin));
          ZXP(out) = z;
          );
    unit->m_phase = phase;
}


////////////////////////////////////////////////////////////////////

// LOAD FUNCTION

PluginLoad(BLOsc2)
{
    ft = inTable;
    
    DefineSimpleUnit(BLOsc2);
}

////////////////////////////////////////////////////////////////////
