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
/* ENVIRONMENT HEADER						*/
/****************************************************************/
#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

class CEnvironment;

/****************************************************************/
/* LIBRARIES */
#include "common.h"
#include "oscillator.h"

class CEnvironment {
	public:
	CEnvironment  ( XMLElement* );
	~CEnvironment ( void   );

	void environmentStep ( void );
	void reset ( void );

	void setRandom      ( CRandom* input )        { m_pcRandom       = input; };	
	void setClock       ( int* input  )           { m_nSimStep       = input; };
	void setAmplitude   ( float input )           { m_fAmplitude     = input; };
	void setSimSetUp    ( int i1, int i2, int i3) { m_nSampling      = i1; m_nFFTsize = i2; m_nPop = i3; m_nhalfFFTsize = int( m_nFFTsize / 2 );};	
	void setOscillators ( TVCOscillator* input )  { m_vpcOscillators = input; };

	bool       isFFT           ( void ){return m_bNewFFT;};
	
	TVFloat*   getCSignal      ( void ){return &m_vCSignal;};
	TVFloat*   getCSampled     ( void ){return &m_vCSampled;};
	TVFreqCmp* getCFFT         ( void ){return &m_vCFFT;};
	TVFloat*   getCFFTamp      ( void ){return &m_vCFFTamp;};

	TVFloat*   getNCSignal     ( void ){return &m_vNCSignal;};
	TVFloat*   getNCSampled    ( void ){return &m_vNCSampled;};
	TVFreqCmp* getNCFFT        ( void ){return &m_vNCFFT;};
	TVFloat*   getNCFFTamp     ( void ){return &m_vNCFFTamp;};

	TVFloat*   getGSignal      ( void ){return &m_vGSignal;};
	TVFloat*   getGSampled     ( void ){return &m_vGSampled;};
	TVFreqCmp* getGFFT         ( void ){return &m_vGFFT;};
	TVFloat*   getGFFTamp      ( void ){return &m_vGFFTamp;};	

	TVFloat*   getNCSignalArg  ( void ){return &m_vNCArg;};
	TVFloat*   getNCSignalAmp  ( void ){return &m_vNCAmp;};		
	
	
	private:
	/* Simulation Stuff */
	CRandom*       m_pcRandom;
	TVCOscillator* m_vpcOscillators;
	CFFT*          m_pcFFT;
	int            m_nEnvType;
	int            m_nSampling;
	int            m_nPop;
	int            m_nFFTsize;
	int            m_nhalfFFTsize;
	int*           m_nSimStep;	
	bool           m_bNewFFT;
	float          m_fAmplitude;
	float          m_fOffset;		
	string         m_sSourceFile;	

	/* Historics and State */	
	TVFloat   m_vCSignal;	
	TVFloat   m_vCSampled;
	TVFreqCmp m_vCFFT;
	TVFloat   m_vCFFTamp;

	TVFloat   m_vNCSignal;	
	TVFloat   m_vNCSampled;
	TVFreqCmp m_vNCFFT;
	TVFloat   m_vNCFFTamp;

	TVFloat   m_vGSignal;	
	TVFloat   m_vGSampled;
	TVFreqCmp m_vGFFT;
	TVFloat   m_vGFFTamp;	

	TVFreqCmp   m_vInputSignal;
	TVFloat     m_vInputProfile;

	TVFloat     m_vNCArg;
	TVFloat	    m_vNCAmp;		

	/* FUNCTIONS */
	float _nextInputProfile    ( void );
	float _nextInputSinusoidal ( void );
	void  _configureSinusoidal ( void );
	void  _configureProfile    ( void );
	void  _defineNCfromFFT     ( void );
	void  _initializeNC        ( void );
	void  _signalFFT           ( TVFloat* , TVFreqCmp* , TVFloat* );
};
#endif
