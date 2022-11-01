/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  expressionTree.h
// Created:  5/6/2011
// Author:  K. Loux
// Description:  Handles user-specified mathematical operations on datasets.

#ifndef _EXPRESSION_TREE_H_
#define _EXPRESSION_TREE_H_

// LibPlot2D headers
#include <lp2d/utilities/managedList.h>
#include <lp2d/utilities/dataset2D.h>
#include <lp2d/utilities/math/complex.h>

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <queue>
#include <stack>

class ExpressionTree
{
public:
	// Main solver method
	wxString Solve(wxString expression, const double &minFreq, const double &maxFreq,
		const unsigned int &resolution, LibPlot2D::Dataset2D &magnitude, LibPlot2D::Dataset2D &phase);

	wxString CheckExpression(wxString expression);

private:
	std::queue<wxString> outputQueue;
	std::vector<LibPlot2D::Complex> dataVector;

	void PrepareDataset(const double &minFreq, const double &maxFreq,
		const unsigned int &resolution, LibPlot2D::Dataset2D &magnitude, LibPlot2D::Dataset2D &phase);

	wxString ParseExpression(const wxString &expression);
	wxString ParseNext(const wxString &expression, bool &lastWasOperator,
		unsigned int &advance, std::stack<wxString> &operatorStack);
	wxString EvaluateExpression();

	void ProcessOperator(std::stack<wxString> &operatorStack, const wxString &s);
	void ProcessCloseParenthese(std::stack<wxString> &operatorStack);

	bool NextIsNumber(const wxString &s, unsigned int *stop = nullptr, const bool &lastWasOperator = true) const;
	bool NextIsOperator(const wxString &s, unsigned int *stop = nullptr) const;
	bool NextIsS(const wxString &s, unsigned int *stop = nullptr) const;

	bool IsLeftAssociative(const wxChar &c) const;
	bool OperatorShift(const wxString &stackString, const wxString &newString) const;

	void PopStackToQueue(std::stack<wxString> &stack);
	bool EmptyStackToQueue(std::stack<wxString> &stack);
	unsigned int GetPrecedence(const wxString &s) const;

	void PushToStack(const double &value, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack) const;
	void PushToStack(const std::vector<LibPlot2D::Complex> &vector, std::stack<std::vector<LibPlot2D::Complex>> &vectorStack,
		std::stack<bool> &useDoubleStack) const;
	bool PopFromStack(std::stack<double> &doubleStack, std::stack<std::vector<LibPlot2D::Complex>> &vectorStack,
		std::stack<bool> &useDoubleStack, double &value, std::vector<LibPlot2D::Complex> &vector) const;

	double ApplyOperation(const wxString &operation, const double &first, const double &second) const;
	std::vector<LibPlot2D::Complex> ApplyOperation(const wxString &operation, const std::vector<LibPlot2D::Complex> &first, const double &second) const;
	std::vector<LibPlot2D::Complex> ApplyOperation(const wxString &operation, const double &first, const std::vector<LibPlot2D::Complex> &second) const;
	std::vector<LibPlot2D::Complex> ApplyOperation(const wxString &operation, const std::vector<LibPlot2D::Complex> &first,
		const std::vector<LibPlot2D::Complex> &second) const;

	bool EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<std::vector<LibPlot2D::Complex>> &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<std::vector<LibPlot2D::Complex>> &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateUnaryOperator(const wxString &operation, std::stack<double> &doubleStack,
		std::stack<std::vector<LibPlot2D::Complex>> &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateNumber(const wxString &number, std::stack<double> &doubleStack,
		std::stack<bool> &useDoubleStack, wxString &errorString) const;
	bool EvaluateS(std::stack<std::vector<LibPlot2D::Complex>> &vectorStack,
		std::stack<bool> &useDoubleStack) const;

	bool ParenthesesBalanced(const wxString &expression) const;
};

#endif// _EXPRESSION_TREE_H_
