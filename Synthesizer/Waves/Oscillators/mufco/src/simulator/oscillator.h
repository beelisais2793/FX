/*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*    Copyright (C) 2013  Manuel Castillo Cagigal
*
*    Authors: Manuel Castillo Cagigal <manuel.castillo@upm.es>
*             Eduardo Matallanas de Avila <eduardo.matallanas@upm.es>
*	      Alvaro Gutierrez Martin <aguti@etsit.upm.es>
*
*    Developed in Robolabo <http://www.robolabo.etsit.upm.es/>
*/

/****************************************************************/
/* OSCILLATOR HEADER						*/
/****************************************************************/
#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

class COscillator;

/****************************************************************/
/* LIBRARIES */
#include "common.h"

typedef vector<COscillator*> TVCOscillator;

class COscillator {
	public:
	COscillator  ( string* , int ); 
	~COscillator ( void );

	void  reset       ( void );
	void  elementStep ( void );
	
	float getPower    ( void ){return m_fPower;};
	float getArgument ( void ){return m_fArgument;};
	float getPhase    ( void ){return m_fPhase;}
	int   getWorkPer  ( void ){return m_fPeriod;};	
	int   getCmp      ( void ){return m_nComp;};

	void  sendFFT         ( TVFreqCmp* , SFFTst* );
	void  sendMFArguments ( float R    , float P , float A ) { m_R = R; m_Phi = P; m_A = A;};
	
	void  setKuramoto     ( float input )           { m_fK = input; };
	void  setSwitchPr     ( float input )           { m_fP = input; };
	void  setAvoidNoise   ( float input )           { m_fN = input; };

	void  setRandom       ( CRandom* input )        { m_pcRandom = input; };	
	void  setClock        ( int* input )            { m_nSimStep = input; };
	void  setSimSetUp     ( int i1, int i2, int i3) { m_nSampling = i1; m_nFFTsize = i2; m_nPop = i3;};	
	
	private:
	/* Simulator Stuff */	
	CRandom* m_pcRandom;	
	int*     m_nSimStep;	
	int      m_nID;
	int      m_nKType;
	int      m_nPType;
	int      m_nPop;
	int      m_nSampling;
	int      m_nFFTsize;
	int      m_nIniDist;
	float    m_fOffset;
	/* External Information */
	TVFreqCmp* m_vFFT;
	SFFTst*    m_sFFTst;
	TVFloat*   m_vArgs;
	float      m_R;
	float      m_Phi;
	float      m_A;
	/* Output */
	float      m_fPower;	
	/* Kuramoto parameter */
	float m_fK;	
	float m_fP;	
	float m_fN;
	/* State Variables */
	float m_fThr;	
	bool  m_bNew;	
	int   m_nComp;
	float m_fPeriod;
	float m_fOmega;
	float m_fArgument;
 	float m_fPhase;
	bool  m_bNewFFT;	
	/* FUNCTIONS */	
	int   _roulette      ( TVFloat* );
};
#endif
