#include "EulerString.h"
#include <iostream>

EulerString::EulerString()
{
	//ctor
}

EulerString::~EulerString()
{
	//dtor
}
void EulerString::NextMoment(float c)
{
	vector<float> emptyVector; // Это нужно переписать
	int t = table[0].size();
	if (t < 1)
	{
		std::cerr << "Error in function simpleNextMoment.\n Starting conditions have not be set. (t < 1)\n";
		return;
	}
	for (int x = 0; x < table.size(); x++)
	{
		table[x].push_back(
							c*xderivative(x, t - 1, 2)*tstep()*tstep()
							+ tderivative(x, t - 1)*tstep()
							+ table[x][t - 1]);
	}
}
