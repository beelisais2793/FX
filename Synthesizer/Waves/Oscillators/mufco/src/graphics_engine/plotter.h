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

/****************************************************************************/
/* PLOTTER HEADER 							    */
/* This library uses the CImg Library project (http://cimg.sourceforge.net) */
/****************************************************************************/
#ifndef PLOTTER_H_
#define PLOTTER_H_

class CPlotter;

/****************************************************************/
/* LIBRARIES */
#include "CImg.h"
#include "common.h"

using namespace cimg_library;

struct sDisplay {

	CImgDisplay         *screen;
	CImg<unsigned char> picture;

	string xcaption, ycaption;

	//CImg<unsigned char> Xaxis    , Yaxis;
	//CImg<unsigned char> Xcaption , Ycaption;
	//bool        replotX, replotY;

	int         old_width, old_height;

	int         xrange;	
	int         y_ini, y_end;

	TVFloat*    function;
	TVFloat*    v_marks;
	int         v_marks_sp;

	TVFloat*    inputData_freq;
};
typedef vector<sDisplay> TVDisplay;

class CPlotter {

	public:

	CPlotter  ( void );
	~CPlotter ( void );

	void setRefreshRate ( int input ){ m_nRefresh = input; };

	void createDisplay  ( TVFloat* , TVFloat* , int , string );	
	

	void setXrange      ( int , int , string );
	void setYrange      ( int , int , int , string );

	void updateDisplay  ( void );
	bool closed         ( void );

	private:

	int m_nStep;
	int m_nRefresh;

	TVDisplay vDisplay;

	void _replotX  ( int );
	void _replotY  ( int );
	void _captionX ( int );
	void _captionY ( int );
	void _updateTimeDisplay ( int );
	//void _updateFreqDisplay ( int );


};


#endif
