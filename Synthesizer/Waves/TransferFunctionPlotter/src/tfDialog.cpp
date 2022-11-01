/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tfDialog.cpp
// Created:  3/21/2014
// Author:  K. Loux
// Description:  Main frame for the application.

// Local headers
#include "tfDialog.h"
#include "expressionTree.h"

// wxWidgets headers
#include <wx/statline.h>

TFDialog::TFDialog(wxWindow *parent) : wxDialog(parent, wxID_ANY, _T("Specify Transfer Function"),
	wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
{
	CreateControls();
}

TFDialog::TFDialog(wxWindow *parent, const wxString &numerator,
	const wxString &denominator) : wxDialog(parent, wxID_ANY, _T("Specify Transfer Function"),
	wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
{
	CreateControls(numerator, denominator);
}

void TFDialog::CreateControls(const wxString &numerator, const wxString &denominator)
{
	wxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxGROW, 5);

	wxSizer *tfRowSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *tfFractionSizer = new wxBoxSizer(wxVERTICAL);
	tfRowSizer->Add(new wxStaticText(this, wxID_ANY, _T("TF =")), 0, wxALIGN_CENTER_VERTICAL);
	tfRowSizer->Add(tfFractionSizer, 1, wxGROW);

	numeratorCtrl = new wxTextCtrl(this, wxID_ANY, numerator);
	denominatorCtrl = new wxTextCtrl(this, wxID_ANY, denominator);

	tfFractionSizer->Add(numeratorCtrl, 1, wxGROW);
	tfFractionSizer->Add(new wxStaticLine(this), 0, wxGROW | wxTOP | wxBOTTOM, 2);
	tfFractionSizer->Add(denominatorCtrl, 1, wxGROW);

	mainSizer->Add(tfRowSizer, 1, wxGROW);
	mainSizer->AddSpacer(15);
	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxGROW);

	SetSizerAndFit(topSizer);
	SetMaxSize(wxSize(-1, GetSize().GetY()));
	SetMinSize(wxSize(400, GetSize().GetY()));

	// Required to call this after adjusting min and max sizes in order
	// for the dialog to center properly
	Fit();

	numeratorCtrl->SetFocus();
}

bool TFDialog::TransferDataFromWindow()
{
	ExpressionTree et;
	wxString errorString = et.CheckExpression(numeratorCtrl->GetValue());
	if (!errorString.IsEmpty())
	{
		wxMessageBox(errorString, _T("Invalid Numerator"), 5L, this);
		return false;
	}

	errorString = et.CheckExpression(denominatorCtrl->GetValue());
	if (!errorString.IsEmpty())
	{
		wxMessageBox(errorString, _T("Invalid Denominator"), 5L, this);
		return false;
	}

	if (numeratorCtrl->GetValue().IsEmpty() || denominatorCtrl->GetValue().IsEmpty())
	{
		wxMessageBox(_T("Values must not be blank."), _T("Invalid Transfer Function"), 5L, this);
		return false;
	}

	return true;
}
