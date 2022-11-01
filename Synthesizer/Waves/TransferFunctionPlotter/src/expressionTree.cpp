/*===================================================================================
                                TransferFunctionPlotter
                              Copyright Kerry R. Loux 2020

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  expressionTree.cpp
// Created:  5/6/2011
// Author:  K. Loux
// Description:  Handles user-specified mathematical operations on datasets.

// Local headers
#include "expressionTree.h"
#include "tfpMath.h"

// LibPlot2D headers
#include <lp2d/utilities/math/plotMath.h>

// wxWidgets headers
#include <wx/wx.h>

//==========================================================================
// Class:			ExpressionTree
// Function:		Solve
//
// Description:		Main solving method for the tree.
//
// Input Arguments:
//		expression	= wxString containing the expression to parse
//
// Output Arguments:
//		solvedData	= Dataset2D& containing the evaluated data
//
// Return Value:
//		wxString, empty for success, error string if unsuccessful
//
//==========================================================================
wxString ExpressionTree::Solve(wxString expression, const double &minFreq,
	const double &maxFreq, const unsigned int &resolution, LibPlot2D::Dataset2D &magnitude, LibPlot2D::Dataset2D &phase)
{
	if (!ParenthesesBalanced(expression))
		return _T("Imbalanced parentheses!");

	wxString errorString;
	errorString = ParseExpression(expression);

	if (!errorString.IsEmpty())
		return errorString;

	PrepareDataset(minFreq, maxFreq, resolution, magnitude, phase);

	errorString = EvaluateExpression();

	if (errorString.IsEmpty())
	{
		magnitude += TFPMath::GetMagnitudeData(dataVector);
		magnitude = TFPMath::ConvertToDecibels(magnitude);
		phase += TFPMath::GetPhaseData(dataVector);
	}

	return errorString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ParenthesesBalanced
//
// Description:		Checks to see if the expression has balanced parentheses.
//
// Input Arguments:
//		expression	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if parentheses are balanced, false otherwise
//
//==========================================================================
bool ExpressionTree::ParenthesesBalanced(const wxString &expression) const
{
	unsigned int leftCount(0), rightCount(0);
	int location = expression.find(_T("("));

	while (location != wxNOT_FOUND)
	{
		++leftCount;
		location = expression.find(_T("("), location + 1);
	}

	location = expression.find(_T(")"));

	while (location != wxNOT_FOUND)
	{
		++rightCount;
		location = expression.find(_T(")"), location + 1);
	}

	if (leftCount != rightCount)
		return false;

	return true;
}

wxString ExpressionTree::CheckExpression(wxString expression)
{
	return ParseExpression(expression);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ParseExpression
//
// Description:		Parses the expression and produces a queue of Reverse
//					Polish Notation values and operations.  Implements the
//					shunting-yard algorithm as described by Wikipedia.
//
// Input Arguments:
//		expression	= const wxString& to be parsed
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing error descriptions or an empty string on success
//
//==========================================================================
wxString ExpressionTree::ParseExpression(const wxString &expression)
{
	std::stack<wxString> operatorStack;
	bool lastWasOperator(true);
	wxString errorString;

	for (unsigned int i = 0; i < expression.Len(); ++i)
	{
		if (expression.Mid(i, 1).Trim().IsEmpty())
			continue;

		unsigned int advance;
		errorString = ParseNext(expression.Mid(i), lastWasOperator,
			advance, operatorStack);
		if (!errorString.IsEmpty())
			return errorString;
		i += advance - 1;
	}

	if (!EmptyStackToQueue(operatorStack))
		errorString = _T("Imbalanced parentheses!");

	return errorString;
}

//=============================================================================
// Class:			ExpressionTree
// Function:		ParseNext
//
// Description:		Parses the expression and processes the next item.
//
// Input Arguments:
//		expression		= const wxString&
//		lastWasOperator	= bool&
//		advance			= unsigned int&
//		operatorStack	= std::stack<wxString>&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing any errors
//
//=============================================================================
wxString ExpressionTree::ParseNext(const wxString &expression,
	bool &lastWasOperator, unsigned int &advance,
	std::stack<wxString> &operatorStack)
{
	bool thisWasOperator(false);
	if (NextIsNumber(expression, &advance, lastWasOperator))
		outputQueue.push(expression.Mid(0, advance));
	else if (NextIsS(expression, &advance))
		outputQueue.push(expression.Mid(0, advance));
	else if (NextIsOperator(expression, &advance))
	{
		ProcessOperator(operatorStack, expression.Mid(0, advance));
		thisWasOperator = true;
	}
	else if (expression[0] == '(')
	{
		if (!lastWasOperator)
			operatorStack.push(_T("*"));
		operatorStack.push(expression[0]);
		advance = 1;
		thisWasOperator = true;	}
	else if (expression[0] == ')')
	{
		ProcessCloseParenthese(operatorStack);
		advance = 1;
	}
	else
		return _T("Unrecognized character:  '") + expression.Mid(0, 1) + _T("'.");

	lastWasOperator = thisWasOperator;
	return wxEmptyString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PrepareDataset
//
// Description:		Sizes the dataset and populates the x-axis with frequency
//					data.  Frequencies are spaced logarithmically.
//
// Input Arguments:
//		minFreq		= const double& indicating the minimum frequency [Hz]
//		maxFreq		= const double& indicating the maximum frequency [Hz]
//		resolution	= const unsigned int& number of frequencies
//
// Output Arguments:
//		data		= Dataset2D&
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PrepareDataset(const double &minFreq, const double &maxFreq,
	const unsigned int &resolution, LibPlot2D::Dataset2D &magnitude, LibPlot2D::Dataset2D &phase)
{
	magnitude.Resize(resolution);
	dataVector.assign(resolution, LibPlot2D::Complex(0.0, 0.0));

	for (unsigned int i = 0; i < resolution; ++i)
	{
		magnitude.GetX()[i] = pow(10.0,
			(double)i / (double)(resolution - 1) * log10(maxFreq / minFreq) + log10(minFreq));
		magnitude.GetY()[i] = 0.0;
		dataVector[i] = LibPlot2D::Complex(0.0, magnitude.GetX()[i] * 2.0 * M_PI);
	}

	phase = magnitude;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ProcessOperator
//
// Description:		Processes the next operator in the expression, adding it
//					to the appropriate stack.  This method enforces the order
//					of operations.
//
// Input Arguments:
//		operatorStack	= std::stack<wxString>&
//		s				= const wxString& representing the next operator
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::ProcessOperator(std::stack<wxString> &operatorStack, const wxString &s)
{
	while (!operatorStack.empty())
	{
		if ((!NextIsOperator(operatorStack.top()) ||
			!OperatorShift(operatorStack.top(), s)))
			break;
		PopStackToQueue(operatorStack);
	}
	operatorStack.push(s);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ProcessCloseParenthese
//
// Description:		Adjusts the stacks in response to encountering a close
//					parenthese.
//
// Input Arguments:
//		operatorStack	= std::stack<wxString>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::ProcessCloseParenthese(std::stack<wxString> &operatorStack)
{
	while (!operatorStack.empty())
	{
		if (operatorStack.top().Cmp(_T("(")) == 0)
			break;
		PopStackToQueue(operatorStack);
	}

	if (operatorStack.empty())
	{
		assert(false);
		// Should never happen due to prior parenthese balance checks
		//return _T("Imbalanced parentheses!");
	}

	operatorStack.pop();
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateExpression
//
// Description:		Evaluates the expression in the queue using Reverse Polish
//					Notation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		results	= Dataset2D&
//
// Return Value:
//		wxString containing a description of any errors, or wxEmptyString on success
//
//==========================================================================
wxString ExpressionTree::EvaluateExpression()
{
	wxString next, errorString;

	std::stack<double> doubleStack;
	std::stack<std::vector<LibPlot2D::Complex>> vectorStack;
	std::stack<bool> useDoubleStack;

	while (!outputQueue.empty())
	{
		next = outputQueue.front();
		outputQueue.pop();

		if (!EvaluateNext(next, doubleStack, vectorStack, useDoubleStack, errorString))
			return errorString;
	}

	if (useDoubleStack.size() > 1)
		return _T("Not enough operators!");
	if (useDoubleStack.top())
		return _T("Expression evaluates to a number!");
	else
		dataVector = vectorStack.top();

	return wxEmptyString;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PopStackToQueue
//
// Description:		Removes the top entry of the stack and puts it in the queue.
//
// Input Arguments:
//		stack	= std::stack<wxString>& to be popped
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PopStackToQueue(std::stack<wxString> &stack)
{
	outputQueue.push(stack.top());
	stack.pop();
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EmptyStackToQueue
//
// Description:		Empties the contents of the stack into the queue.
//
// Input Arguments:
//		stack	= std::stack<wxString>& to be emptied
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise (imbalance parentheses)
//
//==========================================================================
bool ExpressionTree::EmptyStackToQueue(std::stack<wxString> &stack)
{
	while (!stack.empty())
	{
		if (stack.top().Cmp(_T("(")) == 0)
			return false;
		PopStackToQueue(stack);
	}

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsNumber
//
// Description:		Determines if the next portion of the expression is a number.
//
// Input Arguments:
//		s				= const wxString& containing the expression
//		lastWasOperator	= const bool& indicating whether or not the last thing
//						  on the stack is an operator
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of number
//
// Return Value:
//		bool, true if a number is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsNumber(const wxString &s, unsigned int *stop,
	const bool &lastWasOperator) const
{
	if (s.Len() == 0)
		return false;

	bool foundDecimal = s[0] == '.';
	if (foundDecimal ||
		(int(s[0]) >= int('0') && int(s[0]) <= int('9')) ||
		(s[0] == '-' && lastWasOperator && NextIsNumber(s.Mid(1), nullptr, false)))
	{
		unsigned int i;
		for (i = 1; i < s.Len(); ++i)
		{
			if (s[i] == '.')
			{
				if (foundDecimal)
					return false;
				foundDecimal = true;
			}
			else if (int(s[i]) < int('0') || int(s[i]) > int('9'))
				break;
		}

		if (stop)
			*stop = i;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsOperator
//
// Description:		Determines if the next portion of the expression is an operator.
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of operator
//
// Return Value:
//		bool, true if an operator is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsOperator(const wxString &s, unsigned int *stop) const
{
	if (s.Len() == 0)
		return false;

	if (s[0] == '+' ||// From least precedence
		s[0] == '-' ||
		s[0] == '*' ||
		s[0] == '/' ||
		s[0] == '^')// To most precedence
	{
		if (stop)
			*stop = 1;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		NextIsS
//
// Description:		Determines if the next portion of the expression is "s" (or "S").
//
// Input Arguments:
//		s		= const wxString& containing the expression
//
// Output Arguments:
//		stop	= unsigned int* (optional) indicating length of operator
//
// Return Value:
//		bool, true if "s" is next in the expression
//
//==========================================================================
bool ExpressionTree::NextIsS(const wxString &s, unsigned int *stop) const
{
	if (s.Len() == 0)
		return false;

	if (s[0] == 's' || s[0] == 'S')
	{
		if (stop)
			*stop = 1;
		return true;
	}

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		OperatorShift
//
// Description:		Determines if the new operator requires a shift in
//					operator placement.
//
// Input Arguments:
//		stackString	= const wxString& containing the expression
//		newString	= const wxString& containing the expression
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if shifting needs to occur
//
//==========================================================================
bool ExpressionTree::OperatorShift(const wxString &stackString, const wxString &newString) const
{
	unsigned int stackPrecedence = GetPrecedence(stackString);
	unsigned int newPrecedence = GetPrecedence(newString);

	if (stackPrecedence == 0 || newPrecedence == 0)
		return false;

	if (IsLeftAssociative(newString[0]))
	{
		if (newPrecedence <= stackPrecedence)
			return true;
	}
	else if (newPrecedence < stackPrecedence)
		return true;

	return false;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		GetPrecedence
//
// Description:		Determines the precedence of the specified operator
//					(higher values are performed first)
//
// Input Arguments:
//		s	= const wxString& containing the operator
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int representing the precedence
//
//==========================================================================
unsigned int ExpressionTree::GetPrecedence(const wxString &s) const
{
	if (s.Len() != 1)
		return 0;

	if (s[0] == '+' ||
		s[0] == '-')
		return 2;
	else if (s[0] == '*' ||
		s[0] == '/')
		return 3;
	else if (s[0] == '^')
		return 4;

	return 0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		IsLeftAssociative
//
// Description:		Determines if the specified operator is left or right
//					associative.
//
// Input Arguments:
//		c	= const wxChar&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if left associative
//
//==========================================================================
bool ExpressionTree::IsLeftAssociative(const wxChar &c) const
{
	switch (c)
	{
	case '^':
		return false;

	default:
		return true;
	}
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PushToStack
//
// Description:		Pushes the specified value onto the stack.
//
// Input Arguments:
//		value			= const double&
//		doubleStack		= std::stack<double>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PushToStack(const double &value, std::stack<double> &doubleStack,
	std::stack<bool> &useDoubleStack) const
{
	doubleStack.push(value);
	useDoubleStack.push(true);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PushToStack
//
// Description:		Pushes the specified dataset onto the stack.
//
// Input Arguments:
//		vector			= const std::vector<LibPlot2D::Complex>&
//		vectorStack		= std::stack<std::vector<LibPlot2D::Complex>>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ExpressionTree::PushToStack(const std::vector<LibPlot2D::Complex> &vector, std::stack<std::vector<LibPlot2D::Complex>> &vectorStack,
	std::stack<bool> &useDoubleStack) const
{
	vectorStack.push(vector);
	useDoubleStack.push(false);
}

//==========================================================================
// Class:			ExpressionTree
// Function:		PopFromStack
//
// Description:		Pops the next value from the top of the appropriate stack.
//
// Input Arguments:
//		doubleStack		= std::stack<double>&
//		vectorStack		= std::stack<std::vector<LibPlot2D::Complex>>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		value			= double&
//		vector			= std::vector<LibPlot2D::Complex>&
//
// Return Value:
//		bool, true if a double was popped, false if a dataset was popped
//
//==========================================================================
bool ExpressionTree::PopFromStack(std::stack<double> &doubleStack, std::stack<std::vector<LibPlot2D::Complex>> &vectorStack,
	std::stack<bool> &useDoubleStack, double &value, std::vector<LibPlot2D::Complex> &vector) const
{
	assert(!useDoubleStack.empty());

	bool useDouble = useDoubleStack.top();
	useDoubleStack.pop();

	if (useDouble)
	{
		assert(!doubleStack.empty());
		value = doubleStack.top();
		doubleStack.pop();
	}
	else
	{
		assert(!vectorStack.empty());
		vector = vectorStack.top();
		vectorStack.pop();
	}

	return useDouble;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const double&
//		second		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the result of the operation
//
//==========================================================================
double ExpressionTree::ApplyOperation(const wxString &operation,
	const double &first, const double &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return second + first;
	else if (operation.Cmp(_T("-")) == 0)
		return second - first;
	else if (operation.Cmp(_T("*")) == 0)
		return second * first;
	else if (operation.Cmp(_T("/")) == 0)
		return second / first;
	else if (operation.Cmp(_T("^")) == 0)
		return pow(second, first);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const const std::vector<LibPlot2D::Complex>&
//		second		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the result of the operation
//
//==========================================================================
std::vector<LibPlot2D::Complex> ExpressionTree::ApplyOperation(const wxString &operation,
	const std::vector<LibPlot2D::Complex> &first, const double &second) const
{
	std::vector<LibPlot2D::Complex> vector(first.size(), LibPlot2D::Complex(second, 0.0));
	if (operation.Cmp(_T("+")) == 0)
		return TFPMath::operator+<LibPlot2D::Complex>(vector, first);
	else if (operation.Cmp(_T("-")) == 0)
		return TFPMath::operator-<LibPlot2D::Complex>(vector, first);
	else if (operation.Cmp(_T("*")) == 0)
		return TFPMath::operator*<LibPlot2D::Complex>(vector, first);
	else if (operation.Cmp(_T("/")) == 0)
		return TFPMath::operator/<LibPlot2D::Complex>(vector, first);
	else if (operation.Cmp(_T("^")) == 0)
	{
		std::vector<LibPlot2D::Complex> result(first.size());
		for (unsigned int i = 0; i < result.size(); ++i)
			result[i] = vector[i].ToPower(first[i]);
		return result;
	}

	assert(false);
	return vector;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const double&
//		second		= const std::vector<LibPlot2D::Complex>&
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the result of the operation
//
//==========================================================================
std::vector<LibPlot2D::Complex> ExpressionTree::ApplyOperation(const wxString &operation,
	const double &first, const std::vector<LibPlot2D::Complex> &second) const
{
	std::vector<LibPlot2D::Complex> vector(second.size(), LibPlot2D::Complex(first, 0.0));
	if (operation.Cmp(_T("+")) == 0)
		return TFPMath::operator+<LibPlot2D::Complex>(second, vector);
	else if (operation.Cmp(_T("-")) == 0)
		return TFPMath::operator-<LibPlot2D::Complex>(second, vector);
	else if (operation.Cmp(_T("*")) == 0)
		return TFPMath::operator*<LibPlot2D::Complex>(second, vector);
	else if (operation.Cmp(_T("/")) == 0)
		return TFPMath::operator/<LibPlot2D::Complex>(second, vector);
	else if (operation.Cmp(_T("^")) == 0)
	{
		std::vector<LibPlot2D::Complex> result(second.size());
		for (unsigned int i = 0; i < result.size(); ++i)
			result[i] = second[i].ToPower(vector[i]);
		return result;
	}

	assert(false);
	return vector;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		ApplyOperation
//
// Description:		Applies the specified operation to the specified operands.
//
// Input Arguments:
//		operation	= const wxString& describing the function to apply
//		first		= const const std::vector<LibPlot2D::Complex>&
//		second		= const const std::vector<LibPlot2D::Complex>&
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the result of the operation
//
//==========================================================================
std::vector<LibPlot2D::Complex> ExpressionTree::ApplyOperation(const wxString &operation,
	const std::vector<LibPlot2D::Complex> &first, const std::vector<LibPlot2D::Complex> &second) const
{
	if (operation.Cmp(_T("+")) == 0)
		return TFPMath::operator+<LibPlot2D::Complex>(second, first);
	else if (operation.Cmp(_T("-")) == 0)
		return TFPMath::operator-<LibPlot2D::Complex>(second, first);
	else if (operation.Cmp(_T("*")) == 0)
		return TFPMath::operator*<LibPlot2D::Complex>(second, first);
	else if (operation.Cmp(_T("/")) == 0)
		return TFPMath::operator/<LibPlot2D::Complex>(second, first);
	else if (operation.Cmp(_T("^")) == 0)
	{
		std::vector<LibPlot2D::Complex> result(first.size());
		for (unsigned int i = 0; i < result.size(); ++i)
			result[i] = second[i].ToPower(first[i]);
		return result;
	}

	assert(false);
	return first;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateOperator
//
// Description:		Evaluates the operator specified.
//
// Input Arguments:
//		operator		= const wxString& describing the function to apply
//		doubleStack		= std::stack<double>&
//		vectorStack		= std::stack<std::vector<Complex>>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateOperator(const wxString &operation, std::stack<double> &doubleStack,
	std::stack<std::vector<LibPlot2D::Complex>> &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	double value1, value2;
	std::vector<LibPlot2D::Complex> vector1, vector2;

	if (useDoubleStack.size() < 2)
		return EvaluateUnaryOperator(operation, doubleStack, vectorStack, useDoubleStack, errorString);
	else if (PopFromStack(doubleStack, vectorStack, useDoubleStack, value1, vector1))
	{
		if (PopFromStack(doubleStack, vectorStack, useDoubleStack, value2, vector2))
			PushToStack(ApplyOperation(operation, value1, value2), doubleStack, useDoubleStack);
		else
			PushToStack(ApplyOperation(operation, value1, vector2), vectorStack, useDoubleStack);
	}
	else if (PopFromStack(doubleStack, vectorStack, useDoubleStack, value2, vector2))
		PushToStack(ApplyOperation(operation, vector1, value2), vectorStack, useDoubleStack);
	else
		PushToStack(ApplyOperation(operation, vector1, vector2), vectorStack, useDoubleStack);
	return true;
}

//=============================================================================
// Class:			ExpressionTree
// Function:		EvaluateUnaryOperator
//
// Description:		Evaluates the operator specified.  The only unary operator
//					we recognize is minus (negation).
//
// Input Arguments:
//		operator		= const wxString& describing the function to apply
//		doubleStack		= std::stack<double>&
//		vectorStack		= std::stack<std::vector<LibPlot2D::Complex>>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool ExpressionTree::EvaluateUnaryOperator(const wxString &operation, std::stack<double> &doubleStack,
	std::stack<std::vector<LibPlot2D::Complex>> &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	if (operation.Cmp(_T("-")) != 0)
	{
		errorString = _T("Attempting to apply operator without two operands!");
		return false;
	}

	double value;
	std::vector<LibPlot2D::Complex> vector;
	if (PopFromStack(doubleStack, vectorStack, useDoubleStack, value, vector))
		PushToStack(ApplyOperation(_T("*"), -1.0, value), doubleStack, useDoubleStack);
	else
		PushToStack(ApplyOperation(_T("*"), -1.0, vector), vectorStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateNumber
//
// Description:		Evaluates the number specified.
//
// Input Arguments:
//		number			= const wxString& describing the number
//		doubleStack		= std::stack<double>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString		= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateNumber(const wxString &number, std::stack<double> &doubleStack,
	std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	double value;

	if (!number.ToDouble(&value))
	{
		errorString = _T("Could not convert ") + number + _T(" to a number.");
		return false;
	}

	PushToStack(value, doubleStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateS
//
// Description:		Evaluates the frequency specified.
//
// Input Arguments:
//		vectorStack		= std::stack<std::vector<LibPlot2D::Complex>>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateS(std::stack<std::vector<LibPlot2D::Complex>> &vectorStack,
	std::stack<bool> &useDoubleStack) const
{
	PushToStack(dataVector, vectorStack, useDoubleStack);

	return true;
}

//==========================================================================
// Class:			ExpressionTree
// Function:		EvaluateNext
//
// Description:		Determines how to evaluate the specified term and takes
//					appropriate action.
//
// Input Arguments:
//		next			= const wxString&
//		doubleStack		= std::stack<double>&
//		vectorStack		= std::stack<std::vector<LibPlot2D::Complex>>&
//		useDoubleStack	= std::stack<bool>&
//
// Output Arguments:
//		errorString	= wxString&
//
// Return Value:
//		bool, true for valid operation, false otherwise
//
//==========================================================================
bool ExpressionTree::EvaluateNext(const wxString &next, std::stack<double> &doubleStack,
		std::stack<std::vector<LibPlot2D::Complex>> &vectorStack, std::stack<bool> &useDoubleStack, wxString &errorString) const
{
	if (NextIsNumber(next))
		return EvaluateNumber(next, doubleStack, useDoubleStack, errorString);
	if (NextIsOperator(next))
		return EvaluateOperator(next, doubleStack, vectorStack, useDoubleStack, errorString);
	if (NextIsS(next))
		return EvaluateS(vectorStack, useDoubleStack);
	else
		errorString = _T("Unable to evaluate '") + next + _T("'.");

	return false;
}
