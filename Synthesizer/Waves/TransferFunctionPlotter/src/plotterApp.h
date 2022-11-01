/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotterApp.h
// Created:  9/18/2012
// Author:  K. Loux
// Description:  The application class.

#ifndef _PLOTTER_APP_H_
#define _PLOTTER_APP_H_

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class MainFrame;

// The application class
class TransferFunctionPlotterApp : public wxApp
{
public:
	// Initialization function
	bool OnInit();

	// The name of the application
	static const wxString transferFunctionPlotterTitle;// As displayed
	static const wxString transferFunctionPlotterName;// Internal
	static const wxString creator;
	static const wxString versionString;

private:
	// The main class for the application - this object is the parent for all other objects
	MainFrame *mainFrame;
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(TransferFunctionPlotterApp);

#endif// _PLOTTER_APP_H_
