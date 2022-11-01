/*
  ==============================================================================

    IdealBar.h
    Created: 9 Apr 2021 12:46:31pm
    Author:  Devansh

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <stdio.h>
#include <math.h>
using namespace std;

class idealBar {
public:
    //Constructor and Destructor
    //idealBar(double length, double width, double height, double Fs, double E, double density, double dampNonFreq, double dampFreq);
    idealBar(double length, double width, double height, double Fs, double E, double density, double dampNonFreq, double dampFreq);
    ~idealBar();

    //Function to set constants of ideal bar
    //void setBarDimensions(double yModulus, double density, double height);

    void initializeBar(double length, double width, double height, double Fs, double E, double density, double dampNonFreq, double dampFreq);
    //Functions to return constants of ideal bar - intended for debugging
    double getYModulus();
    double getDensity();
    double getStiffnessCoefficient();

    //Functions to initialise, update and excite finite difference scheme of ideal bar
    void strike(int hannWidth);
    void initializeUpdateVectors();
    void updateOperation();
    void stateChange();

    //Function to read output of finite difference scheme at a given location
    double getSchemeOutput(int location);
    int excitationLocation;
    
    //Check Status of Initialization
    bool isInitialized = false;

private:
    //Initialise all data members here

    //Dimensions of ideal bar
    double barLength, barWidth, barHeight, barArea, barMomentOfInertia;

    //Bar Constants
    double barE;
    double barDensity;
    double stiffnessCoefficient;

    //Damping Terms
    double sigmaZero;
    double sigmaOne;
    
    //Excitation Locations and mode cancellation ratios
    int cRatio1, cRatio2;

    //Vectors to store FDS states
    std::vector<double> u;
    std::vector<double> uNext;
    std::vector<double> uPrev;
    
    std::vector<std::vector<double>> uStates;
    std::vector<double*> uSwitch;

    //FDS Factors
    double h, k, N;

    //Update Equation Constants
    float MU, c1, c2;
};

