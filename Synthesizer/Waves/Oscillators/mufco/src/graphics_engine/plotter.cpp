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
/* PLOTTER METHODS						            */
/* This library uses the CImg Library project (http://cimg.sourceforge.net) */
/****************************************************************************/

#include "plotter.h"

const unsigned char cimg_red[] = { 255,0,0 }, cimg_green[] = { 0,210,0 }, cimg_wgreen[] = { 124,210,124 }, cimg_blue[] = { 0,0,255 }, cimg_wblue[] = { 124,124,255 }, cimg_black[] = {0,0,0}, cimg_orange[] = {255,100,0}, cimg_white[] = {255,255,255};

/******************************************************************************/
CPlotter::CPlotter  ( void ){
	m_nStep    = 0;
	m_nRefresh = 0;
};

/******************************************************************************/
CPlotter::~CPlotter  ( void ){	
	for ( int i = 0 ; i < vDisplay.size() ; i++ )
		delete vDisplay[i].screen;	
	vDisplay.clear();
};

/******************************************************************************/
void CPlotter::createDisplay ( TVFloat* inputData , TVFloat* v_marks , int v_marks_sp , string name ){
	sDisplay            tmp_disp;	
	tmp_disp.picture    = CImg<unsigned char> (600,200,1,3,255);
	tmp_disp.screen     = new CImgDisplay ( tmp_disp.picture , name.c_str() );
	tmp_disp.old_width  = tmp_disp.screen->width();
	tmp_disp.old_height = tmp_disp.screen->height();	
	tmp_disp.function   = inputData;
	tmp_disp.v_marks    = v_marks;
	tmp_disp.v_marks_sp = v_marks_sp;
	vDisplay.push_back ( tmp_disp );	
	return;
};

/******************************************************************************/
void CPlotter::setXrange ( int ptr , int xrange , string xcaption ){	
	vDisplay[ptr].xrange   = xrange;		
	vDisplay[ptr].xcaption = xcaption;
	_replotX  ( ptr );	
	if ( xcaption.size() != 0 )
		_captionX ( ptr );	
	return;
};

/******************************************************************************/
void CPlotter::setYrange ( int ptr , int y_ini , int y_end , string ycaption ){	
	vDisplay[ptr].y_ini    = y_ini;
	vDisplay[ptr].y_end    = y_end;
	vDisplay[ptr].ycaption = ycaption;	
	_replotY  ( ptr );	
	if ( ycaption.size() != 0 )
		_captionY ( ptr );
	return;
};

/******************************************************************************/
void CPlotter::updateDisplay ( void ){
	if (  m_nStep >= m_nRefresh ){
		for ( int i = 0 ; i < vDisplay.size() ; i++ ){
			if ( vDisplay[i].function != NULL && vDisplay[i].function->size() >  vDisplay[i].xrange )
				_replotX( i );
			else if ( vDisplay[i].v_marks != NULL && vDisplay[i].v_marks_sp * vDisplay[i].v_marks->size() >  vDisplay[i].xrange )
				_replotX( i );
			_updateTimeDisplay ( i );				
		}
		m_nStep = 0;
	}
	else
		m_nStep++;
	return;
};

/******************************************************************************/
void CPlotter::_replotX ( int ptr ){
	int width, height, subwidth, subheight;
	width  = vDisplay[ptr].screen->width();
	height = vDisplay[ptr].screen->height();
	if ( width <= 50 )
		subwidth = 0;
	else
		subwidth = width - 50;
	if ( height <= 50 )
		subheight = 0;
	else
		subheight = height - 50;
	CImg<unsigned char> subvisu  ( subwidth , 25       , 1 , 3 , 255 );
	if ( vDisplay[ptr].function != NULL && vDisplay[ptr].xrange < vDisplay[ptr].function->size() )
		subvisu.draw_axes( vDisplay[ptr].function->size() - vDisplay[ptr].xrange , vDisplay[ptr].function->size() - 1 , 0 , 0 , cimg_black );
	else if ( vDisplay[ptr].v_marks != NULL && vDisplay[ptr].xrange < vDisplay[ptr].v_marks_sp * vDisplay[ptr].v_marks->size() )
		subvisu.draw_axes( vDisplay[ptr].v_marks_sp * vDisplay[ptr].v_marks->size() - vDisplay[ptr].xrange , vDisplay[ptr].v_marks_sp * vDisplay[ptr].v_marks->size() - 1 , 0 , 0 , cimg_black );
	else
		subvisu.draw_axes( 0 , vDisplay[ptr].xrange , 0 , 0 , cimg_black );
	vDisplay[ptr].picture.draw_image ( 50 , subheight , 0 , 0 , subvisu , 1 );	
	return;
};

/******************************************************************************/
void CPlotter::_captionX ( int ptr ){
	int width, height, subwidth, subheight;
	width  = vDisplay[ptr].screen->width();
	height = vDisplay[ptr].screen->height();
	if ( width <= 50 )
		subwidth = 0;
	else
		subwidth = width - 50;
	if ( height <= 50 )
		subheight = 0;
	else
		subheight = height - 50;
	CImg<unsigned char> subvisu      ( subwidth   , 25 , 1 , 3 , 255 );
	subvisu.draw_text                ( subwidth/2 , 0  , vDisplay[ptr].xcaption.c_str() , cimg_black );
	vDisplay[ptr].picture.draw_image ( 50, subheight + 25 , 0, 0, subvisu , 1 );
	return;
};

/******************************************************************************/
void CPlotter::_replotY ( int ptr ){
	int width, height, subheight;
	width  = vDisplay[ptr].screen->width();
	height = vDisplay[ptr].screen->height();
	if ( height <= 50 )
		subheight = 0;
	else
		subheight = height - 50;

	CImg<unsigned char> subvisu  ( 35 , subheight , 1, 3, 255 );
	subvisu.draw_axes            ( 34 , 34, vDisplay[ptr].y_end, vDisplay[ptr].y_ini, cimg_black);
	vDisplay[ptr].picture.draw_image ( 15 , 0             , 0, 0, subvisu    , 1 );
	return;
};

/******************************************************************************/
void CPlotter::_captionY ( int ptr ){
	int width, height, subheight;
	width  = vDisplay[ptr].screen->width();
	height = vDisplay[ptr].screen->height();
	if ( height <= 50 )
		subheight = 0;
	else
		subheight = height - 50;
	CImg<unsigned char> verText   ( 100 , 15 , 1, 3, 255);
	verText.draw_text( 0 , 0 , vDisplay[ptr].ycaption.c_str() , cimg_black);
	verText.rotate(-90.0);
	CImg<unsigned char> subvisu ( 15    , subheight , 1, 3, 255);
	subvisu.draw_image ( 0 , subheight/2 - 75 , 0, 0, verText  , 1 );
	vDisplay[ptr].picture.draw_image ( 0  , 0             , 0, 0, subvisu , 1 );
	return;
};

/******************************************************************************/
bool CPlotter::closed ( void ){
	bool result = false;
	for ( int i = 0 ; i < vDisplay.size() ; i++ )
		result |= vDisplay[i].screen->is_closed();
	return result;
};

/******************************************************************************/
void CPlotter::_updateTimeDisplay ( int ptr ){
	CImg<unsigned char> subvisu;
	int width , height, subheight, subwidth;
	int ini, fin;
	/* Scale */
	vDisplay[ptr].screen->resize();
	width  = vDisplay[ptr].screen->width();
	height = vDisplay[ptr].screen->height();
	if ( ( width != vDisplay[ptr].old_width ) || ( height != vDisplay[ptr].old_height )){
		vDisplay[ptr].picture.assign ( width , height , 1, 3, 255 );
		_replotX  ( ptr );
		_replotY  ( ptr );
		_captionX ( ptr );
		_captionY ( ptr );
		vDisplay[ptr].old_width  = width; 
		vDisplay[ptr].old_height = height;
	}

	if ( width <= 50 )
		subwidth = 0;
	else
		subwidth = width - 50;
	if ( height <= 50 )
		subheight = 0;
	else
		subheight = height - 50;

	subvisu.assign ( subwidth , subheight , 1, 3, 255);

	float fScale = float( vDisplay[ptr].y_end - vDisplay[ptr].y_ini );
	float fStepW;
	int   x0, x1, y0, y1;

	/* Level 0 */
	if ( vDisplay[ptr].y_ini < 0 ){
		x0 = 0;
		x1 = subwidth;
		y0 = subheight * (( 1.0 - ( - vDisplay[ptr].y_ini)/ fScale ));
		y1 = y0;
		subvisu.draw_line( x0 , y0 , x1 , y1 , cimg_red);
	}

	if (vDisplay[ptr].function != NULL){	
		fStepW = float( subwidth )/float( vDisplay[ptr].xrange );		
		ini = int( vDisplay[ptr].function->size() ) - ( vDisplay[ptr].xrange + 1 );
		if (ini < 0)
			ini = 0;			
		fin = vDisplay[ptr].function->size();
		for (int j = 0 ; j < (fin - ini - 1) ; j++){	
			x0 = j  * fStepW;						
			x1 = x0 + fStepW;
			y0 = subheight * (( 1.0 - ( (*(vDisplay[ptr].function))[ini + j    ] - vDisplay[ptr].y_ini)/ fScale ));
			y1 = subheight * (( 1.0 - ( (*(vDisplay[ptr].function))[ini + j + 1] - vDisplay[ptr].y_ini)/ fScale ));
			subvisu.draw_line( x0 , y0 , x1 , y1 , cimg_blue);
			if ( vDisplay[ptr].v_marks != NULL && (ini + j)%vDisplay[ptr].v_marks_sp == 0 ){	
				y0 = subheight * (( 1.0 - ( - vDisplay[ptr].y_ini)/ fScale ));
				y1 = subheight * (( 1.0 - ( (*(vDisplay[ptr].v_marks))[ (ini + j)/vDisplay[ptr].v_marks_sp ] - vDisplay[ptr].y_ini)/ fScale ));		
				subvisu.draw_line( x0 , y0 , x0 , y1 , cimg_green);			
			}											
		}
	}
	else if ( vDisplay[ptr].v_marks != NULL ){
		fStepW = float( vDisplay[ptr].v_marks_sp * subwidth )/float( vDisplay[ptr].xrange );
		ini = int( vDisplay[ptr].v_marks->size() ) - int ( float ( vDisplay[ptr].xrange )/float( vDisplay[ptr].v_marks_sp ) );
		if (ini < 0)
			ini = 0;			
		fin = vDisplay[ptr].v_marks->size();
		for (int j = 0 ; j < (fin - ini - 1) ; j++){
			x0 = j  * fStepW;
			y0 = subheight * (( 1.0 - ( - vDisplay[ptr].y_ini)/ fScale ));
			y1 = subheight * (( 1.0 - ( (*(vDisplay[ptr].v_marks))[ ini + j ] - vDisplay[ptr].y_ini)/ fScale ));		
			subvisu.draw_line( x0 , y0 , x0 , y1 , cimg_blue);
		}
	}
	
	vDisplay[ptr].picture.draw_image ( 50 , 0              , 0, 0, subvisu, 1 );
	vDisplay[ptr].screen->display    ( vDisplay[ptr].picture );

	return;
};
