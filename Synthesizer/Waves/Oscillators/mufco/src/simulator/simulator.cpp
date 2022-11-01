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
/* MuFCO SIMULATOR METHODS					*/
/****************************************************************/
#include "simulator.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
CSimulator::CSimulator ( string pcParamsFile ){
	/* Get configuration File */
	m_sParamsFile = pcParamsFile;

	/* Initialize the objects */
	m_pcRandom      = NULL;
	m_pcEnvironment = NULL;
	m_pcPlotter     = NULL;

	/* Configure from XML file */
	XMLDocument conf;
    	conf.LoadFile( m_sParamsFile.c_str() );
	XMLElement* root = conf.FirstChildElement();
	string elemName , attr;	
	for( XMLElement* elem = root->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "Simulation" ){
			attr         = elem->Attribute("seed");
			m_nSeed      = atoi( attr.c_str() );
			attr         = elem->Attribute("length");
			m_nStepLimit = atoi( attr.c_str() );
			attr         = elem->Attribute("sampling");
			m_nSampling  = atoi( attr.c_str() );
			attr         = elem->Attribute("fft_lng");
			m_nFFTsize   = atoi( attr.c_str() );
		}
		else if ( elemName == "Writer" ){
			attr      = elem->Attribute("active");
			m_bWriter = bool (atoi( attr.c_str() ));
			if ( m_bWriter ){
				for( XMLElement* e = elem->FirstChildElement() ; e != NULL ; e = e->NextSiblingElement() ){
					elemName = e->Value();
					if ( elemName == "components" ){
						attr    = e->Attribute("number");
						m_nWCmp = atoi( attr.c_str() );
					}
					else if ( elemName == "output" )
						m_sOutputFolder = e->Attribute("folder");				
				}
			}	
		}
		else if ( elemName == "Visualization" ){
			attr    = elem->Attribute("active");
			m_bVisu = bool (atoi( attr.c_str() ));
			if ( m_bVisu ){
				_configureVisu ( elem );
			}
		}
		else if ( elemName == "Oscilators" ){
			attr            = elem->Attribute("number");
			m_nPopulation   = atoi( attr.c_str() );
			attr            = elem->Attribute("ff");
			m_nF            = atoi( attr.c_str() );	
			attr            = elem->Attribute("N");
			m_fN            = atof( attr.c_str() );	
			attr            = elem->Attribute("hist");
			m_nFFTst_hist   = atoi( attr.c_str() );			
		}
		else if ( elemName == "Environment" ){
			m_pcEnvironment   = new CEnvironment ( elem );			
			m_pcEnvironment->setClock       ( &m_nSimStep );
			m_pcEnvironment->setSimSetUp    ( m_nSampling , m_nFFTsize , m_nPopulation );
			m_pcEnvironment->setOscillators ( &m_vpcOscillators );
		}

	}
	conf.Clear();	
	restart();
	return;
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CSimulator::restart ( void ){
	/* Variables initial values */
	m_nSimStep       = 0;
	m_sFFTst.FFTave = 0.0;
	m_sFFTst.FFTvar = 0.0;	
	m_sFFTst.FFTmax = 0.0;
	m_sFFTst.FFTff  = 0.0;
	m_sFFTst.HAve.clear();
	for ( int i = 0 ; i < m_nFFTsize/2 + 1 ; i++ )
		m_sFFTst.HAve.push_back( 0.0 );
	m_sFFTst.HVar.clear();
	for ( int i = 0 ; i < m_nFFTsize/2 + 1 ; i++ )
		m_sFFTst.HVar.push_back( 0.0 );
	m_sFFTst.HRel.clear();
	for ( int i = 0 ; i < m_nFFTsize/2 + 1 ; i++ )
		m_sFFTst.HRel.push_back( 0.0 );
	m_sFFTst.HAmp.clear();
	TVFloat tmp_vf;
	for ( int i = 0 ; i <  m_nFFTsize/2 + 1 ; i++ )
		m_sFFTst.HAmp.push_back(tmp_vf);
	/* Random */	
	if ( m_pcRandom )
		delete m_pcRandom;	
	if ( m_nSeed < 0 )	
		m_pcRandom = new CRandom ();
	else
		m_pcRandom = new CRandom ( m_nSeed );
	/* Oscilators */
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ ){
		delete m_vpcOscillators[i];
	}
	m_vpcOscillators.clear();
	COscillator* tmp_oscillator;
	for ( int i = 0 ; i < m_nPopulation ; i++ ){
		tmp_oscillator = new COscillator ( &m_sParamsFile , i );				
		tmp_oscillator->setClock         ( &m_nSimStep );
		tmp_oscillator->setSimSetUp      ( m_nSampling , m_nFFTsize , m_nPopulation );
		tmp_oscillator->setRandom        ( m_pcRandom );	
		tmp_oscillator->reset            ( );
		m_vpcOscillators.push_back       ( tmp_oscillator );
	}
	m_vOscillatorDist.clear();
	for ( int i = 0 ; i < m_nFFTsize/2 ; i++ ){
		m_vOscillatorDist.push_back(0);
	}
	/* Environment */
	m_pcEnvironment->setRandom ( m_pcRandom );
	m_pcEnvironment->reset();
	/* Writer */
	if ( m_bWriter )
		_writerInit();
	return;
};

/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
CSimulator::~CSimulator ( void ){
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ )
		delete m_vpcOscillators[i];	
	delete m_pcEnvironment;
	delete m_pcRandom;
	if ( m_bVisu  )
		delete m_pcPlotter;
	return;
};

/****************************************************************/
/* MAIN FUNCTION						*/
/****************************************************************/
void CSimulator::executeSimulation ( void ){	
	bool     SimFinished = false;
	while(!SimFinished){
		/* ELEMENTS */			
		for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ ){
			m_vpcOscillators[i]->elementStep( );						
		}	
		/* ENVIRONMENT */	
		m_pcEnvironment->environmentStep ();		
		/* STATE VARIABLE WHEN FFT AVAILABLE */		
		if ( m_pcEnvironment->isFFT() ){
			/* MF variables*/
			_calculateMF( );
			/* FFT statistics */	
			_calculateFFTst ( );
			/* Send info to oscilators */
			TVFreqCmp* tmp_FFT = m_pcEnvironment->getGFFT();
			float tmp_amp;
			int   osc_cmp;
			for ( int i = 0 ; i < m_vOscillatorDist.size() ; i++ ){
				m_vOscillatorDist[i] = 0;
			}
			for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ ){
				osc_cmp = m_vpcOscillators[ i ]->getCmp();
				m_vpcOscillators  [ i ]->sendFFT( tmp_FFT , &m_sFFTst );					
				tmp_amp           = (*(m_pcEnvironment->getNCSignalAmp()))[ osc_cmp ]; 			
				m_vpcOscillators  [ i ]->sendMFArguments (  m_vR[ osc_cmp ] , m_vPhi[ osc_cmp ] , tmp_amp );
				m_vOscillatorDist [ osc_cmp ]++;
			}
			/* WRITE */	
			if ( m_bWriter && m_pcEnvironment->isFFT() )		
				_writer ( );
		}		
		/* VISU */			
		if ( m_bVisu  )
			m_pcPlotter->updateDisplay ();
		/* FINAL CONDITION */		
		if ( m_bVisu  ){
			if ( m_pcPlotter->closed() )
				SimFinished = true;
		}
		else{
			if ( m_nSimStep > m_nStepLimit )
				SimFinished = true;			
		}					
		m_nSimStep++;
	}
	return;
};

/****************************************************************/
void CSimulator::setKuramoto ( float input ){	
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ )
		m_vpcOscillators[i]->setKuramoto ( input );	
	return;
};

/****************************************************************/
void CSimulator::setSwitchPr ( float input ){
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ )
		m_vpcOscillators[i]->setSwitchPr ( input );
	return;
};

/****************************************************************/
void CSimulator::setAvoidNoise ( float input ){
	m_fN = input;
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ )
		m_vpcOscillators[i]->setAvoidNoise ( input );
	return;
};

/****************************************************************/
void CSimulator::_calculateMF   ( void ){
	TVFloat  tmp_real, tmp_img;
	TVInt    tmp_pop;
	TVFloat* EArg = m_pcEnvironment->getNCSignalArg();
	TVFloat* EAmp = m_pcEnvironment->getNCSignalAmp();
	for ( int i = 0 ; i < m_nFFTsize/2 ; i++ ){
		tmp_real.push_back ( (*EAmp)[i] * cos ( (*EArg)[i] ) );
		tmp_img.push_back  ( (*EAmp)[i] * sin ( (*EArg)[i] ) );
		tmp_pop.push_back  ( 0 );				
	}
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ ){
		tmp_real[m_vpcOscillators[i]->getCmp()] += cos ( m_vpcOscillators[i]->getArgument() );
		tmp_img [m_vpcOscillators[i]->getCmp()] += sin ( m_vpcOscillators[i]->getArgument() );
		tmp_pop [m_vpcOscillators[i]->getCmp()]++;
	}				
	m_vR.clear();
	m_vPhi.clear();
	m_vTheta.clear();
	for ( int i = 0 ; i < m_nFFTsize/2 ; i++ ){
		if ( ( float(tmp_pop[i]) + (*EAmp)[i] ) > 1e-3 ){ 
			tmp_real[i] /= ( float(tmp_pop[i]) + (*EAmp)[i] );
			tmp_img [i] /= ( float(tmp_pop[i]) + (*EAmp)[i] );
			complex<float> tmp_complex ( tmp_real[i] , tmp_img[i] );
			m_vR.push_back     ( abs( tmp_complex ) );
			m_vPhi.push_back   ( arg( tmp_complex ) );								
		}
		else{
			tmp_real[i] = 0.0;
			tmp_img [i] = 0.0;
			m_vR.push_back(0.0);
			m_vPhi.push_back(0.0);
		}
		float tmp_theta = 0.0;	
		if ( i != 0 )
			tmp_theta = m_vPhi.back() - fmod ( ( TWO_PI * float ( m_nSimStep) / ( m_nSampling * float( m_nFFTsize  )/float(i) ) ) , TWO_PI );
		if ( tmp_theta > M_PI )
			tmp_theta -= TWO_PI;
		else if ( tmp_theta < -M_PI )
			tmp_theta += TWO_PI;
		m_vTheta.push_back ( tmp_theta );
	}
	return;
};

/****************************************************************/
void CSimulator::_calculateFFTst ( void ){
	TVFreqCmp* tmp_FFT = m_pcEnvironment->getGFFT();
	/* THETA */	
	m_vFFTTheta.clear();
	for ( int i = 0 ; i < m_nFFTsize/2 ; i++ ){
		float tmp_theta = 0.0;	
		if ( i != 0 )
			tmp_theta = (*tmp_FFT)[i].phs - fmod ( ( TWO_PI * float ( m_nSimStep) / ( m_nSampling * float( m_nFFTsize  )/float(i) ) ) , TWO_PI );
		if ( tmp_theta > M_PI )
			tmp_theta -= TWO_PI;
		else if ( tmp_theta < -M_PI )
			tmp_theta += TWO_PI;
		m_vFFTTheta.push_back( tmp_theta );
	}
	/* AVERAGE AND VARIANCE OF THE SPECTRUM */
	m_sFFTst.FFTave = 0.0;
	for ( int i = 1 ; i < tmp_FFT->size() ; i++ ){
		m_sFFTst.FFTave += (*tmp_FFT)[i].amp;
	}			
	m_sFFTst.FFTave /= tmp_FFT->size();
	m_sFFTst.FFTvar = 0.0;
	for ( int i = 1 ; i < tmp_FFT->size() ; i++ ){
		m_sFFTst.FFTvar += pow ( (*tmp_FFT)[i].amp - m_sFFTst.FFTave , 2 );		
	}			
	m_sFFTst.FFTvar /= tmp_FFT->size();	
	/* HISTORIC MAXIMUM */
	for ( int i = 1 ; i < tmp_FFT->size() ; i++ ){
		if ( (*tmp_FFT)[i].amp > m_sFFTst.FFTmax )
			m_sFFTst.FFTmax   = (*tmp_FFT)[i].amp;		
	}
	/* AVERGA AND VARIANCE WITH MEMORY FACTOR */
	for ( int i = 0 ; i < m_nFFTsize/2 + 1 ; i++ ){
		if ( m_sFFTst.HAve[i] == 0.0 )
			m_sFFTst.HAve[i] = tmp_FFT->at(i).amp;
		else   
			m_sFFTst.HAve[i] = 0.9 * m_sFFTst.HAve[i] + 0.1 * tmp_FFT->at(i).amp;
		if ( m_sFFTst.HVar[i] == 0.0 )
			m_sFFTst.HVar[i] = pow ( tmp_FFT->at(i).amp - m_sFFTst.HAve[i] , 2 );
		else   
			m_sFFTst.HVar[i] = 0.9 * m_sFFTst.HVar[i] + 0.1 * pow ( tmp_FFT->at(i).amp - m_sFFTst.HAve[i] , 2 );
	}	
	/* AVERAGE, DEVIATION AND THEIR RELATIONSHIP OF THE FREQ_CMP AMPLITUDES */
	// Get new values
	for ( int i = 0 ; i < m_sFFTst.HAmp.size() ; i++ ){
		m_sFFTst.HAmp[i].push_back( tmp_FFT->at(i).amp );
	}
	// Clean the excess
	if ( m_sFFTst.HAmp[0].size() > m_nFFTst_hist ){
		for ( int i = 0 ; i < m_sFFTst.HAmp.size() ; i++ ){
			m_sFFTst.HAmp[i].erase( m_sFFTst.HAmp[i].begin() );
		}		
	}
	// Calculate average and deviation if there are enough samples
	if ( m_sFFTst.HAmp[0].size() >= m_nFFTst_hist  ){
		TVFloat ave, var;
		for ( int i = 0 ; i < m_sFFTst.HAmp.size() ; i++ ){
			ave.push_back(0.0);
			var.push_back(0.0);
		}
		for ( int j = 0 ; j < m_sFFTst.HAmp.size() ; j++ ) {	
			for ( int i = 0 ; i < m_nFFTst_hist ; i++ ){			
				ave[j] += m_sFFTst.HAmp[j][ m_sFFTst.HAmp[0].size() - i - 1 ];
			}
			ave[j] /= m_nFFTst_hist;			
		}
		for ( int j = 0 ; j < m_sFFTst.HAmp.size() ; j++ ) {	
			for ( int i = 0 ; i < m_nFFTst_hist ; i++ ){			
				var[j] += fabs ( m_sFFTst.HAmp[j][ m_sFFTst.HAmp[0].size() - i - 1 ] - ave[j] );
			}
			var[j] /= m_nFFTst_hist;			
		}		
		m_sFFTst.HRel.clear();
		for ( int i = 0 ; i < m_sFFTst.HAmp.size() ; i++ )
			m_sFFTst.HRel.push_back( var[i]/ave[i] );

	}
	else{
		m_sFFTst.HRel.clear();
		for ( int i = 0 ; i < m_sFFTst.HAmp.size() ; i++ )
			m_sFFTst.HRel.push_back( 0.0 );
	}	
	/* FORM FACTOR EQUATIONS */
	switch( m_nF ){
		case 1:				
			m_sFFTst.FFTff = 0.0;
			for ( int i = 1 ; i < tmp_FFT->size() ; i++ ){
				m_sFFTst.FFTff +=  pow ( tmp_FFT->at(i).amp , 4 );
			}
			m_sFFTst.FFTff /= float(tmp_FFT->size()) * pow ( m_sFFTst.FFTmax , 4 ); 	
			break;
		case 2:
			m_sFFTst.FFTff = 0.0;
			for ( int i = 1 ; i < tmp_FFT->size() ; i++ ){
				m_sFFTst.FFTff +=  pow ( tmp_FFT->at(i).amp , 4 ) * exp( - m_fN * m_sFFTst.HRel[i] );
			}
			m_sFFTst.FFTff /= float(tmp_FFT->size()) * pow ( m_sFFTst.FFTmax , 4 );   			
			break;
		case 3:
			m_sFFTst.FFTff = 0.0;
			for ( int i = 1 ; i < tmp_FFT->size() ; i++ ){
				m_sFFTst.FFTff +=  pow ( (*tmp_FFT)[i].amp / (*tmp_FFT)[0].amp , 4 );

			}
			m_sFFTst.FFTff /= tmp_FFT->size(); 
			break;
		case 4:
			m_sFFTst.FFTff = m_sFFTst.FFTvar/m_sFFTst.FFTave;
			break;
		default:			
			m_sFFTst.FFTff = 0.0;
			break;
	}
	return;
};

/****************************************************************/
TVFloat CSimulator::getEvaluation ( void ){
	TVFloat results;
	TVFloat* tmp_sgn = m_pcEnvironment->getGSignal();
	int ini_ptr = tmp_sgn->size() - 43200;
	// Evaluate 30 days	
	float   tmp_daily_max, tmp_daily_min;	
	TVFloat daily_max, daily_min;
	float   month_max = tmp_sgn->at( ini_ptr );
	float   month_min = tmp_sgn->at( ini_ptr );
	for ( int i = 0 ; i < 43200 ; i++ ){
		// Monthly
		if ( month_max < tmp_sgn->at( ini_ptr + i ) )
			month_max = tmp_sgn->at( ini_ptr + i );
		if ( month_min > tmp_sgn->at( ini_ptr + i ) )
			month_min = tmp_sgn->at( ini_ptr + i );
		// Daily 
		if ( i % 1440 == 0 ){
			if ( i != 0 ){
				daily_max.push_back( tmp_daily_max );
				daily_min.push_back( tmp_daily_min );
			}
			tmp_daily_max = tmp_daily_min = tmp_sgn->at( ini_ptr + i );						
		}
		if ( tmp_daily_max < tmp_sgn->at( ini_ptr + i ) )
			tmp_daily_max = tmp_sgn->at( ini_ptr + i );
		if ( tmp_daily_min > tmp_sgn->at( ini_ptr + i ) )
			tmp_daily_min = tmp_sgn->at( ini_ptr + i );
	}
	float   month_rel = month_max / month_min;
	TVFloat daily_rel;
	for ( int i = 0 ; i < daily_max.size() ; i++ ){
		daily_rel.push_back( daily_max[i] / daily_min[i] );
	}
	float  daily_rel_ave = 0.0;
	for ( int i = 0 ; i < daily_rel.size() ; i++ ){
		daily_rel_ave += daily_rel[i];
	}
	daily_rel_ave /= daily_rel.size();

	// Get results
	results.push_back( month_rel );	
	results.push_back( daily_rel_ave );
	return results;
};

/****************************************************************/
void CSimulator::_writer ( void ){
	ofstream out;
	string   file_name;
	/* Y */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/y.table" );
	out.open         ( file_name.c_str() , ios::app );
	out << m_pcEnvironment->getGSampled()->back() << endl;
	out.close();
	/* Y FFT AMP */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/fft.table" );
	out.open         ( file_name.c_str() , ios::app );	
	for ( int i = 0 ; i < m_pcEnvironment->getGFFTamp()->size() ; i++ ){
		out << (*(m_pcEnvironment->getGFFTamp()))[i] << " ";		
	}
	out << endl;
	out.close();
	/* Y FFT STATISTICS */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/res.table" );
	out.open         ( file_name.c_str() , ios::app );
	out << m_sFFTst.FFTave << " " << m_sFFTst.FFTvar << " " << m_sFFTst.FFTff << endl;
	out.close();
	/* NC FFT */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/ncfft.table" );
	out.open         ( file_name.c_str() , ios::app );	
	for ( int i = 0 ; i < m_pcEnvironment->getNCFFTamp()->size() ; i++ ){
		out << (*(m_pcEnvironment->getNCFFTamp()))[i] << " ";		
	}
	out << endl;
	out.close();
	/* DISTRIBUTION */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/dis.table" );
	out.open         ( file_name.c_str() , ios::app );
	for ( int i = 0 ; i < m_vOscillatorDist.size() ; i++ )	
		out << m_vOscillatorDist[i] << " ";
	out << endl;
	out.close();
	/* C FFT */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/cfft.table" );
	out.open         ( file_name.c_str() , ios::app );	
	for ( int i = 0 ; i < m_pcEnvironment->getCFFTamp()->size() ; i++ ){
		out << (*(m_pcEnvironment->getCFFTamp()))[i] << " ";		
	}
	out << endl;
	out.close();
	/* PHASES */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/ph.table" );
	out.open         ( file_name.c_str() , ios::app );
	out << "0 ";
	out << m_vTheta[m_nWCmp] << " ";
	out << m_vFFTTheta[m_nWCmp] << " ";
	for ( int i = 0 ; i < m_vpcOscillators.size() ; i++ )
		out << m_vpcOscillators[i]->getPhase() << " ";
	out << endl;
	out.close();
	/* COHERENCE */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/r.table" );
	out.open         ( file_name.c_str() , ios::app );
	for ( int i = 0 ; i < m_vR.size() ; i++ )
		out << m_vR[i] << " ";
	out << endl; 
	out.close();		
	return;
};

/****************************************************************/
void CSimulator::_writerInit ( void ){
	ofstream out;	
	string   file_name;
	/* Y */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/y.table" );
	out.open         ( file_name.c_str() );	
	out.close();
	/* Y FFT */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/fft.table" );
	out.open         ( file_name.c_str() );	
	out.close();
	/* Y FFT STATISTICS */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/res.table" );
	out.open         ( file_name.c_str() );	
	out.close();
	/* NC FFT */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/ncfft.table" );
	out.open         ( file_name.c_str() );	
	out.close();
	/* DISTRIBUTION */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/dis.table" );
	out.open         ( file_name.c_str() );	
	out.close();
	/* C FFT */
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/cfft.table" );
	out.open         ( file_name.c_str() );	
	out.close();	
	/* PHASES */	
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/ph.table" );
	out.open         ( file_name.c_str() );	
	out.close();
	/* COHERENCE */	
	file_name.assign ( m_sOutputFolder );
	file_name.append ( "/r.table" );
	out.open         ( file_name.c_str() );	
	out.close();	
	return;
};

/****************************************************************/
void CSimulator::_configureVisu ( XMLElement* conf ){

	m_pcPlotter = new CPlotter ( );	
	TVFloat *tmp_f1, *tmp_f2;
	string elemName, attr;
	attr       = conf->Attribute("rf_rate");
	m_pcPlotter->setRefreshRate ( atoi( attr.c_str() ) );
	int cnt = 0;
	int type;
	for( XMLElement* e = conf->FirstChildElement() ; e != NULL ; e = e->NextSiblingElement() ){
		attr  = e->Attribute("type");
		type  = atoi( attr.c_str() );
		if ( type == 00 ){					
			tmp_f1 = m_pcEnvironment->getGSignal  ( );
			tmp_f2 = m_pcEnvironment->getGSampled ( );
			attr  = e->Attribute("title");
			m_pcPlotter->createDisplay ( tmp_f1 , tmp_f2 , m_nSampling , attr );
			attr   = e->Attribute("x_lng");
			m_pcPlotter->setXrange     ( cnt , atoi( attr.c_str() ) , "Time [k]" );
			int y_ini, y_end;
			attr   = e->Attribute("y_ini");
			y_ini  = atoi( attr.c_str() );
			attr   = e->Attribute("y_end");
			y_end  = atoi( attr.c_str() );
			m_pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
			cnt++;
		}
		else if ( type == 01 ){					
			tmp_f1 = &m_vR;
			attr   = e->Attribute("title");
			m_pcPlotter->createDisplay ( NULL , tmp_f1       , 1 , attr );			
			m_pcPlotter->setXrange     ( cnt  , m_nFFTsize/2 + 1 , "Freq[W]" );
			int y_ini, y_end;
			attr   = e->Attribute("y_ini");
			y_ini  = atoi( attr.c_str() );
			attr   = e->Attribute("y_end");
			y_end  = atoi( attr.c_str() );
			m_pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
			cnt++;
		}
		else if ( type == 10 ){					
			tmp_f1 = m_pcEnvironment->getGFFTamp();
			attr   = e->Attribute("title");
			m_pcPlotter->createDisplay ( NULL , tmp_f1       , 1 , attr );			
			m_pcPlotter->setXrange     ( cnt  , m_nFFTsize/2 + 1 , "Freq[W]" );
			int y_ini, y_end;
			attr   = e->Attribute("y_ini");
			y_ini  = atoi( attr.c_str() );
			attr   = e->Attribute("y_end");
			y_end  = atoi( attr.c_str() );
			m_pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
			cnt++;
		}
		else if ( type == 11 ){					
			tmp_f1 = m_pcEnvironment->getNCFFTamp();
			attr   = e->Attribute("title");
			m_pcPlotter->createDisplay ( NULL , tmp_f1       , 1 , attr );			
			m_pcPlotter->setXrange     ( cnt    , m_nFFTsize/2 + 1, "Freq[W]" );
			int y_ini, y_end;
			attr   = e->Attribute("y_ini");
			y_ini  = atoi( attr.c_str() );
			attr   = e->Attribute("y_end");
			y_end  = atoi( attr.c_str() );
			m_pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
			cnt++;
		}
		else if ( type == 12 ){					
			tmp_f1 = m_pcEnvironment->getCFFTamp();
			attr   = e->Attribute("title");
			m_pcPlotter->createDisplay ( NULL , tmp_f1       , 1 , attr );			
			m_pcPlotter->setXrange     ( cnt    , m_nFFTsize/2 + 1, "Freq[W]" );
			int y_ini, y_end;
			attr   = e->Attribute("y_ini");
			y_ini  = atoi( attr.c_str() );
			attr   = e->Attribute("y_end");
			y_end  = atoi( attr.c_str() );
			m_pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
			cnt++;
		}
		else if ( type == 13 ){					
			tmp_f1 = &m_vOscillatorDist;
			attr   = e->Attribute("title");
			m_pcPlotter->createDisplay ( NULL , tmp_f1       , 1 , attr );			
			m_pcPlotter->setXrange     ( cnt    , m_nFFTsize/2 + 1, "Freq[W]" );
			int y_ini, y_end;
			attr   = e->Attribute("y_ini");
			y_ini  = atoi( attr.c_str() );
			attr   = e->Attribute("y_end");
			y_end  = atoi( attr.c_str() );
			m_pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
			cnt++;
		}
	}
	return;
};



