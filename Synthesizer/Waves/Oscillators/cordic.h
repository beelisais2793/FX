#ifndef CORDIC_H
#define CORDIC_H

#define PI 3.14159265

struct Results{
	float *x_new;
	float *y_new;
};

class Cordic
{
public:
	Cordic(float, float, float, int);
	void Solve();
	Results GetResults();

private:

	float x_old;
	float y_old;
	float x_new;
	float y_new;

	int iterations;
	int MAX_ITERATIONS;

	float curr_angle;
	float target_angle;

	Results results;
};


#endif // 
