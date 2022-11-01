/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataManager.cpp
// Created:  9/21/2012
// Author:  K. Loux
// Description:  Data container/manager.  Allows single TFs to appear in options grid,
//				 while maintaining separate data for amplitude, phase (and total for
//				 all TFs).

// Local headers
#include "dataManager.h"
#include "expressionTree.h"

// LibPlot2D headers
#include <lp2d/utilities/math/plotMath.h>

const unsigned int DataManager::numberOfPoints(5000);

DataManager::DataManager()
{
	frequencyHertz = true;
	minFreq = 0.01;
	maxFreq = 100.0;
}

bool DataManager::AddTransferFunction(const wxString &numerator, const wxString &denominator)
{
	amplitudePlots.Add(std::make_unique<LibPlot2D::Dataset2D>(numberOfPoints));
	phasePlots.Add(std::make_unique<LibPlot2D::Dataset2D>(numberOfPoints));

	transferFunctions.push_back(std::make_pair(numerator, denominator));
	return UpdateTransferFunction(amplitudePlots.GetCount() - 1, numerator, denominator);
}

bool DataManager::UpdateTransferFunction(const unsigned int &i, const wxString &numerator, const wxString &denominator)
{
	LibPlot2D::Dataset2D magnitude;
	LibPlot2D::Dataset2D phase;

	transferFunctions[i].first = numerator;
	transferFunctions[i].second = denominator;

	ExpressionTree expressionTree;
	wxString errorString;
	if (frequencyHertz)
		errorString = expressionTree.Solve(AssembleTransferFunctionString(transferFunctions[i]),
			minFreq, maxFreq, numberOfPoints, magnitude, phase);
	else
	{
		errorString = expressionTree.Solve(AssembleTransferFunctionString(transferFunctions[i]),
			minFreq * 0.5 / M_PI, maxFreq * 0.5 / M_PI, numberOfPoints, magnitude, phase);

		magnitude.MultiplyXData(2.0 * M_PI);
		phase.MultiplyXData(2.0 * M_PI);
	}

	if (!errorString.IsEmpty())
	{
		wxMessageBox(errorString);
		return false;
	}

	assert(magnitude.GetNumberOfPoints() == amplitudePlots[i]->GetNumberOfPoints());
	assert(phase.GetNumberOfPoints() == phasePlots[i]->GetNumberOfPoints());

	for (unsigned int j = 0; j < magnitude.GetNumberOfPoints(); ++j)
	{
		amplitudePlots[i]->GetX()[j] = magnitude.GetX()[j];
		amplitudePlots[i]->GetY()[j] = magnitude.GetY()[j];
		phasePlots[i]->GetX()[j] = phase.GetX()[j];
		phasePlots[i]->GetY()[j] = phase.GetY()[j];
	}

	totalNeedsUpdate = true;

	return true;
}

void DataManager::SetFrequencyUnitsHertz()
{
	if (frequencyHertz)
		return;
	frequencyHertz = true;
	UpdateAllTransferFunctionData();
}

void DataManager::SetFrequencyUnitsRadPerSec()
{
	if (!frequencyHertz)
		return;
	frequencyHertz = false;
	UpdateAllTransferFunctionData();
}

void DataManager::SetFrequencyRange(const double &min, const double &max)
{
	if (min >= max)
		return;

	minFreq = min;
	maxFreq = max;

	UpdateAllTransferFunctionData();
}

wxString DataManager::ConstructTotalTransferFunction() const
{
	wxString totalNum, totalDen;
	for (unsigned int i = 0; i < transferFunctions.size(); ++i)
	{
		totalNum.Append(_T("(") + transferFunctions[i].first + _T(")"));
		totalDen.Append(_T("(") + transferFunctions[i].second + _T(")"));
		if (i < transferFunctions.size() - 1)
		{
			totalNum.Append(_T("*"));
			totalDen.Append(_T("*"));
		}
	}

	return AssembleTransferFunctionString(totalNum, totalDen);
}

void DataManager::UpdateTotalTransferFunctionData()
{
	if (!totalNeedsUpdate)
		return;

	const wxString totalTF(ConstructTotalTransferFunction());

	if (totalTF.IsEmpty())
		return;

	ExpressionTree expressionTree;
	if (frequencyHertz)
		expressionTree.Solve(totalTF, minFreq, maxFreq, numberOfPoints, totalAmplitude, totalPhase);
	else
	{
		expressionTree.Solve(totalTF, minFreq * 0.5 / M_PI,
			maxFreq * 0.5 / M_PI, numberOfPoints, totalAmplitude, totalPhase);

		totalAmplitude.MultiplyXData(2.0 * M_PI);
		totalPhase.MultiplyXData(2.0 * M_PI);
	}

	totalNeedsUpdate = false;
}

void DataManager::RemoveAllTransferFunctions()
{
	amplitudePlots.Clear();
	phasePlots.Clear();
	transferFunctions.clear();

	// FIXME:  What about total?
}

void DataManager::RemoveTransferFunctions(const unsigned int &i)
{
	amplitudePlots.Remove(i);
	phasePlots.Remove(i);
	transferFunctions.erase(transferFunctions.begin() + i);

	totalNeedsUpdate = true;
	UpdateTotalTransferFunctionData();
}

void DataManager::UpdateAllTransferFunctionData()
{
	for (unsigned int i = 0; i < amplitudePlots.GetCount(); ++i)
		UpdateTransferFunction(i, transferFunctions[i].first, transferFunctions[i].second);

	UpdateTotalTransferFunctionData();
}

wxString DataManager::AssembleTransferFunctionString(const wxString &numerator, const wxString &denominator) const
{
	if (numerator.IsEmpty() && denominator.IsEmpty())
		return wxEmptyString;
	return "(" + numerator + ")/(" + denominator + ")";
}

wxString DataManager::AssembleTransferFunctionString(const std::pair<wxString, wxString> &tf) const
{
	return AssembleTransferFunctionString(tf.first, tf.second);
}

LibPlot2D::Dataset2D& DataManager::GetTotalAmplitudeData()
{
	UpdateTotalTransferFunctionData();
	return totalAmplitude;
}

LibPlot2D::Dataset2D& DataManager::GetTotalPhaseData()
{
	UpdateTotalTransferFunctionData();
	return totalPhase;
}
