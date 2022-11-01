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

/******************************************************************************/
/* MULTY FREQUENCY COUPLED OSCILLATORS SIMULATOR (MuFCO) M                    */
/******************************************************************************/

/******************************************************************************/
/* LIBRARIES 								      */
#include "simulator.h"

/******************************************************************************/
/* MAIN 								      */
int main(int argc, char** argv) {	
	string pcExperimentFilename;
	for(int i=1; i<argc; i++) { //loop on command line arguments
		if(argv[i][0] != '-') { // Type error
			cout << "./MuFCOSim [-p <parameters file>]" << endl;
			exit(0);
		}
		switch(argv[i][1]) { //switch on current argument
			/* Get Input File */
			case 'p':
				++i;
				pcExperimentFilename.assign(argv[i]);
				cout << " Filename: " << pcExperimentFilename << " CHOOSEN." << endl;
				break;					
			/* Option not recognized*/
			default:
				cout << "ERROR: Option not recognized" << endl;
				break;
		}
	}
	/* EXECUTION */	
	CSimulator *pcSimulator = new CSimulator ( pcExperimentFilename );
	pcSimulator->executeSimulation ( );
	TVFloat results = pcSimulator->getEvaluation ( );
	cout << " RESULTS: ";
	for ( int i = 0 ; i < results.size() ; i++ ){
		cout << results[i] << " ";
	}	
	cout << endl;
	delete pcSimulator;
	return 0;		
};



