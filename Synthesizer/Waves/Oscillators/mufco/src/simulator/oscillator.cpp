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
/* OSCILLATOR METHODS						*/
/****************************************************************/
#include "oscillator.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
COscillator::COscillator ( string* sParamsFile , int nID ){ 
	m_nID       = nID;
	// Configuration from XML
	XMLDocument conf;
    	conf.LoadFile( sParamsFile->c_str() );
	XMLElement* root = conf.FirstChildElement();
	string elemName , attr;	
	for( XMLElement* elem = root->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "Oscilators" ){
			attr       = elem->Attribute("distribution");
			m_nIniDist = atoi( attr.c_str() );
			attr       = elem->Attribute("N");
			m_fN       = atof( attr.c_str() );
			attr       = elem->Attribute("offset");
			m_fOffset  = atof( attr.c_str() );
			for( XMLElement* e = elem->FirstChildElement() ; e != NULL ; e = e->NextSiblingElement() ){
				elemName = e->Value();
				if ( elemName == "coupling" ){
					attr     = e->Attribute("type");
					m_nKType = atoi( attr.c_str() );
					attr     = e->Attribute("K");
					m_fK     = atof( attr.c_str() );
				}
				else if ( elemName == "switching" ){
					attr     = e->Attribute("type");
					m_nPType = atoi( attr.c_str() );
					attr     = e->Attribute("P");
					m_fP     = atof( attr.c_str() );										
				}
			}							
		}
	}
	return;
};
/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
COscillator::~COscillator ( void ){	
	return;
};

/****************************************************************/
void COscillator::reset( void ){
	// Reset initial variables
	m_fPhase  = m_pcRandom->nextDouble( - M_PI , M_PI );	
	if ( m_nIniDist < 0 )
		m_nComp = m_pcRandom->nextInt( 1 , m_nFFTsize/2 );
	else	
		m_nComp = m_nIniDist;
	m_fPeriod = float ( m_nSampling ) * float( m_nFFTsize  )/float(m_nComp);
	m_fOmega  = TWO_PI / m_fPeriod;		
	m_fPower  = 0.0;	
	m_bNewFFT = false;
	m_fThr    = 0.0;	
	m_bNew    = true;	
	return;
};

/****************************************************************/
/* MAIN FUNCTION						*/
/****************************************************************/
void COscillator::elementStep ( void ){
	if ( m_bNewFFT ){
		// Component Switching
		if ( m_nPType == 1 ){				
			if ( m_pcRandom->nextDouble() < m_fP * m_sFFTst->FFTff ){			
				TVFloat tmp_vfloat;
				for ( int i = 1 ; i < m_vFFT->size() ; i++ ){						
					tmp_vfloat.push_back( pow ( m_vFFT->at(i).amp , 2 ) );
				}				
				int new_cmp = _roulette ( &tmp_vfloat ) + 1; // 0 is taken into account
				if ( m_nComp != new_cmp ){				
					m_nComp     = new_cmp;										
					m_fPeriod   = float ( m_nSampling ) * m_vFFT->at(m_nComp).period;
					m_fOmega    = TWO_PI / m_fPeriod;							
					m_fPhase    = m_pcRandom->nextDouble( - M_PI , M_PI );	
					m_fArgument = TWO_PI * float ( *m_nSimStep - 1 ) / m_fPeriod + m_fPhase ;
				}				
			}
		}
		else if ( m_nPType == 2 ){
			if ( m_pcRandom->nextDouble() < m_fP * m_sFFTst->FFTff ){			
				TVFloat tmp_vfloat;
				for ( int i = 1 ; i < m_vFFT->size() ; i++ ){						
					tmp_vfloat.push_back( pow ( m_vFFT->at(i).amp , 2 ) * exp( - m_fN * m_sFFTst->HRel[i] ) );
				}				
				int new_cmp = _roulette ( &tmp_vfloat ) + 1; // 0 is taken into account
				if ( m_nComp != new_cmp ){				
					m_nComp     = new_cmp;										
					m_fPeriod   = float ( m_nSampling ) * m_vFFT->at(m_nComp).period;
					m_fOmega    = TWO_PI / m_fPeriod;							
					m_fPhase    = m_pcRandom->nextDouble( - M_PI , M_PI );	
					m_fArgument = TWO_PI * float ( *m_nSimStep - 1 ) / m_fPeriod + m_fPhase ;
				}
			}										
		}
		else if ( m_nPType == 3 ){
			if ( m_bNew ){
				m_fThr = m_vFFT->at(m_nComp).amp;				
				m_bNew = false;
			}						
			float prob  = 1.0 / ( 1.0 + exp ( -0.2 * ( m_vFFT->at(m_nComp).amp - m_fThr ) ) );
			float alpha = (1.0 + 1.0 * sqrt( m_sFFTst->HVar[m_nComp] ));
			float inc   = alpha * exp( - fabs( ( m_vFFT->at(m_nComp).amp - m_fThr )/ alpha ) );
			if ( m_pcRandom->nextDouble() < prob ){	// WORK	- LEARNING
				m_fThr += - inc * ( 1.0 - 2.0 * exp ( - m_vFFT->at(m_nComp).amp ) );
			}
			else{
				TVFloat tmp_vfloat;
				for ( int i = 1 ; i < m_vFFT->size() ; i++ ){						
					tmp_vfloat.push_back( pow ( m_vFFT->at(i).amp , 2 ) );
				}				
				int new_cmp = _roulette ( &tmp_vfloat ) + 1; // 0 is taken into account
				if ( m_nComp != new_cmp ){				
					m_nComp     = new_cmp;										
					m_fPeriod   = float ( m_nSampling ) * m_vFFT->at(m_nComp).period;
					m_fOmega    = TWO_PI / m_fPeriod;							
					m_fPhase    = m_pcRandom->nextDouble( - M_PI , M_PI );	
					m_fArgument = TWO_PI * float ( *m_nSimStep - 1 ) / m_fPeriod + m_fPhase ;				
				}
			}				
		}
		// Kuramoto Coupling
		if ( m_nComp != 0 ){
			float ph_mod = 0;				
			switch ( m_nKType ){
				case 1: 
					ph_mod = m_fK * m_R * sin ( m_Phi - m_fArgument );					
					break;
				case 2:
					ph_mod = m_fK * ( m_vFFT->at(m_nComp).amp / ( m_nPop + m_A )  ) * sin ( m_vFFT->at(m_nComp).phs - m_fArgument );
					break;
				case 3:
					ph_mod = m_fK * ( m_vFFT->at(m_nComp).amp / m_sFFTst->FFTmax  ) * sin ( m_vFFT->at(m_nComp).phs - m_fArgument ); 
					break;
				case 4:
					ph_mod = m_fK * ( m_vFFT->at(m_nComp).amp / m_vFFT->at(0).amp ) * sin ( m_vFFT->at(m_nComp).phs - m_fArgument ); 
					break;
				default:
					break;	
			}
			m_fPhase += ph_mod;
			if ( m_fPhase > M_PI )
				m_fPhase -= TWO_PI;
			else if ( m_fPhase < - M_PI )
				m_fPhase += TWO_PI;
		}				
		m_bNewFFT = false;
	}
	/* Oscilator */
	m_fArgument = m_fOmega * float ( *m_nSimStep ) + m_fPhase ;
	m_fPower    = m_fOffset + cos ( m_fArgument );	
	return;
};

/******************************************************************************/
void COscillator::sendFFT ( TVFreqCmp* input , SFFTst* input2 ) { 
	m_vFFT    = input; 
	m_bNewFFT = true; 
	m_sFFTst  = input2;
	return;
};

/******************************************************************************/
int COscillator::_roulette ( TVFloat* prob ){	
	TVFloat roulette;
	roulette.push_back( (*prob)[0] );
	for ( int i = 1 ; i < prob->size() ; i++ ){
		roulette.push_back( (*prob)[i] +  roulette.back() );
	}	
	float rand = m_pcRandom->nextDouble( roulette.back() );	
	bool  Exit = false;
	int   cnt  = 0;
	while (!Exit){
		if ( roulette[cnt] > rand ){			
			Exit   = true;
		}
		else{
			cnt++;
		}
	}
	return cnt;
};


