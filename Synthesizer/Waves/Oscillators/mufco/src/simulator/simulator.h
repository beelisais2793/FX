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
/* MuFCO SIMULATOR HEADER					*/
/****************************************************************/
#ifndef SIMULATOR_H_
#define SIMULATOR_H_

class CSimulator;

/****************************************************************/
/* LIBRARIES */
#include "common.h"
#include "plotter.h"
#include "environment.h"
#include "oscillator.h"

class CSimulator {
	public:
	CSimulator                 ( string );
	~CSimulator                ( void );
	void     restart           ( void );
	void     executeSimulation ( void );
	
	void     setKuramoto       ( float );
	void     setSwitchPr       ( float );
	void     setAvoidNoise     ( float );
	void     setSeed           ( int seed    ) {m_nSeed = seed;};
	void     setPopulation     ( int input   ) {m_nPopulation = input;};
	void     setAmplitude      ( float input ) {m_pcEnvironment->setAmplitude( input );};
	
	SFFTst   getFFTst          ( void    ) { return m_sFFTst;};
	float    getFFTamp         ( int cmp ) { return (*(m_pcEnvironment->getGFFTamp()))[cmp]; };
	float    getR              ( int cmp ) { return m_vR[cmp]; };

	TVFloat  getEvaluation     ( void );

	
	private:
	/* Simulator stuff */
	CEnvironment* m_pcEnvironment;	
	TVCOscillator m_vpcOscillators;	
	CRandom*      m_pcRandom;
	int           m_nSeed;
	int   	      m_nSimStep;
	int           m_nFFTsize;
	int           m_nFFTst_hist;
	int           m_nSampling;
	int           m_nStepLimit;	
	int           m_nPopulation;
	string 	      m_sParamsFile;
	/* Status */
	TVFloat  m_vOscillatorDist;
	TVFloat  m_vFFTTheta;
	SFFTst   m_sFFTst;
	int      m_nF;
	float    m_fN;	
	TVFloat  m_vPhi;
	TVFloat  m_vR;
	TVFloat  m_vTheta;
	/* Writer */
	bool    m_bWriter;
	int     m_nWCmp;
	string  m_sOutputFolder;	
	/* Visualization */
	CPlotter*  m_pcPlotter;
	bool       m_bVisu;
	int        m_nWTimeLength;
	float      m_fYTscale;
	float      m_fYFscale;
	/* FUNCTION */
	void _writer         ( void );
	void _writerInit     ( void );
	void _configureVisu  ( XMLElement* );
	void _calculateMF    ( void );
	void _calculateFFTst ( void );
};
#endif
