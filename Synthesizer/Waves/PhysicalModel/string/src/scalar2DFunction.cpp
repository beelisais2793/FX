#include <iostream>
#include <vector>
#include "scalar2DFunction.h"
#include <math.h>

scalar2DFunction::scalar2DFunction()
{
	//ctor
}

scalar2DFunction::~scalar2DFunction()
{
	//dtor
}

vector<vector<float>> scalar2DFunction::getTable()
{
	return this->table;
}
vector<float> scalar2DFunction::getLastString()
{
	return table.back();
}
// Переписать так, что функции будет передоваться поток ввода
void scalar2DFunction::readTable(int quantityOfXVal, int quantityOfTVal)
{
	table.clear();
	float tmp;
	vector<float> emptyVector; // Это нужно переписать
	for (int i = 0; i < quantityOfXVal; i++)
	{
		table.push_back(emptyVector);
		for (int j = 0; j < quantityOfTVal; j++)
		{
			std::cin >> tmp;
			table[i].push_back(tmp);
		}
	}
}
//Это нужно переписать с использованием потоков вывода
void scalar2DFunction::printTable()
{
	for (int i = 0; i < table.size(); i++)
	{
		for (int j = 0; j < table[i].size(); j++)
		{
			std::cout << table[i][j] << ' ';
		}
		std::cout << std::endl;
	}
}
void scalar2DFunction::printTableWithNumbers()
{
	for (int i = 0; i < table.size(); i++)
	{
		std::cout << i << '\t';
		for (int j = 0; j < table[i].size(); j++)
		{
			std::cout << table[i][j] << '\t';
		}
	std::cout << '\n';
	}
}
void scalar2DFunction::printLastString()
{
	for (int x = 0; x < table.size(); x++)
	{
		std::cout << table[x].back() << '\n';
	}
	std::cout << '\n';
}
//Переписать через потоки
void scalar2DFunction::readMaxes()
{
	std::cin >> xmin >> xmax >> tmin >> tmax;
}
//Переписать чрез потоки
void scalar2DFunction::printMaxes()
{
	std::cout   << xmin << '\t'
				<< xmax << '\n'
				<< tmin << '\t'
				<< tmax;
}
float scalar2DFunction::gettmax()
{
	return tmax;
}
float scalar2DFunction::gettmin()
{
	return tmin;
}
float scalar2DFunction::getxmax()
{
	return xmax;
}
float scalar2DFunction::getxmin()
{
	return tmin;
}
float scalar2DFunction::tstep()
{
	if ((table.size() < 1) || (table[0].size() < 1))
	{
		cerr << "Error in function \"tstep\".\n Table is empty.\n";
		return -1;
	}
	return (tmax - tmin)/table[0].size();
}

float scalar2DFunction::xstep()
{
	if (table.size() < 1)
	{
		cerr << "Error in function \"tstep\".\n Table is empty.\n";
		return -1;
	}
	return (xmax - xmin)/table.size();
}
int scalar2DFunction::inRange(int x, int t)
{
	if ((x < 0) || (x >= table.size()))
	{
		return 0;
	}
	if ((t < 0) || (t >= table[x].size()))
	{
		return 0;
	}
	return 1;
}
int scalar2DFunction::inRangePrint(int x, int t)
{
	if ((x < 0) || (x >= table.size()))
	{
		cerr << "The function can not be differentiated, because x out of range.\n";
		//cerr << "x = " << x << " table.size() = " << table.size() << '\n';
		return 0;
	}
	if ((t < 0) || (t >= table[x].size()))
	{
		cerr << "The function can not be differentiated, because t out of range.\n";
		return 0;
	}
	return 1;
}
float scalar2DFunction::tderivative(int x, int t, int n)
{
	float step = tstep();
	//Обработать step = -1;
	if (step == 0)
	{
		cerr << "The function can not be differentiated, because step = 0.\n";
		return 0;
	}
	if (!inRangePrint(x, t)) return 0;
	if (t >= n)
	{
		return (table[x][t] - table[x][t - n])/pow(step, n);
	}
	return 0; // Derivative in points t < n, because I can't concoct  something better.
}
float scalar2DFunction::xderivative(int x, int t, int n)
{
	float step = xstep();
	//Обработать step = -1;
	if (step == 0)
	{
		cerr << "The function can not be differentiated, because step = 0.\n";
		return 0;
	}
	if (!inRangePrint(x, t)) return 0;
	if (x >= n)
	{
		return (table[x][t] - table[x - n][t])/pow(step, n);
	}
	return 0; // Derivative in points x  < n, because I can't concoct  something better.
}
float scalar2DFunction::tderivative(int x, int t)
{
	return tderivative(x, t, 1);
}
float scalar2DFunction::xderivative(int x, int t)
{
	return xderivative(x, t, 1);
}

