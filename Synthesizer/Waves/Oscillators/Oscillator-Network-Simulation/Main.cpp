#include <iostream>
#include <vector>
#include <time.h>
#include <random>
#include <fstream>
#include <string>
#include "Oscillator.h"

//For now we use global constants
const int NUMBER_OSCILLATORS = 100; //Number of Oscillators in the system (Duh!)
const double STEP_SIZE = 0.001f; //Step Size for the runge kuta or the euler integration
const double COUPLING_STRENGTH = 0.1f; //Coupling strength
const double VARIABLE_B = 9.0f; //Synchronizing factor in the equation for the SNIPER-Model
const int TIMESTEPS = 1750; //Timesteps of the system. So to get the whole time of the simulation do TIMESTEPS * STEP_SIZE
const double DELAYTIME = 2.0f; //Time of the delayed feedback

//Forward declarations
//Loading the couplings matrix from a file and reorganizing the couplings into a 1-dim integer array
void getCouplingFromFile(std::string fileName, int matrix[]);
//Function to create the oscillator objects
std::vector<Oscillator*> creatingOscillators(int matrix[]);
//Main loop of the programm
int mainSimulation(std::vector<Oscillator*> &oscillators);

int main()
{
	//Loading the couplings matrix from a file and reorganizing the couplings into a 1-dim integer array
	int matrix[NUMBER_OSCILLATORS * NUMBER_OSCILLATORS];
	getCouplingFromFile("Kopplungsmatrix.txt", matrix);

	//Creating the oscillators (It takes the 1-Dim couplings "matrix" and gives every oscillator its couplings as a member Variable)
	std::vector<Oscillator*> oscillators = creatingOscillators(matrix);

	//Main loop of the programm
	int ret = mainSimulation(oscillators);

	return ret;
}

//Loading the couplings matrix from a file and reorganizing the couplings into a 1-dim integer array
void getCouplingFromFile(std::string fileName, int matrix[])
{
	//Reading the coupling from a file
	std::ifstream couplingMatrix("Kopplungsmatrix.txt");
	if (couplingMatrix.fail())
	{
		perror("Kopplungsmatrix.txt");
	}

	//Pushing the coupling into a vector of strings
	std::vector<std::string> rows;
	std::string number;
	while (getline(couplingMatrix, number))
	{
		rows.push_back(number);
	}

	//Changing the coupling into an array of numbers
	for (unsigned int i = 0; i < rows.size(); i++) {
		for (unsigned int j = 0; j < rows[i].size(); j++) {
			matrix[rows.size()*i + j] = rows[i][j] - '0';
		}
	}

}

//Function to create the oscillator objects
std::vector<Oscillator*> creatingOscillators(int matrix[])
{
	//Creating the random engine and seeding it with the time
	std::mt19937 mt_rand((unsigned int)time(0));
	std::uniform_real<double> rand_double(0, 1);

	//Vector of Object pointers for the oscillators
	std::vector<Oscillator*> oscillators;

	//Creating the objects, oscillators, and pushing them into the vector of pointers
	for (int i = 0; i < NUMBER_OSCILLATORS; i++) {
		Oscillator *tmp = new Oscillator(rand_double(mt_rand), rand_double(mt_rand), i, matrix, NUMBER_OSCILLATORS, COUPLING_STRENGTH, DELAYTIME, STEP_SIZE);
		oscillators.push_back(tmp);
	}

	return oscillators;
}



//Main loop of the programm
int mainSimulation(std::vector<Oscillator*> &oscillators)
{
	//Open file for saving the data
	std::ofstream outputFile;
	outputFile.open("C:/Users/Felix/PycharmProjects/Masterarbeit/Data.txt");
	if (outputFile.fail())
	{
		perror("Data.txt");
		return 1;
	}

	//Main loop of the simulation
	int steps = 0; //Steps counting for the simulation
	while (steps < TIMESTEPS) { //Doing the simulation until TIMESTEPS is reached

		//Looping through all oscillators and doing the Runge-Kutta-4
		for (int rungeStep = 0; rungeStep < 4; rungeStep++) { //Looping through the 4 runge-steps
			for (int i = 0; i < NUMBER_OSCILLATORS; i++) { //Looping through all oscillators and calculating dx/dt for this step
				oscillators[i]->rungeKutta(oscillators, rungeStep, VARIABLE_B, steps);
			}

			//Adding for all oscillators the new part step (dx[i]/dt) * dt
			if (rungeStep != 3) {
				for (int i = 0; i < NUMBER_OSCILLATORS; i++) {
					if (rungeStep < 2) {
						oscillators[i]->nextRungeStep(rungeStep, STEP_SIZE / 2.0f); //Adding this dx on the original coordinate to get the temporary new step marked with index 0-2
					}
					else {
						oscillators[i]->nextRungeStep(rungeStep, STEP_SIZE); //Adding this dx on the original coordinate to get the temporary new step marked with index 3
					}
				}
			}
			
		}

		//Finally calculating the real new coordinate from the arithmetic mean of the rungesteps
		for (int i = 0; i < 1; i++) {
			oscillators[i]->finalRungeStep(STEP_SIZE, steps);
		}

		//Couts the steps so the simulation can be followed via the command prompt
		std::cout << steps << " steps " << std::endl;

		//Putting Data into the File
		for (int i = 0; i < NUMBER_OSCILLATORS; i++) {

			outputFile << oscillators[i]->getX(0) << "	";
		}
		outputFile << steps << "\n";

		//Incrementing the steps
		steps++;
	}

	//Closing the file
	outputFile.close();

	//So the console doesn´t close immediatly
	std::cout << "ALL DONE!\n";
	int tmp;
	std::cin >> tmp;

	return 0;
}
