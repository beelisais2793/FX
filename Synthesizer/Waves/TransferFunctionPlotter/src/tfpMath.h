/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tfpMath.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.

#ifndef _PLOT_MATH_H_
#define _PLOT_MATH_H_

// Standard C++ headers
#include <vector>

// LibPlot2D forward declarations
namespace LibPlot2D
{
class Dataset2D;
class Complex;
}

namespace TFPMath
{
	LibPlot2D::Dataset2D GetMagnitudeData(const std::vector<LibPlot2D::Complex> &data);
	LibPlot2D::Dataset2D GetPhaseData(const std::vector<LibPlot2D::Complex> &data);

	LibPlot2D::Dataset2D ConvertToDecibels(const LibPlot2D::Dataset2D &data);

	template<typename T>
	std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b);
	template<typename T>
	std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b);
	template<typename T>
	std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b);
	template<typename T>
	std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b);
}

template<typename T>
std::vector<T> TFPMath::operator+(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());
	for (unsigned int i = 0; i < result.size(); ++i)
		result[i] = a[i] + b[i];

	return result;
}

template<typename T>
std::vector<T> TFPMath::operator-(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());
	for (unsigned int i = 0; i < result.size(); ++i)
		result[i] = a[i] - b[i];

	return result;
}

template<typename T>
std::vector<T> TFPMath::operator*(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());
	for (unsigned int i = 0; i < result.size(); ++i)
		result[i] = a[i] * b[i];

	return result;
}

template<typename T>
std::vector<T> TFPMath::operator/(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> result(a.size());
	for (unsigned int i = 0; i < result.size(); ++i)
		result[i] = a[i] / b[i];

	return result;
}

#endif// _TFP_MATH_H_
