/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataManager.h
// Created:  9/21/2012
// Author:  K. Loux
// Description:  Data container/manager.  Allows single TFs to appear in options grid,
//				 while maintaining separate data for amplitude, phase (and total for
//				 all TFs).

// LibPlot2D headers
#include <lp2d/utilities/dataset2D.h>
#include <lp2d/utilities/managedList.h>

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <vector>
#include <utility>

class DataManager
{
public:
	DataManager();

	bool AddTransferFunction(const wxString &numerator, const wxString &denominator);
	bool UpdateTransferFunction(const unsigned int &i, const wxString &numerator, const wxString &denominator);

	void SetFrequencyUnitsHertz();
	void SetFrequencyUnitsRadPerSec();
	void SetFrequencyRange(const double &min, const double &max);

	LibPlot2D::Dataset2D& GetAmplitudeData(const unsigned int &i) { return *amplitudePlots[i]; }
	LibPlot2D::Dataset2D& GetPhaseData(const unsigned int &i) { return *phasePlots[i]; }
	LibPlot2D::Dataset2D& GetTotalAmplitudeData();
	LibPlot2D::Dataset2D& GetTotalPhaseData();

	wxString GetNumerator(const unsigned int &i) const { return transferFunctions[i].first; }
	wxString GetDenominator(const unsigned int &i) const { return transferFunctions[i].second; }

	unsigned int GetCount() const { return amplitudePlots.GetCount(); }

	void RemoveAllTransferFunctions();
	void RemoveTransferFunctions(const unsigned int &i);

	void UpdateTotalTransferFunctionData();

private:
	double minFreq, maxFreq;// [Hz]
	bool frequencyHertz;

	static const unsigned int numberOfPoints;

	LibPlot2D::ManagedList<LibPlot2D::Dataset2D> amplitudePlots;
	LibPlot2D::ManagedList<LibPlot2D::Dataset2D> phasePlots;

	bool totalNeedsUpdate;
	LibPlot2D::Dataset2D totalAmplitude;
	LibPlot2D::Dataset2D totalPhase;

	std::vector<std::pair<wxString, wxString>> transferFunctions;
	wxString ConstructTotalTransferFunction() const;
	void UpdateAllTransferFunctionData();

	wxString AssembleTransferFunctionString(const wxString &numerator, const wxString &denominator) const;
	wxString AssembleTransferFunctionString(const std::pair<wxString, wxString> &tf) const;
};
