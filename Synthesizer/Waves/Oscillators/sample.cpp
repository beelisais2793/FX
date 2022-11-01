#include <iostream>
#include "Cordic.h"
#include <math.h>

int main()
{
	Cordic *obj1 = new Cordic(1, 0, 53, 50);
	Results result;

	obj1->Solve();
	result = obj1->GetResults();

	float rel_error_cos = abs((*(result.x_new) - cos(53) / cos(53))) * 100;
	float rel_error_sin = abs((*(result.x_new) - sin(53) / sin(53))) * 100;

	std::cout << "Cosine of target angle: " << *(result.x_new) << std::endl;
	std::cout << "Relative error of cos: " << rel_error_cos << std::endl;
	std::cout << "Sine of target angle: " << *(result.y_new) << std::endl;
	std::cout << "Relative error of sin: " << rel_error_sin << std::endl;
}