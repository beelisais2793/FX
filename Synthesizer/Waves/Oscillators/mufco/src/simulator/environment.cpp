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
/* ENVIRONMENT METHODS						*/
/****************************************************************/
#include "environment.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
CEnvironment::CEnvironment ( XMLElement* conf ){
	// Configuration	
	string elemName, attr;
	attr         = conf->Attribute("type");
	m_nEnvType   = atoi( attr.c_str() );
	attr         = conf->Attribute("amplitude");
	m_fAmplitude = atof( attr.c_str() );
	attr         = conf->Attribute("offset");
	m_fOffset    = atof( attr.c_str() );
	string        tmp_data;	
	ostringstream tmp_str;			
	tmp_str       << m_nEnvType;
	tmp_data.assign ( "src_" );		
	tmp_data.append ( tmp_str.str() );
	for( XMLElement* e = conf->FirstChildElement() ; e != NULL ; e = e->NextSiblingElement() ){
		elemName = e->Value();
		if ( elemName == tmp_data ){
			m_sSourceFile = e->Attribute("file");
		}
	}
	// FFT object
	m_pcFFT    = new CFFT();
	return;
};

/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
CEnvironment::~CEnvironment ( void ){
	delete m_pcFFT;
	return;
};

/****************************************************************/
void CEnvironment::reset ( void ){
	// Clean historics and state
	m_vCSignal.clear();	
	m_vCSampled.clear();
	m_vCFFT.clear();
	m_vCFFTamp.clear();

	m_vNCSignal.clear();	
	m_vNCSampled.clear();
	m_vNCFFT.clear();
	m_vNCFFTamp.clear();

	m_vGSignal.clear();	
	m_vGSampled.clear();
	m_vGFFT.clear();
	m_vGFFTamp.clear();	

	m_vInputSignal.clear();
	m_vInputProfile.clear();

	m_vNCArg.clear();
	m_vNCAmp.clear();

	// Initialize variables
	m_bNewFFT = false;
	// Load Profiles
	_initializeNC ( );
	switch( m_nEnvType ){		
		case 1:
			_configureSinusoidal ( ); 
			break;
		case 2:
			_configureProfile    ( );
			break;
		default:			
			break;
	}	
	return;
};

/****************************************************************/
/* MAIN FUNCTION						*/
/****************************************************************/
void CEnvironment::environmentStep ( void ){
	/* Not Controllable */
	float nc_power;
	switch( m_nEnvType ){		
		case 1:
			nc_power = _nextInputSinusoidal( );
			break;
		case 2:
			nc_power  = _nextInputProfile( );			
			break;
		default:
			nc_power  = 0.0;			
			break;
	}	
	m_vNCSignal.push_back( nc_power );	
	/* Controllable elements */
	float c_power = 0.0;
	for ( int i = 0 ; i < m_vpcOscillators->size() ; i++ )
		c_power += (*m_vpcOscillators)[i]->getPower(); 
	m_vCSignal.push_back( c_power );	
	/* Global signal - Filtering */
	m_vGSignal.push_back( nc_power + c_power );	
	if ( (*m_nSimStep)%m_nSampling == 0 ){
		/* Sampling */		
		m_vGSampled.push_back ( m_vGSignal.back() );
		m_vCSampled.push_back      ( m_vCSignal.back()    );
		m_vNCSampled.push_back     ( m_vNCSignal.back()   );
		/* FFT */	
		_signalFFT ( &m_vGSampled  , &m_vGFFT  , &m_vGFFTamp  );		
		_signalFFT ( &m_vCSampled  , &m_vCFFT  , &m_vCFFTamp  );
		_signalFFT ( &m_vNCSampled , &m_vNCFFT , &m_vNCFFTamp );
		if ( m_nEnvType == 2 )
			_defineNCfromFFT ( );
	}
	else{
		m_bNewFFT = false;
	}
	return;
};

/****************************************************************/
void CEnvironment::_signalFFT ( TVFloat* input , TVFreqCmp* fft_out , TVFloat* fft_amp_out ){	
	fft_out->clear();
	fft_amp_out->clear();
	SFreqCmp    tmp_freqcmp;
	/* There are enough samples */
	if ( input->size() >= m_nFFTsize ){				
		double time_domain  [m_nFFTsize][2];
		double frec_domain  [m_nFFTsize][2];							
		for ( int j = 0 ; j < m_nFFTsize ; j++ ){
			time_domain[j][0] = (*input)[ input->size() - j - 1 ]; // Realign time domain values
			time_domain[j][1] = 0.0;				
		}						
		m_pcFFT->fft ( m_nFFTsize, &time_domain[0], &frec_domain[0] );							
		for (int j = 0 ; j < m_nhalfFFTsize + 1 ; j++){
			complex<double> tmp_complex (frec_domain[j][0],frec_domain[j][1]);			
			if ( j == 0 ){
				tmp_freqcmp.amp      = abs(tmp_complex)/float(m_nFFTsize);
				tmp_freqcmp.period   = 0.0;
				tmp_freqcmp.phs      = 0.0;
			}
			else{
				tmp_freqcmp.amp      =   2.0 * abs(tmp_complex)/float(m_nFFTsize);	
				tmp_freqcmp.period   =   float( m_nFFTsize  )/float(j);
				tmp_freqcmp.phs      = - arg  ( tmp_complex );						
			}
			fft_out->push_back     ( tmp_freqcmp );	
			fft_amp_out->push_back ( tmp_freqcmp.amp );							
		}
		m_bNewFFT = true;			
	}
	/* There are not enough samples  */
	else{			
		tmp_freqcmp.amp      = 0.0;
		tmp_freqcmp.period   = 0.0;
		tmp_freqcmp.phs      = 0.0;
		for (int j = 0 ; j < m_nhalfFFTsize + 1 ; j++){
			fft_out->push_back     ( tmp_freqcmp );	
			fft_amp_out->push_back ( tmp_freqcmp.amp );
		}
		m_bNewFFT = false;
	}	
	return;
};

/****************************************************************/
float CEnvironment::_nextInputProfile ( void ){
	float  result = m_vInputProfile[ (*m_nSimStep)%m_vInputProfile.size() ];
	return result;
};

/****************************************************************/
float CEnvironment::_nextInputSinusoidal( void ){
	float result    = 0.0;		
	for ( int i = 0 ; i < m_nFFTsize ; i++ ){
		if ( m_vInputSignal[i].amp > 0.0 ){
			if ( i != 0 ){
				m_vNCArg[i]   = ( TWO_PI * float(*m_nSimStep)/ ( float ( m_nSampling ) * m_vInputSignal[i].period ) + m_vInputSignal[i].phs );			
				result       += m_vInputSignal[i].amp * ( m_fOffset + cos( m_vNCArg[i] ) );
			}
			else{
				m_vNCArg[i]   = 0.0;
				result       += m_vInputSignal[i].amp;
			}
		}		
	}	
	return result;
};

/****************************************************************/
void CEnvironment::_defineNCfromFFT ( void ){
	for ( int i = 0 ; i < m_nFFTsize ; i++ ){
		m_vNCArg[i] = m_vNCFFT[i].phs;
		m_vNCAmp[i] = m_vNCFFT[i].amp;
	}
	return;
};

/****************************************************************/
void CEnvironment::_initializeNC ( void ){
	/* Initialize components */	
	SFreqCmp tmp_FC;	
	for ( int i = 0 ; i < m_nFFTsize ; i++ ){
		tmp_FC.amp    = 0.0;
		tmp_FC.phs    = 0.0;
		if ( i != 0 )
			tmp_FC.period = float( m_nFFTsize )/float( i );		
		else
			tmp_FC.period = 0.0;
		m_vInputSignal.push_back( tmp_FC );
		m_vNCArg.push_back(0.0);
		m_vNCAmp.push_back(0.0);
	}
	return;
};

/****************************************************************/
void CEnvironment::_configureProfile    ( void ){
	/* Read profile from file */
	ifstream InputFile    ( m_sSourceFile.c_str() );
	m_vInputProfile.clear ( );
	float tmp_float, tmp_float_old, slope;
	InputFile.ignore   ( 256, ' ' );
	InputFile.ignore   ( 256, ' ' );
	InputFile.ignore   ( 256, ' ' );
	InputFile.ignore   ( 256, ' ' );
	InputFile.ignore   ( 256, ' ' );
	InputFile >> tmp_float_old;
	tmp_float_old *= m_fAmplitude;	
	InputFile.ignore   ( 256, '\n' );
	while ( !InputFile.eof() ){
		InputFile.ignore   ( 256, ' ' );
		InputFile.ignore   ( 256, ' ' );
		InputFile.ignore   ( 256, ' ' );
		InputFile.ignore   ( 256, ' ' );
		InputFile.ignore   ( 256, ' ' );
		InputFile >> tmp_float;
		tmp_float *= m_fAmplitude;
		slope = (tmp_float - tmp_float_old)/60.0;
		for ( int j = 0 ; j  < 60 ; j++ ){
			m_vInputProfile.push_back( tmp_float_old + slope * j );
		}
		InputFile.ignore   ( 256, '\n' );
		tmp_float_old = tmp_float;
	}
	InputFile.close();
	return;
};

/****************************************************************/
void CEnvironment::_configureSinusoidal ( void ){ 	
	/* Read components from XML file */
	XMLDocument conf;
    	conf.LoadFile( m_sSourceFile.c_str() );
	XMLElement* root = conf.FirstChildElement();
	string   elemName, attr;
	SFreqCmp tmp_FC;
	for( XMLElement* elem = root->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "Cmp" ){
			attr          = elem->Attribute("period");
			tmp_FC.period = atof( attr.c_str() ) / float ( m_nSampling );
			attr          = elem->Attribute("phase");
			tmp_FC.phs    = atof( attr.c_str() );
			attr          = elem->Attribute("amplitude");
			tmp_FC.amp    = m_fAmplitude * atof( attr.c_str() );
			if ( tmp_FC.period > 0.0 ){
				m_vInputSignal [ int ( float( m_nFFTsize  ) / tmp_FC.period ) ] = tmp_FC;
				m_vNCAmp       [ int ( float( m_nFFTsize  ) / tmp_FC.period ) ] = tmp_FC.amp;
			}
			else{
				m_vInputSignal [ 0 ] = tmp_FC;
				m_vNCAmp       [ 0 ] = tmp_FC.amp;

			}
		}
	}
	return;
};


