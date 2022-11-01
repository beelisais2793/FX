/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tfDialog.h
// Created:  3/21/2014
// Author:  K. Loux
// Description:  Main frame for the application.

#ifndef _TF_DIALOG_H_
#define _TF_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// The main frame class
class TFDialog : public wxDialog
{
public:
	TFDialog(wxWindow *parent);
	TFDialog(wxWindow *parent, const wxString &numerator, const wxString &denominator);
	virtual ~TFDialog() {}

	wxString GetNumerator() const { return numeratorCtrl->GetValue(); }
	wxString GetDenominator() const { return denominatorCtrl->GetValue(); }

private:
	void CreateControls(const wxString &numerator = "", const wxString &denominator = "");

	wxTextCtrl *numeratorCtrl;
	wxTextCtrl *denominatorCtrl;

protected:
	virtual bool TransferDataFromWindow();
};

#endif// _TF_DIALOG_H_
