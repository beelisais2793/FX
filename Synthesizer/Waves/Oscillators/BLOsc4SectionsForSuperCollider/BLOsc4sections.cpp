#include "SC_PlugIn.h"
#include <tgmath.h>

using namespace std;


// BASIC ADMINISTRATION

static InterfaceTable *ft;

struct BLOsc4sections : public Unit
{
float partialOmega; // 2pi/sr : phase increment per sample while frequency = 1 
float phaseCurrent;

// For section 1
float loHarmonicsIntCurrent1;
float loHarmonicsFracCurrent1;
float hiHarmonicsIntCurrent1;
float hiHarmonicsFracCurrent1;
float spreadCurrent1;

// For section 2
float loHarmonicsIntCurrent2;
float loHarmonicsFracCurrent2;
float hiHarmonicsIntCurrent2;
float hiHarmonicsFracCurrent2;
float spreadCurrent2;

// For section 3
float loHarmonicsIntCurrent3;
float loHarmonicsFracCurrent3;
float hiHarmonicsIntCurrent3;
float hiHarmonicsFracCurrent3;
float spreadCurrent3;

// For section 4
float loHarmonicsIntCurrent4;
float loHarmonicsFracCurrent4;
float hiHarmonicsIntCurrent4;
float hiHarmonicsFracCurrent4;
float spreadCurrent4;

// To check if each argument is "ar" or "kr"
int freqRate; // ar:0  kr:1
int loHarmonicsRate1; // ar:0  kr:1
int hiHarmonicsRate1; // ar:0  kr:1
int slopeRate1; // ar:0  kr:1
int evenOddRatioRate1; // ar:0  kr:1
int sectionAmpRate1; // ar:0  kr:1

int loHarmonicsRate2; // ar:0  kr:1
int hiHarmonicsRate2; // ar:0  kr:1
int slopeRate2; // ar:0  kr:1
int evenOddRatioRate2; // ar:0  kr:1
int sectionAmpRate2; // ar:0  kr:1

int loHarmonicsRate3; // ar:0  kr:1
int hiHarmonicsRate3; // ar:0  kr:1
int slopeRate3; // ar:0  kr:1
int evenOddRatioRate3; // ar:0  kr:1
int sectionAmpRate3; // ar:0  kr:1

int loHarmonicsRate4; // ar:0  kr:1
int hiHarmonicsRate4; // ar:0  kr:1
int slopeRate4; // ar:0  kr:1
int evenOddRatioRate4; // ar:0  kr:1
int sectionAmpRate4; // ar:0  kr:1

};

extern "C"
{
  void BLOsc4sections_next(BLOsc4sections *unit, int inNumSamples);
  void BLOsc4sections_Ctor(BLOsc4sections* unit);
};

//////////////////////////////////////////////////////////////////

// CONSTRUCTOR

void BLOsc4sections_Ctor(BLOsc4sections* unit)
{

  SETCALC(BLOsc4sections_next);

  unit->partialOmega = twopi_f / SAMPLERATE;
  unit->phaseCurrent = 0.0f;

  unit->loHarmonicsIntCurrent1 = 0.0f;
  unit->loHarmonicsFracCurrent1 = 0.0f;
  unit->hiHarmonicsIntCurrent1 = 0.0f;
  unit->hiHarmonicsFracCurrent1 = 0.0f;
  unit->spreadCurrent1 = 0.0f;
  
  unit->loHarmonicsIntCurrent2 = 0.0f;
  unit->loHarmonicsFracCurrent2 = 0.0f;
  unit->hiHarmonicsIntCurrent2 = 0.0f;
  unit->hiHarmonicsFracCurrent2 = 0.0f;
  unit->spreadCurrent2 = 0.0f;

  unit->loHarmonicsIntCurrent3 = 0.0f;
  unit->loHarmonicsFracCurrent3 = 0.0f;
  unit->hiHarmonicsIntCurrent3 = 0.0f;
  unit->hiHarmonicsFracCurrent3 = 0.0f;
  unit->spreadCurrent3 = 0.0f;

  unit->loHarmonicsIntCurrent4 = 0.0f;
  unit->loHarmonicsFracCurrent4 = 0.0f;
  unit->hiHarmonicsIntCurrent4 = 0.0f;
  unit->hiHarmonicsFracCurrent4 = 0.0f;
  unit->spreadCurrent4 = 0.0f;

  if (INRATE(0) == calc_FullRate) unit->freqRate = 0; else unit->freqRate = 1;

  if (INRATE(1) == calc_FullRate) unit->loHarmonicsRate1 = 0; else unit->loHarmonicsRate1 = 1;
  if (INRATE(2) == calc_FullRate) unit->hiHarmonicsRate1 = 0; else unit->hiHarmonicsRate1 = 1;
  if (INRATE(3) == calc_FullRate) unit->slopeRate1 = 0; else unit->slopeRate1 = 1;
  if (INRATE(4) == calc_FullRate) unit->evenOddRatioRate1 = 0; else unit->evenOddRatioRate1 = 1;
  if (INRATE(6) == calc_FullRate) unit->sectionAmpRate1 = 0; else unit->sectionAmpRate1 = 1;

  if (INRATE(7) == calc_FullRate) unit->loHarmonicsRate2 = 0; else unit->loHarmonicsRate2 = 1;
  if (INRATE(8) == calc_FullRate) unit->hiHarmonicsRate2 = 0; else unit->hiHarmonicsRate2 = 1;
  if (INRATE(9) == calc_FullRate) unit->slopeRate2 = 0; else unit->slopeRate2 = 1;
  if (INRATE(10) == calc_FullRate) unit->evenOddRatioRate2 = 0; else unit->evenOddRatioRate2 = 1;
  if (INRATE(12) == calc_FullRate) unit->sectionAmpRate2 = 0; else unit->sectionAmpRate2 = 1;

  if (INRATE(13) == calc_FullRate) unit->loHarmonicsRate3 = 0; else unit->loHarmonicsRate3 = 1;
  if (INRATE(14) == calc_FullRate) unit->hiHarmonicsRate3 = 0; else unit->hiHarmonicsRate3 = 1;
  if (INRATE(15) == calc_FullRate) unit->slopeRate3 = 0; else unit->slopeRate3 = 1;
  if (INRATE(16) == calc_FullRate) unit->evenOddRatioRate3 = 0; else unit->evenOddRatioRate3 = 1;
  if (INRATE(18) == calc_FullRate) unit->sectionAmpRate3 = 0; else unit->sectionAmpRate3 = 1;

  if (INRATE(19) == calc_FullRate) unit->loHarmonicsRate4 = 0; else unit->loHarmonicsRate4 = 1;
  if (INRATE(20) == calc_FullRate) unit->hiHarmonicsRate4 = 0; else unit->hiHarmonicsRate4 = 1;
  if (INRATE(21) == calc_FullRate) unit->slopeRate4 = 0; else unit->slopeRate4 = 1;
  if (INRATE(22) == calc_FullRate) unit->evenOddRatioRate4 = 0; else unit->evenOddRatioRate4 = 1;
  if (INRATE(24) == calc_FullRate) unit->sectionAmpRate4 = 0; else unit->sectionAmpRate4 = 1;


  BLOsc4sections_next(unit, 1);
}

//////////////////////////////////////////////////////////////////

// Function to calculate amp factor

float ampFactor_calc (float slope, float evenOddFactor, float loHarmonicsIntCurrent, float hiHarmonicsIntCurrent, float loEvenHarmonics, float hiEvenHarmonics, float numEvenHarmonics, float fundamentalAdjust, float extraHarmonicsAdjust)
{
  float result;
  result = slope == 1.0f? 
(hiHarmonicsIntCurrent - loHarmonicsIntCurrent + 1.0f) - (evenOddFactor * numEvenHarmonics) + fundamentalAdjust + extraHarmonicsAdjust
:
(pow(slope,loHarmonicsIntCurrent) - pow(slope,hiHarmonicsIntCurrent+1.0f))
/
(1.0f - slope)
- 
(evenOddFactor * 
  (pow(slope, loEvenHarmonics)
    -
    pow(slope, hiEvenHarmonics + 2.0f)
    )
  /
  (1.0f - pow(slope, 2.0f))) 
+
fundamentalAdjust * pow(slope,loHarmonicsIntCurrent-1.0f)
+
extraHarmonicsAdjust * pow(slope, hiHarmonicsIntCurrent+1.0f)
;
return result;
}
//ampFactor will be used to normalize the output amplitude. To avoid the denominator of this calculation to be 0 when slope = 1, the different formula is used when slope == 1.


// Function to calculte the first and the last fractional harmonics
complex<float> fractionalFundamental_calc (float phaseCurrent, float slope, float loHarmonicsIntCurrent, float fundamentalAdjust)
{
  complex<float> result;
  result = 
  fundamentalAdjust * 
  pow(slope,loHarmonicsIntCurrent-1.0f) * 
  exp(complex<float>(0.0f, (loHarmonicsIntCurrent-1.0f)*phaseCurrent));
  return result;
}

complex<float> fractionalExtraHarmonics_calc (float phaseCurrent, float slope, float hiHarmonicsIntCurrent, float extraHarmonicsAdjust)
{
  complex<float> result;
  result = 
  extraHarmonicsAdjust * 
  pow(slope,hiHarmonicsIntCurrent+1.0f) * 
  exp(complex<float>(0.0f, (hiHarmonicsIntCurrent+1.0f)*phaseCurrent));
  return result;
}


// Function to calculate complex signal
complex<float> complexSignal_calc (complex<float> baseOsc, complex<float> fractionalFundamental, complex<float> fractionalExtraHarmonics, float phaseCurrent, float slope, float evenOddFactor, float spreadCurrent, int spreadCompensation, float loHarmonicsIntCurrent, float hiHarmonicsIntCurrent, float loEvenHarmonics, float hiEvenHarmonics, float ampFactor)
{
  complex<float> result;
  if (slope == 1.0f && phaseCurrent == 0.0f) {
    result = complex<float>(0.0f, 1.0f);
  }
  else {
    result = 
    (
      pow(slope * baseOsc, loHarmonicsIntCurrent)
      -
      pow(slope * baseOsc, hiHarmonicsIntCurrent + 1.0f)
      )
    /
    (1.0f - slope * baseOsc)
    - 
    evenOddFactor * 
    (
      pow(slope * baseOsc, loEvenHarmonics)
      -
      pow(slope * baseOsc, hiEvenHarmonics + 2.0f)
      )
    /
    (1.0f - pow(slope * baseOsc, 2.0f))
    + fractionalFundamental
    + fractionalExtraHarmonics
    ;

    result = pow(result/ampFactor, spreadCurrent);
  };

/* FREQUENCY SHIFTER */
  if (spreadCompensation != 0) {
   result = polar(abs(result), arg(result) - (phaseCurrent * loHarmonicsIntCurrent * (spreadCurrent - 1.0f)));
 }
  // if spreadCompensation is not 0, frequency shifts down according to given spread value (in other words, it always keeps the same lowest harmonic number)
  // if spread is 0, frequency shift does NOT happen (Therefore it keeps the original result of power function)

  return result;
}


// Function to calculate one block as real signal (imag = sine phase)

float oneBlock_calc (complex<float> baseOsc, float phaseCurrent, float slope, float evenOddRatio, float spreadCurrent, int spreadCompensation, float loHarmonicsIntCurrent, float hiHarmonicsIntCurrent, float loHarmonicsFracCurrent, float hiHarmonicsFracCurrent)
{
  float loEvenHarmonics = static_cast<int>(loHarmonicsIntCurrent)%2 == 0? loHarmonicsIntCurrent : loHarmonicsIntCurrent + 1.0f; // The lowest even harmonic index
float hiEvenHarmonics = static_cast<int>(hiHarmonicsIntCurrent)%2 == 0? hiHarmonicsIntCurrent : hiHarmonicsIntCurrent - 1.0f; // The highest even harmonic index
float numEvenHarmonics = (hiEvenHarmonics - loEvenHarmonics) / 2.0f + 1.0f; //The total number of even harmonics

float evenOddFactor = 1.0f - evenOddRatio;

  float fundamentalAdjust = static_cast<int>(loHarmonicsIntCurrent)%2 == 0? loHarmonicsFracCurrent : evenOddRatio * loHarmonicsFracCurrent;
float extraHarmonicsAdjust = static_cast<int>(hiHarmonicsIntCurrent)%2 == 0? hiHarmonicsFracCurrent : evenOddRatio * hiHarmonicsFracCurrent;

  float ampFactor = ampFactor_calc(slope, evenOddFactor, loHarmonicsIntCurrent, hiHarmonicsIntCurrent, loEvenHarmonics, hiEvenHarmonics, numEvenHarmonics, fundamentalAdjust, extraHarmonicsAdjust);

complex<float> fractionalFundamental = fractionalFundamental_calc(phaseCurrent, slope, loHarmonicsIntCurrent, fundamentalAdjust);
complex<float> fractionalExtraHarmonics = fractionalExtraHarmonics_calc(phaseCurrent, slope, hiHarmonicsIntCurrent, extraHarmonicsAdjust);

complex<float> signalC = 
complexSignal_calc(baseOsc, fractionalFundamental, fractionalExtraHarmonics, phaseCurrent, slope, evenOddFactor, spreadCurrent, spreadCompensation, loHarmonicsIntCurrent, hiHarmonicsIntCurrent, loEvenHarmonics, hiEvenHarmonics, ampFactor); 
// signal as complex number

float signalR = signalC.imag(); // signal as real number
return signalR;
}

//////////////////////////////////////////////////////////////////

// UGEN CALCULATION

void BLOsc4sections_next(BLOsc4sections *unit, int inNumSamples)
{

float *out = ZOUT(0);

float *freqIn = ZIN(0);
float freq = ZIN0(0);


/* Arguments for section 1 */
float *loHarmonicsIn1 = ZIN(1);
float loHarmonics1 = ZIN0(1);

float *hiHarmonicsIn1 = ZIN(2);
float hiHarmonics1 = ZIN0(2);

float *slopeIn1 = ZIN(3);
float slope1 = ZIN0(3);

float *evenOddRatioIn1 = ZIN(4);
float evenOddRatio1 = ZIN0(4);

int spreadIn1 = ZIN0(5);
float spreadCurrent1 = unit->spreadCurrent1 == 0.0f? static_cast<float>(spreadIn1) : unit->spreadCurrent1;
// initial value = ZIN0(5), value after running = unit->spreadCurrent

float *sectionAmpIn1 = ZIN(6);
float sectionAmp1 = ZIN0(6);

int sectionIsOn1 = (loHarmonics1 == 0.0f || hiHarmonics1 == 0.0f)? 0 : 1;
// if both loHarmonics and hiHarmonics are not 0, the section is active

/* Arguments for section 2 */
float *loHarmonicsIn2 = ZIN(7);
float loHarmonics2 = ZIN0(7);

float *hiHarmonicsIn2 = ZIN(8);
float hiHarmonics2 = ZIN0(8);

float *slopeIn2 = ZIN(9);
float slope2 = ZIN0(9);

float *evenOddRatioIn2 = ZIN(10);
float evenOddRatio2 = ZIN0(10);

int spreadIn2 = ZIN0(11);
float spreadCurrent2 = unit->spreadCurrent2 == 0.0f? static_cast<float>(spreadIn2) : unit->spreadCurrent2;
// initial value = ZIN0(11), value after running = unit->spreadCurrent

float *sectionAmpIn2 = ZIN(12);
float sectionAmp2 = ZIN0(12);

int sectionIsOn2 = (loHarmonics2 == 0.0f || hiHarmonics2 == 0.0f)? 0 : 1;
// if both loHarmonics and hiHarmonics are not 0, the section is active


/* Arguments for section 3 */
float *loHarmonicsIn3 = ZIN(13);
float loHarmonics3 = ZIN0(13);

float *hiHarmonicsIn3 = ZIN(14);
float hiHarmonics3 = ZIN0(14);

float *slopeIn3 = ZIN(15);
float slope3 = ZIN0(15);

float *evenOddRatioIn3 = ZIN(16);
float evenOddRatio3 = ZIN0(16);

int spreadIn3 = ZIN0(17);
float spreadCurrent3 = unit->spreadCurrent3 == 0.0f? static_cast<float>(spreadIn3) : unit->spreadCurrent3;
// initial value = ZIN0(17), value after running = unit->spreadCurrent

float *sectionAmpIn3 = ZIN(18);
float sectionAmp3 = ZIN0(18);

int sectionIsOn3 = (loHarmonics3 == 0.0f || hiHarmonics3 == 0.0f)? 0 : 1;
// if both loHarmonics and hiHarmonics are not 0, the section is active


/* Arguments for section 4 */
float *loHarmonicsIn4 = ZIN(19);
float loHarmonics4 = ZIN0(19);

float *hiHarmonicsIn4 = ZIN(20);
float hiHarmonics4 = ZIN0(20);

float *slopeIn4 = ZIN(21);
float slope4 = ZIN0(21);

float *evenOddRatioIn4 = ZIN(22);
float evenOddRatio4 = ZIN0(22);

int spreadIn4 = ZIN0(23);
float spreadCurrent4 = unit->spreadCurrent4 == 0.0f? static_cast<float>(spreadIn4) : unit->spreadCurrent4;
// initial value = ZIN0(23), value after running = unit->spreadCurrent

float *sectionAmpIn4 = ZIN(24);
float sectionAmp4 = ZIN0(24);

int sectionIsOn4 = (loHarmonics4 == 0.0f || hiHarmonics4 == 0.0f)? 0 : 1;
// if both loHarmonics and hiHarmonics are not 0, the section is active


int spreadCompensation = ZIN0(25);

float phaseCurrent = unit->phaseCurrent;

/* Section 1 */
float loHarmonicsIntCurrent1 = unit->loHarmonicsIntCurrent1 == 0.0f? ceil(loHarmonics1) : unit->loHarmonicsIntCurrent1;
float hiHarmonicsIntCurrent1 = unit->hiHarmonicsIntCurrent1 == 0.0f? floor(hiHarmonics1) : unit->hiHarmonicsIntCurrent1;
// unit->... == 0? means set up initial value when starting the UGen

float loHarmonicsFracCurrent1 = unit->loHarmonicsFracCurrent1 == 0.0f? loHarmonicsIntCurrent1 - loHarmonics1 : unit->loHarmonicsFracCurrent1;
float hiHarmonicsFracCurrent1 = unit->hiHarmonicsFracCurrent1 == 0.0f? hiHarmonics1 - hiHarmonicsIntCurrent1 : unit->hiHarmonicsFracCurrent1;
// unit->... == 0? means set up initial value when starting the UGen

/* Section 2 */
float loHarmonicsIntCurrent2 = unit->loHarmonicsIntCurrent2 == 0.0f? ceil(loHarmonics2) : unit->loHarmonicsIntCurrent2;
float hiHarmonicsIntCurrent2 = unit->hiHarmonicsIntCurrent2 == 0.0f? floor(hiHarmonics2) : unit->hiHarmonicsIntCurrent2;

float loHarmonicsFracCurrent2 = unit->loHarmonicsFracCurrent2 == 0.0f? loHarmonicsIntCurrent2 - loHarmonics2 : unit->loHarmonicsFracCurrent2;
float hiHarmonicsFracCurrent2 = unit->hiHarmonicsFracCurrent2 == 0.0f? hiHarmonics2 - hiHarmonicsIntCurrent2 : unit->hiHarmonicsFracCurrent2;

/* Section 3 */
float loHarmonicsIntCurrent3 = unit->loHarmonicsIntCurrent3 == 0.0f? ceil(loHarmonics3) : unit->loHarmonicsIntCurrent3;
float hiHarmonicsIntCurrent3 = unit->hiHarmonicsIntCurrent3 == 0.0f? floor(hiHarmonics3) : unit->hiHarmonicsIntCurrent3;

float loHarmonicsFracCurrent3 = unit->loHarmonicsFracCurrent3 == 0.0f? loHarmonicsIntCurrent3 - loHarmonics3 : unit->loHarmonicsFracCurrent3;
float hiHarmonicsFracCurrent3 = unit->hiHarmonicsFracCurrent3 == 0.0f? hiHarmonics3 - hiHarmonicsIntCurrent3 : unit->hiHarmonicsFracCurrent3;

/* Section 4 */
float loHarmonicsIntCurrent4 = unit->loHarmonicsIntCurrent4 == 0.0f? ceil(loHarmonics4) : unit->loHarmonicsIntCurrent4;
float hiHarmonicsIntCurrent4 = unit->hiHarmonicsIntCurrent4 == 0.0f? floor(hiHarmonics4) : unit->hiHarmonicsIntCurrent4;

float loHarmonicsFracCurrent4 = unit->loHarmonicsFracCurrent4 == 0.0f? loHarmonicsIntCurrent4 - loHarmonics4 : unit->loHarmonicsFracCurrent4;
float hiHarmonicsFracCurrent4 = unit->hiHarmonicsFracCurrent4 == 0.0f? hiHarmonics4 - hiHarmonicsIntCurrent4 : unit->hiHarmonicsFracCurrent4;


LOOP(inNumSamples,
  
complex<float> baseOsc = exp(complex<float>(0.0f, phaseCurrent));


/* Section 1 */
float loHarmonicsInt1;
float loHarmonicsFrac1;
float hiHarmonicsInt1;
float hiHarmonicsFrac1;
float signalR1;

if (sectionIsOn1 == 0) {
  signalR1 = 0.0f;
} else {
if (unit->loHarmonicsRate1 == 0) loHarmonics1 = ZXP(loHarmonicsIn1); 
if (unit->hiHarmonicsRate1 == 0) hiHarmonics1 = ZXP(hiHarmonicsIn1); 
if (unit->slopeRate1 == 0) slope1 = ZXP(slopeIn1); 
if (unit->evenOddRatioRate1 == 0) evenOddRatio1 = ZXP(evenOddRatioIn1); 

loHarmonicsInt1 = ceil(loHarmonics1);
loHarmonicsFrac1 = loHarmonicsInt1 - loHarmonics1;
if (loHarmonicsIntCurrent1 == loHarmonicsInt1) loHarmonicsFracCurrent1 = loHarmonicsFrac1;

hiHarmonicsInt1 = floor(hiHarmonics1);
hiHarmonicsFrac1 = hiHarmonics1 - hiHarmonicsInt1;
if (hiHarmonicsIntCurrent1 == hiHarmonicsInt1) hiHarmonicsFracCurrent1 = hiHarmonicsFrac1;

signalR1 = oneBlock_calc(baseOsc, phaseCurrent, slope1, evenOddRatio1, spreadCurrent1, spreadCompensation, loHarmonicsIntCurrent1, hiHarmonicsIntCurrent1, loHarmonicsFracCurrent1, hiHarmonicsFracCurrent1);
}

/* Section 2 */
float loHarmonicsInt2;
float loHarmonicsFrac2;
float hiHarmonicsInt2;
float hiHarmonicsFrac2;
float signalR2;

if (sectionIsOn2 == 0) {
  signalR2 = 0.0f;
} else {
if (unit->loHarmonicsRate2 == 0) loHarmonics2 = ZXP(loHarmonicsIn2); 
if (unit->hiHarmonicsRate2 == 0) hiHarmonics2 = ZXP(hiHarmonicsIn2); 
if (unit->slopeRate2 == 0) slope2 = ZXP(slopeIn2); 
if (unit->evenOddRatioRate2 == 0) evenOddRatio2 = ZXP(evenOddRatioIn2); 

loHarmonicsInt2 = ceil(loHarmonics2);
loHarmonicsFrac2 = loHarmonicsInt2 - loHarmonics2;
if (loHarmonicsIntCurrent2 == loHarmonicsInt2) loHarmonicsFracCurrent2 = loHarmonicsFrac2;

hiHarmonicsInt2 = floor(hiHarmonics2);
hiHarmonicsFrac2 = hiHarmonics2 - hiHarmonicsInt2;
if (hiHarmonicsIntCurrent2 == hiHarmonicsInt2) hiHarmonicsFracCurrent2 = hiHarmonicsFrac2;

signalR2 = oneBlock_calc(baseOsc, phaseCurrent, slope2, evenOddRatio2, spreadCurrent2, spreadCompensation, loHarmonicsIntCurrent2, hiHarmonicsIntCurrent2, loHarmonicsFracCurrent2, hiHarmonicsFracCurrent2);
}

/* Section 3 */
float loHarmonicsInt3;
float loHarmonicsFrac3;
float hiHarmonicsInt3;
float hiHarmonicsFrac3;
float signalR3;

if (sectionIsOn3 == 0) {
  signalR3 = 0.0f;
} else {
if (unit->loHarmonicsRate3 == 0) loHarmonics3 = ZXP(loHarmonicsIn3); 
if (unit->hiHarmonicsRate3 == 0) hiHarmonics3 = ZXP(hiHarmonicsIn3); 
if (unit->slopeRate3 == 0) slope3 = ZXP(slopeIn3); 
if (unit->evenOddRatioRate3 == 0) evenOddRatio3 = ZXP(evenOddRatioIn3); 

loHarmonicsInt3 = ceil(loHarmonics3);
loHarmonicsFrac3 = loHarmonicsInt3 - loHarmonics3;
if (loHarmonicsIntCurrent3 == loHarmonicsInt3) loHarmonicsFracCurrent3 = loHarmonicsFrac3;

hiHarmonicsInt3 = floor(hiHarmonics3);
hiHarmonicsFrac3 = hiHarmonics3 - hiHarmonicsInt3;
if (hiHarmonicsIntCurrent3 == hiHarmonicsInt3) hiHarmonicsFracCurrent3 = hiHarmonicsFrac3;

signalR3 = oneBlock_calc(baseOsc, phaseCurrent, slope3, evenOddRatio3, spreadCurrent3, spreadCompensation, loHarmonicsIntCurrent3, hiHarmonicsIntCurrent3, loHarmonicsFracCurrent3, hiHarmonicsFracCurrent3);
}

/* Section 4 */
float loHarmonicsInt4;
float loHarmonicsFrac4;
float hiHarmonicsInt4;
float hiHarmonicsFrac4;
float signalR4;

if (sectionIsOn4 == 0) {
  signalR4 = 0.0f;
} else {
if (unit->loHarmonicsRate4 == 0) loHarmonics4 = ZXP(loHarmonicsIn4); 
if (unit->hiHarmonicsRate4 == 0) hiHarmonics4 = ZXP(hiHarmonicsIn4); 
if (unit->slopeRate4 == 0) slope4 = ZXP(slopeIn4); 
if (unit->evenOddRatioRate4 == 0) evenOddRatio4 = ZXP(evenOddRatioIn4); 

loHarmonicsInt4 = ceil(loHarmonics4);
loHarmonicsFrac4 = loHarmonicsInt4 - loHarmonics4;
if (loHarmonicsIntCurrent4 == loHarmonicsInt4) loHarmonicsFracCurrent4 = loHarmonicsFrac4;

hiHarmonicsInt4 = floor(hiHarmonics4);
hiHarmonicsFrac4 = hiHarmonics4 - hiHarmonicsInt4;
if (hiHarmonicsIntCurrent4 == hiHarmonicsInt4) hiHarmonicsFracCurrent4 = hiHarmonicsFrac4;

signalR4 = oneBlock_calc(baseOsc, phaseCurrent, slope4, evenOddRatio4, spreadCurrent4, spreadCompensation, loHarmonicsIntCurrent4, hiHarmonicsIntCurrent4, loHarmonicsFracCurrent4, hiHarmonicsFracCurrent4);
}

if (unit->sectionAmpRate1 == 0) sectionAmp1 = ZXP(sectionAmpIn1); 
if (unit->sectionAmpRate2 == 0) sectionAmp2 = ZXP(sectionAmpIn2); 
if (unit->sectionAmpRate3 == 0) sectionAmp3 = ZXP(sectionAmpIn3); 
if (unit->sectionAmpRate4 == 0) sectionAmp4 = ZXP(sectionAmpIn4); 

float activeSectionAmpSum = 0.0f;
if (sectionIsOn1 == 1) activeSectionAmpSum += sectionAmp1;
if (sectionIsOn2 == 1) activeSectionAmpSum += sectionAmp2;
if (sectionIsOn3 == 1) activeSectionAmpSum += sectionAmp3;
if (sectionIsOn4 == 1) activeSectionAmpSum += sectionAmp4;

float signalRtotal = 
(
(signalR1 * sectionAmp1) + 
(signalR2 * sectionAmp2) + 
(signalR3 * sectionAmp3) + 
(signalR4 * sectionAmp4)
)
/activeSectionAmpSum
;

float omega;
if (unit->freqRate == 0) freq = ZXP(freqIn);
omega = freq * unit->partialOmega;
// phase increment per sample at given frequency

  phaseCurrent += omega;
  while (phaseCurrent >= twopi_f){
    phaseCurrent -= twopi_f;

   /* Section 1 */
    if (loHarmonicsInt1 != loHarmonicsIntCurrent1) loHarmonicsIntCurrent1 = loHarmonicsInt1;
    // Change loHarmonicsInt value when phase crosses 2pi 

    if (hiHarmonicsInt1 != hiHarmonicsIntCurrent1) hiHarmonicsIntCurrent1 = hiHarmonicsInt1;
    // Change hiHarmonicsInt value when phase crosses 2pi 

    if (static_cast<float>(spreadIn1) != spreadCurrent1) spreadCurrent1 = static_cast<float>(spreadIn1);
    // Change spread value when phase crosses 2pi

  // Do same things for other sections
   /* Section 2 */
    if (loHarmonicsInt2 != loHarmonicsIntCurrent2) loHarmonicsIntCurrent2 = loHarmonicsInt2;
    if (hiHarmonicsInt2 != hiHarmonicsIntCurrent2) hiHarmonicsIntCurrent2 = hiHarmonicsInt2;
    if (static_cast<float>(spreadIn2) != spreadCurrent2) spreadCurrent2 = static_cast<float>(spreadIn2);

   /* Section 3 */
    if (loHarmonicsInt3 != loHarmonicsIntCurrent3) loHarmonicsIntCurrent3 = loHarmonicsInt3;
    if (hiHarmonicsInt3 != hiHarmonicsIntCurrent3) hiHarmonicsIntCurrent3 = hiHarmonicsInt3;
    if (static_cast<float>(spreadIn3) != spreadCurrent3) spreadCurrent3 = static_cast<float>(spreadIn3);
 
   /* Section 4 */
    if (loHarmonicsInt4 != loHarmonicsIntCurrent4) loHarmonicsIntCurrent4 = loHarmonicsInt4;
    if (hiHarmonicsInt4 != hiHarmonicsIntCurrent4) hiHarmonicsIntCurrent4 = hiHarmonicsInt4;
    if (static_cast<float>(spreadIn4) != spreadCurrent4) spreadCurrent4 = static_cast<float>(spreadIn4);    
}
  ZXP(out) = signalRtotal;
  )

unit->phaseCurrent = phaseCurrent;

unit->loHarmonicsIntCurrent1 = loHarmonicsIntCurrent1;
unit->hiHarmonicsIntCurrent1 = hiHarmonicsIntCurrent1;
unit->loHarmonicsFracCurrent1 = loHarmonicsFracCurrent1;
unit->hiHarmonicsFracCurrent1 = hiHarmonicsFracCurrent1;
unit->spreadCurrent1 = spreadCurrent1;

unit->loHarmonicsIntCurrent2 = loHarmonicsIntCurrent2;
unit->hiHarmonicsIntCurrent2 = hiHarmonicsIntCurrent2;
unit->loHarmonicsFracCurrent2 = loHarmonicsFracCurrent2;
unit->hiHarmonicsFracCurrent2 = hiHarmonicsFracCurrent2;
unit->spreadCurrent2 = spreadCurrent2;

unit->loHarmonicsIntCurrent3 = loHarmonicsIntCurrent3;
unit->hiHarmonicsIntCurrent3 = hiHarmonicsIntCurrent3;
unit->loHarmonicsFracCurrent3 = loHarmonicsFracCurrent3;
unit->hiHarmonicsFracCurrent3 = hiHarmonicsFracCurrent3;
unit->spreadCurrent3 = spreadCurrent3;

unit->loHarmonicsIntCurrent4 = loHarmonicsIntCurrent4;
unit->hiHarmonicsIntCurrent4 = hiHarmonicsIntCurrent4;
unit->loHarmonicsFracCurrent4 = loHarmonicsFracCurrent4;
unit->hiHarmonicsFracCurrent4 = hiHarmonicsFracCurrent4;
unit->spreadCurrent4 = spreadCurrent4;
}

////////////////////////////////////////////////////////////////////

// LOAD FUNCTION

PluginLoad(BLOsc4sections)
{
  ft = inTable;

  DefineSimpleUnit(BLOsc4sections);
}

////////////////////////////////////////////////////////////////////

