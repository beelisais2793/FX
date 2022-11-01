#pragma once
#include <complex>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <initializer_list>

namespace TF
{
	template <int terms>
	class Symbolic_eqn
	{
	private:
		std::string symbolicEquation[terms];

	public:
		Symbolic_eqn() = delete;
		Symbolic_eqn(const Symbolic_eqn& eqn) = delete;

		Symbolic_eqn(std::initializer_list<std::string> eqnTerms)
		{
			int i{ 0 };
			for (const auto& term : eqnTerms)
			{
				symbolicEquation[i] = term;
				++i;
			}
		}

		std::string& operator[](int i) { return symbolicEquation[i]; }
	};

	class Variable
	{
	private:
		char m_name;
		double m_value;

	public:
		Variable() = default;
		//Variable(const Variable& var) = delete;

		Variable(const char& name, const double& value) : m_name{ name }, m_value{ value }
		{}

		const char& name() const { return m_name; }
		const double& value() const { return m_value; }
	};

	template <int variables>
	class Variables
	{
	private:
		Variable m_variables[variables];

	public:
		Variables() = delete;
		//Variables(const Variables& var) = delete;

		Variables(std::initializer_list<Variable> vars)
		{
			int i{ 0 };
			for (const auto& var : vars)
			{
				m_variables[i] = var;
				++i;
			}
		}

		Variable& operator[](int i) { return m_variables[i]; }
	};

	class TransferFunction
	{
	private:
		std::vector<double> m_numerator;
		std::vector<double> m_denominator;

	public:
		TransferFunction() = delete;

		TransferFunction(std::vector<double> numerator, std::vector<double> denominator)
			: m_numerator{ numerator }, m_denominator{ denominator }
		{}

		friend std::ostream& operator<< (std::ostream& out, const TransferFunction& tf);

		void bode(int lowerBound = -1, int upperBound = 7);
		void print() { std::cout << *this; };
	};

	class Total
	{
	private:
		double m_total{ 0 };

	public:
		Total() = default;

		Total& add(const double& x, const double& y)
		{
			double temp{ x + y };
			m_total += temp;
			return *this;
		}

		Total& subtract(const double& x, const double& y)
		{
			double temp{ x - y };
			m_total += temp;
			return *this;
		}

		Total& multiply(const double& x, const double& y)
		{
			double temp{ x * y };
			m_total += temp;
			return *this;
		}

		Total& divide(const double& x, const double& y)
		{
			double temp{ x / y };
			m_total += temp;
			return *this;
		}

		Total& power(const double& base, const double& exponent)
		{
			double temp{ pow(base, exponent) };
			m_total += temp;
			return *this;
		}

		Total& add(const double& x)
		{
			m_total += x;
			return *this;
		}

		Total& subtract(const double& x)
		{
			m_total -= x;
			return *this;
		}

		Total& multiply(const double& x)
		{
			m_total *= x;
			return *this;
		}

		Total& divide(const double& x)
		{
			m_total /= x;
			return *this;
		}

		const double& value() const { return m_total; }
		void clear() { m_total = 0; }
	};

	template <int variables>
	double strtonum(std::string str, Variables<variables>& vars)
	{
		if (std::isdigit(str[0]) != 0)
			return std::stoi(str);
		for (int i{ 0 }; i < variables; ++i)
		{
			if (str == std::string(1, vars[i].name()))
			{
				return vars[i].value();
			}
		}
		return 0;
	}

	int findLength(const std::vector<double>& eqn);
	void find_parenthesis(std::string str, std::vector<int>& usedIndices);
	int searchOperator(std::string& str, const char& chosenOperator);

	template<int variables>
	void operate(std::string& substring, int point, const char& chosenOperator, Variables<variables>& vars,
		Total& runningTotal, std::vector<int>& usedIndices, const int parenthesisCheck, std::vector<double>& parenthesisValues)
	{
		double temp_one, temp_two;
		bool pOneFail{ false }, pTwoFail{ false };

		if (std::find(usedIndices.begin(), usedIndices.end(), (point - 1)) != usedIndices.end())
		{
			if (usedIndices.at(std::find(usedIndices.begin(), usedIndices.end(), (point - 1)) - usedIndices.begin()) != std::numeric_limits<int>::max())
			{
				for (int i{ 0 }; i < parenthesisCheck; ++i)
				{
					if (std::find(usedIndices.begin(), usedIndices.end(), (point - 1)) == usedIndices.begin() + i)
					{
						pOneFail = true;
						if (i % 2)
						{
							temp_one = parenthesisValues.at((i - 1) / 2);
							usedIndices[i] = std::numeric_limits<int>::max();
							usedIndices[i - 1] = std::numeric_limits<int>::max();
						}
						else
						{
							temp_one = parenthesisValues.at(i / 2);
							usedIndices[i] = std::numeric_limits<int>::max();
							usedIndices[i + 1] = std::numeric_limits<int>::max();
						}
					}
				}
				if (!pOneFail)
				{
					temp_one = runningTotal.value();
					runningTotal.clear();
				}
			}
			else
			{
				temp_one = runningTotal.value();
				runningTotal.clear();
			}
		}
		else
		{
			temp_one = strtonum(std::string(1, substring.at(point - 1)), vars);
			usedIndices.push_back(point - 1);
		}

		if (std::find(usedIndices.begin(), usedIndices.end(), (point + 1)) != usedIndices.end())
		{
			if (usedIndices.at(std::find(usedIndices.begin(), usedIndices.end(), (point + 1)) - usedIndices.begin()) != std::numeric_limits<int>::max())
			{
				for (int i{ 0 }; i < parenthesisCheck; ++i)
				{
					if (std::find(usedIndices.begin(), usedIndices.end(), (point + 1)) == usedIndices.begin() + i)
					{
						pTwoFail = true;
						if (i % 2)
						{
							temp_two = parenthesisValues.at((i - 1) / 2);
							usedIndices[i] = std::numeric_limits<int>::max();
							usedIndices[i - 1] = std::numeric_limits<int>::max();
						}
						else
						{
							temp_two = parenthesisValues.at(i / 2);
							usedIndices[i] = std::numeric_limits<int>::max();
							usedIndices[i + 1] = std::numeric_limits<int>::max();
						}

					}
				}
				if (!pTwoFail)
				{
					temp_two = runningTotal.value();
					runningTotal.clear();
				}
			}
			else
			{
				temp_two = runningTotal.value();
				runningTotal.clear();
			}
		}
		else
		{
			temp_two = strtonum(std::string(1, substring.at(point + 1)), vars);
			usedIndices.push_back(point + 1);
		}

		if (std::find(usedIndices.begin(), usedIndices.end(), point) != usedIndices.end())
			usedIndices.push_back(point);

		switch (chosenOperator)
		{
		case '^':
			runningTotal.power(temp_one, temp_two);
			break;
		case '*':
			runningTotal.multiply(temp_one, temp_two);
			break;
		case '/':
			runningTotal.divide(temp_one, temp_two);
			break;
		case '+':
			runningTotal.add(temp_one, temp_two);
			break;
		case '-':
			runningTotal.subtract(temp_one, temp_two);
			break;
		}
	}

	template<int terms, int variables>
	std::vector<double> numeric(Symbolic_eqn<terms>& eqn, Variables<variables>& vars)
	{
		std::vector<double> numeric_eqn;
		const char operators[5]{ '^', '*', '/', '+', '-' };

		for (int i_terms{ 0 }; i_terms < terms; ++i_terms)
		{
			Total runningTotal;
			std::string substring{ eqn[i_terms] };
			substring.erase(std::remove_if(substring.begin(), substring.end(), isspace), substring.end());
			std::vector<int> usedIndices;
			std::vector<double> parenthesisValues;
			int start{ 0 }, end{ 0 };
			bool firstCheck{ true };

			if (substring.length() == 1)
			{
				numeric_eqn.push_back(strtonum(substring, vars));
			}
			else if (substring != "")
			{
				find_parenthesis(substring, usedIndices);

				int totalParenthesis{ static_cast<int>(usedIndices.size()) };

				for (int sub{ 0 }; sub < totalParenthesis; (sub += 2))
				{
					std::string parenthesisSubstring = substring.substr((usedIndices[sub] + 1), ((usedIndices[sub + 1] - 1) - usedIndices[sub]));

					for (int i{ 0 }; i < 5; ++i)
					{
						int point{ searchOperator(parenthesisSubstring, operators[i]) };

						if (point == 0)
							throw std::runtime_error("Invalid symbolic equation.");
						else if (point == -1)
							continue;
						else
						{
							parenthesisSubstring[point] = '~';
							point += (usedIndices[sub] + 1);
							operate(substring, point, operators[i], vars, runningTotal, usedIndices, totalParenthesis, parenthesisValues);
							substring[point] = '~';
							i = 0;
						}
					}
					parenthesisValues.push_back(runningTotal.value());
					runningTotal.clear();
				}

				for (int i{ 0 }; i < 5; ++i)
				{
					int point{ searchOperator(substring, operators[i]) };
					if (point == 0)
						throw std::runtime_error("Invalid symbolic equation.");
					else if (point == -1)
						continue;
					else
					{
						operate(substring, point, operators[i], vars, runningTotal, usedIndices, totalParenthesis, parenthesisValues);
						substring[point] = '~';
						i = 0;
					}
				}

				if (runningTotal.value() == 0 && parenthesisValues.size() > 0)
				{
					size_t length{ parenthesisValues.size() - 1 };
					runningTotal.add(parenthesisValues[length]);
				}

				numeric_eqn.push_back(runningTotal.value());
			}
			else
				numeric_eqn.push_back(0);
		}
		return numeric_eqn;
	}

	template<int numerator_terms, int denominator_terms, int total_vars>
	TransferFunction transferFunction(Symbolic_eqn<numerator_terms>& numerator,
		Symbolic_eqn<denominator_terms>& denominator, Variables<total_vars>& vars)
	{
		std::vector<double> numeric_numerator = numeric(numerator, vars);
		std::vector<double> numeric_denominator = numeric(denominator, vars);

		return TransferFunction{ numeric_numerator, numeric_denominator };
	}
}