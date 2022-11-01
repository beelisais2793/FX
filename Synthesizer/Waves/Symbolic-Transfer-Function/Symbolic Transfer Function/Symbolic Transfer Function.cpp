#include "pch.h"
#include "framework.h"
#include "TransferFunction.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <complex>
#include <vector>
#include <cstring>
#include <string>
#include <limits>
#include <cmath>
#include <cctype>

namespace TF
{
	int findLength(const std::vector<double>& eqn)
	{
		int length{ 0 };

		for (int i{ 0 }; i < eqn.size(); ++i)
		{
			length += 3;
			for (int j{ 0 }; j < std::to_string(eqn[i]).length(); ++j)
				++length;
		}
		length += eqn.size();

		return length;
	}

	std::ostream& operator<< (std::ostream& out, const TransferFunction& tf)
	{
		int numeratorLength{ findLength(tf.m_numerator) };
		int denominatorLength{ findLength(tf.m_denominator) };
		int difference{ denominatorLength - numeratorLength };

		if (difference >= 0)
		{
			for (int space{ 0 }; space < difference / 2; ++space)
				out << ' ';

			for (int i{ 0 }; i < tf.m_numerator.size(); ++i)
				out << tf.m_numerator[i] << "s^" << i << ' ';
			out << '\n';

			for (int dash{ 0 }; dash < denominatorLength; ++dash)
				out << '-';

			std::cout << '\n';

			for (int i{ 0 }; i < tf.m_denominator.size(); ++i)
				out << tf.m_denominator[i] << "s^" << i << ' ';
			out << '\n';
		}
		else
		{
			for (int i{ 0 }; i < tf.m_numerator.size(); ++i)
				out << tf.m_numerator[i] << "s^" << i << ' ';
			out << '\n';

			for (int dash{ 0 }; dash < numeratorLength; ++dash)
				out << '-';

			std::cout << '\n';

			for (int space{ 0 }; space < -difference / 2; ++space)
				out << ' ';

			for (int i{ 0 }; i < tf.m_denominator.size(); ++i)
				out << tf.m_denominator[i] << "s^" << i << ' ';
			out << '\n';
		}

		return out;
	}

	void TransferFunction::bode(int lowerBound, int upperBound)
	{
		constexpr double pi{ 3.1415926535897932384626 };
		std::vector<std::complex<double>> bodeMagSolution;
		std::vector<std::complex<double>> bodePhaseSolution;
		std::vector<std::complex<double>> freqResponse;
		std::vector<double> w;
		std::vector<std::complex<double>> jw;

		int iterations{ ((upperBound - lowerBound) * 100) + 1 };
		for (int i{ 0 }; i < iterations; ++i)
		{
			w.push_back(std::pow(10, (lowerBound + (0.01 * i))));
			jw.push_back(std::complex<double>(0.0, w[i]));
			std::complex<double> cnumerator, cdenominator;

			for (int j{ 0 }; j < this->m_numerator.size(); ++j)
				cnumerator += (this->m_numerator[j] * std::pow(jw[i], j));
			for (int k{ 0 }; k < this->m_denominator.size(); ++k)
				cdenominator += (this->m_denominator[k] * std::pow(jw[i], k));

			freqResponse.push_back(cnumerator / cdenominator);
			bodeMagSolution.push_back(20 * std::log10(std::abs(cnumerator / cdenominator)));
			bodePhaseSolution.push_back(std::arg(cnumerator / cdenominator) * (180 / pi));
		}

		std::ofstream outf{ "bode.csv" };

		if (!outf)
			std::cerr << "bode.csv could not be opened." << std::endl;

		outf << "w" << ',' << "G(jw){Re}" << ',' << "G(jw){Im}" << ',' << "|G(jw)|{Re}" << ',' << "|G(jw)|{Im}" << ',' << "<G(jw){Re}" << ',' << "<G(jw){Im}" << '\n';
		std::size_t wsize{ w.size() };
		for (std::size_t x{ 0 }; x < wsize; ++x)
			outf << w[x] << ',' << freqResponse[x] << ',' << bodeMagSolution[x] << ',' << bodePhaseSolution[x] << '\n';
	}


	void find_parenthesis(std::string str, std::vector<int>& usedIndices)
	{
		std::vector<size_t> leftParenthesisPositions;
		std::vector<size_t> rightParenthesisPositions;

		if (str.find('(', 0) != std::string::npos)
		{
			size_t index = str.find('(', 0);
			while (index != std::string::npos)
			{
				leftParenthesisPositions.push_back(index);
				index = str.find('(', index + 1);
			}
		}

		if (str.find(')', 0) != std::string::npos)
		{
			size_t index = str.find(')', 0);
			while (index != std::string::npos)
			{
				rightParenthesisPositions.push_back(index);
				index = str.find(')', index + 1);
			}
		}

		for (int i{ 0 }; i < rightParenthesisPositions.size(); ++i)
		{
			auto iterator = std::upper_bound(leftParenthesisPositions.begin(), leftParenthesisPositions.end(), rightParenthesisPositions[i]);
			--iterator;
			usedIndices.push_back(*iterator);
			usedIndices.push_back(rightParenthesisPositions[i]);
			leftParenthesisPositions.erase(iterator);
		}
	}

	int searchOperator(std::string& str, const char& chosenOperator)
	{
		while (true)
		{
			if (str.find(chosenOperator) == std::string::npos)
				return -1;
			else
			{
				return static_cast<int>(str.find(chosenOperator));
			}
		}
	}
}