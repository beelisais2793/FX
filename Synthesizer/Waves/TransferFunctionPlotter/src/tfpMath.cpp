/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotMath.cpp
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.

// Local headers
#include "tfpMath.h"

// LibPlot2D headers
#include <lp2d/utilities/dataset2D.h>
#include <lp2d/utilities/math/complex.h>
#include <lp2d/utilities/math/plotMath.h>

//==========================================================================
// Namespace:		TFPMath
// Function:		GetMagnitudeData
//
// Description:		Creates a dataset containing the magnitude of the specified
//					complex-valued vector.  Note that the x-component of the
//					dataset will be uninitialized.
//
// Input Arguments:
//		data	= const std::vector<LibPlot2D::Complex>&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D
//
//==========================================================================
LibPlot2D::Dataset2D TFPMath::GetMagnitudeData(const std::vector<LibPlot2D::Complex> &data)
{
	LibPlot2D::Dataset2D result(data.size());
	for (unsigned int i = 0; i < result.GetNumberOfPoints(); ++i)
		result.GetY()[i] = sqrt(data[i].mReal * data[i].mReal + data[i].mImaginary * data[i].mImaginary);

	return result;
}

//==========================================================================
// Namespace:		TFPMath
// Function:		GetPhaseData
//
// Description:		Creates a dataset containing the phase of the specified
//					complex-valued vector.  Note that the x-component of the
//					dataset will be uninitialized.
//
// Input Arguments:
//		data	= const std::vector<LibPlot2D::Complex>&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D
//
//==========================================================================
LibPlot2D::Dataset2D TFPMath::GetPhaseData(const std::vector<LibPlot2D::Complex> &data)
{
	LibPlot2D::Dataset2D result(data.size());
	for (unsigned int i = 0; i < result.GetNumberOfPoints(); ++i)
		result.GetY()[i] = atan2(data[i].mImaginary, data[i].mReal);

	LibPlot2D::PlotMath::Unwrap(result);
	result *= 180.0 / M_PI;

	return result;
}

//==========================================================================
// Namespace:		TFPMath
// Function:		ConvertToDecibels
//
// Description:		Converts the specified magnitude data to decibels.
//
// Input Arguments:
//		data	= const LibPlot2D::Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D
//
//==========================================================================
LibPlot2D::Dataset2D TFPMath::ConvertToDecibels(const LibPlot2D::Dataset2D &data)
{
	LibPlot2D::Dataset2D results(data);
	for (unsigned int i = 0; i < results.GetNumberOfPoints(); ++i)
		results.GetY()[i] = 20.0 * log10(data.GetY()[i]);

	return results;
}
