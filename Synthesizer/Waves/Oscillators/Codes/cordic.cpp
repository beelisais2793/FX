#include "Cordic.h"
#include <math.h>

Cordic::Cordic(float _x_old, float _y_old, float _target_angle, int _MAX_ITERATIONS)
{
	x_old = _x_old;
	y_old = _y_old;

	target_angle = _target_angle;
	curr_angle = 0;
	iterations = 0;
	MAX_ITERATIONS = _MAX_ITERATIONS;

	results.x_new = &x_new;
	results.y_new = &y_new;

}

void Cordic::Solve()
{

	while (iterations != MAX_ITERATIONS)
	{

		if (curr_angle < target_angle)
		{
			curr_angle += atan(1 / pow(2, iterations)) * (180 / PI);
			x_new = x_old - y_old * (1 / pow(2, iterations));
			y_new = y_old + x_old * (1 / pow(2, iterations));

		}

		else
		{
			curr_angle -= atan(1 / pow(2, iterations)) * (180 / PI);
			x_new = x_old - y_old * (-1 / pow(2, iterations));
			y_new = y_old + x_old * (-1 / pow(2, iterations));
		}

		x_old = x_new;
		y_old = y_new;

		iterations++;
	}

	x_new = 0.61 * x_new;
	y_new = 0.61 * y_old;

	//std::cout << x_new << " " << y_new  << " " << curr_angle << " " << target_angle << std::endl;

}

Results Cordic::GetResults()
{
	return results;
}