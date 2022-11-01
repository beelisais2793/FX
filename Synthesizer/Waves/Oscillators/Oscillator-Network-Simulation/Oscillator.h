#pragma once
#include <vector>
#include <string>

class Oscillator
{
public:
	//Con- and Destructor
	Oscillator(double x, double y, int index, int couplingMatrice[], int dimension, double couplingStrength, double delayTime, double timeStep);
	~Oscillator();

	//GETTERS
	double getX(const int index){ return _x[index]; }
	double getY(const int index){ return _y[index]; }
	double getDelayedX(const int index) { return _delayedCoordinateX[index]; }
	double getDelayedY(const int index) { return _delayedCoordinateY[index]; }
	int getIndex() { return _index; }
	std::string getCoupling();
	double getDelaySize() { return _delaySteps; }

	//SETTERS
	void setX(double x) { _x[0] = x; }
	void setY(double y) { _y[0] = y; }
	void setCoupling(int matrice[], int index, int dimension);

	//EulerStep Function
	void eulerStep(std::vector<Oscillator*> oscillators, const double b);
	void nextStep(const double stepSize);

	//RungeKutta Functions
	void rungeKutta(std::vector<Oscillator*>& oscillators, const int& coordinateIndex, const double& b, const int& steps);
	void nextRungeStep(const int& coordinateIndex, const double& stepSize);
	void finalRungeStep(const double& stepSize, const int& steps);

private:
	//Coordinates of the oscillator. The index in the array is for the runge kutta
	double _x[5];
	double _y[5];
	double _dx[4];
	double _dy[4];
	std::vector<double> _delayedCoordinateX;
	std::vector<double> _delayedCoordinateY;
	double _delaySteps;

	int _index; //The index of the oscillator
	
	//How the oscillators are coupled and what coupling strength they have
	std::vector<int> _couplings; 
	double _couplingStrength;

};

