#include "Oscillator.h"
#include <random>
#include <time.h>
#include <iostream>

//Creator for an Oscillator, it takes the coordinates, the index, the topology of the coupling, the dimension as the number of oscillators in the system and the coupling strength
Oscillator::Oscillator(double x, double y, int index, int couplingMatrix[], int dimension, double couplingStrength, double delayTime, double timeStep)
{
	std::cout << "Constructing an Oscillator!\n";
	_x[0] = x;
	_y[0] = y;
	_index = index;
	_couplingStrength = couplingStrength;

	setCoupling(couplingMatrix, index, dimension);

	//Setting up the vector for the delayed feedback
	if (delayTime != 0.0f) {
		for (int i = 0; i < (delayTime / timeStep); i++) {
			_delayedCoordinateX.push_back(0.0f);
			_delayedCoordinateY.push_back(0.0f);
		}
		_delaySteps = (delayTime / timeStep);
	}
}

//Destructor for an oscillators (Should never be called!)
Oscillator::~Oscillator()
{
	std::cout << "The Destructor of the Oscillator is called!\n";
}

//Reading the coupling from an 1-D int array. 
//Every oscillator saves its coupling to the other oscillators. Saves later time in the calculation
void Oscillator::setCoupling(int matrix[], int index, int dimension)
{
	for (int i = 0; i < dimension; i++) {
		if (matrix[index * dimension + i] == 1) {
			_couplings.push_back(i);
		}
	}
}

//Returns the coupling-topology of this oscillator
std::string Oscillator::getCoupling()
{
	std::string coupling = "";
	for (unsigned int i = 0; i < _couplings.size(); i++) {
		coupling = coupling + std::to_string(_couplings[i]);
	}
	return coupling;
}

//Basic euler step for integrating the differential eq.
void Oscillator::eulerStep(std::vector<Oscillator*> oscillators, const double b)
{
	_dx[0] = 0.0f;
	_dy[0] = 0.0f;
	_dx[0] = (_x[0] * (1 - _x[0] * _x[0] - _y[0] * _y[0]) + _y[0] * (_x[0] - b));
	_dy[0] = (_y[0] * (1 - _x[0] * _x[0] - _y[0] * _y[0]) - _x[0] * (_x[0] - b));
	for (unsigned int i = 0; i < _couplings.size(); i++){
		_dx[0] += _couplingStrength * ((oscillators[i]->getX(0) - _x[0]) + (oscillators[i]->getY(0)) - _y[0]);
		_dy[0] += _couplingStrength * ((oscillators[i]->getX(0) - _x[0]) + (oscillators[i]->getY(0)) - _y[0]);
	}
}

//Applying the dx_dy step for euler
void Oscillator::nextStep(const double stepSize)
{
	_x[0] += _dx[0] * stepSize;
	_y[0] += _dy[0] * stepSize;
}

//Runge-Kutta-4 for solving the diff. eq.
void Oscillator::rungeKutta(std::vector<Oscillator*>& oscillators, const int& coordinateIndex, const double& b, const int& steps)
{
	//Creaating the first delayedTime/timeStep coordinates without a delayed feedback
	if (steps < _delaySteps) {
		_dx[coordinateIndex] = 0.0f;
		_dy[coordinateIndex] = 0.0f;
		_dx[coordinateIndex] = (_x[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) + _y[coordinateIndex] * (_x[coordinateIndex] - b));
		_dy[coordinateIndex] = (_y[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) - _x[coordinateIndex] * (_x[coordinateIndex] - b));
		for (unsigned int i = 0; i < _couplings.size(); i++) {
			_dx[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
			_dy[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
		}
	}
	//Now feeding in the delayed feedback (The delayed feedback isnt using runge kutta for now. 
	//This should lead to a faster rising error (proportional to the euler error)
	//Should later be implemented
	else {
		if (coordinateIndex == 0) {
			_dx[coordinateIndex] = 0.0f;
			_dy[coordinateIndex] = 0.0f;
			_dx[coordinateIndex] = (_x[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) + _y[coordinateIndex] * (_x[coordinateIndex] - b)) + _x[coordinateIndex] * _delayedCoordinateX[(steps % int(_delaySteps))];
			_dy[coordinateIndex] = (_y[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) - _x[coordinateIndex] * (_x[coordinateIndex] - b)) + _y[coordinateIndex] * _delayedCoordinateY[(steps % int(_delaySteps))];
			for (unsigned int i = 0; i < _couplings.size(); i++) {
				_dx[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
				_dy[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
			}
		}
		//For the rungesteps 2 and 3 we have to take the average of the two adjusting delayed coordinates
		if (coordinateIndex == 1 || coordinateIndex == 2) {
			double delayedX = _delayedCoordinateX[(steps % int(_delaySteps))] + _delayedCoordinateX[((steps + 1) % int(_delaySteps))] * 0.5f;
			double delayedY = _delayedCoordinateY[(steps % int(_delaySteps))] + _delayedCoordinateY[((steps + 1) % int(_delaySteps))] * 0.5f;
			//Now calculating this runge step
			_dx[coordinateIndex] = 0.0f;
			_dy[coordinateIndex] = 0.0f;
			_dx[coordinateIndex] = (_x[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) + _y[coordinateIndex] * (_x[coordinateIndex] - b)) + _x[coordinateIndex] * delayedX;
			_dy[coordinateIndex] = (_y[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) - _x[coordinateIndex] * (_x[coordinateIndex] - b)) + _y[coordinateIndex] * delayedY;
			for (unsigned int i = 0; i < _couplings.size(); i++) {
				_dx[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
				_dy[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
			}
		}
		//For the last runge step we have to take the next delayed step
		if (coordinateIndex == 3) {
			_dx[coordinateIndex] = 0.0f;
			_dy[coordinateIndex] = 0.0f;
			_dx[coordinateIndex] = (_x[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) + _y[coordinateIndex] * (_x[coordinateIndex] - b)) + _x[coordinateIndex] * _delayedCoordinateX[((steps + 1) % int(_delaySteps))];
			_dy[coordinateIndex] = (_y[coordinateIndex] * (1 - _x[coordinateIndex] * _x[coordinateIndex] - _y[coordinateIndex] * _y[coordinateIndex]) - _x[coordinateIndex] * (_x[coordinateIndex] - b)) + _y[coordinateIndex] * _delayedCoordinateY[((steps + 1) % int(_delaySteps))];
			for (unsigned int i = 0; i < _couplings.size(); i++) {
				_dx[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
				_dy[coordinateIndex] += _couplingStrength * ((oscillators[_couplings[i]]->getX(coordinateIndex) - _x[coordinateIndex]) + (oscillators[_couplings[i]]->getY(coordinateIndex)) - _y[coordinateIndex]);
			}
		}
	}

}

//Applying the dx_dy step for runge kutta (This is just applying the part steps in runge kutta)
void Oscillator::nextRungeStep(const int& coordinateIndex, const double& stepSize)
{
	_x[coordinateIndex + 1] = _x[0] + _dx[coordinateIndex] * stepSize;
	_y[coordinateIndex + 1] = _y[0] + _dy[coordinateIndex] * stepSize;
}

//Applying alle rungeParts as a whole step to the main coordinates _x[0] _y[0]
void Oscillator::finalRungeStep(const double& stepSize, const int& steps)
{
	double multiplicator = 0.166666f;
	_x[0] += (multiplicator * (_dx[0] + 2 * _dx[1] + 2 * _dx[2] + _dx[3]) * stepSize);
	_y[0] += (multiplicator * (_dy[0] + 2 * _dy[1] + 2 * _dy[2] + _dy[3]) * stepSize);
	_delayedCoordinateX[steps % int(_delaySteps)] = _x[0];
	_delayedCoordinateY[steps % int(_delaySteps)] = _y[0];
}
