/*
  ==============================================================================

    IdealBar.cpp
    Created: 9 Apr 2021 12:46:31pm
    Author:  Devansh

  ==============================================================================
*/

#include "IdealBar.h"
#include "IdealBar.h"
#include<math.h>

idealBar::idealBar(double length, double width, double height, double Fs, double E, double density, double dampNonFreq, double dampFreq){
    //Assign length, width and height of bar
    barLength = length;
    barWidth = width;
    barHeight = height;

    //Calculate Area and Moment of Inertia of the bar
    barArea = barHeight * barWidth;
    barMomentOfInertia = (barWidth * barHeight * barHeight * barHeight) / 12.0;

    //Assign Young's modulus and density of the bar
    barE = E;
    barDensity = density;

    //Calculate Stiffness Coefficient of the bar
    stiffnessCoefficient = sqrt((barE * barMomentOfInertia) / (barDensity * barArea));

    //Assign damping terms
    sigmaZero = dampNonFreq;
    sigmaOne = dampFreq;

    //Calculate time-step based on sampling frequency
    k = 1 / Fs;

    //Calculate Spatial Step based on stability condition and length of the bar
    double factor = sqrt((sigmaOne * sigmaOne) + (stiffnessCoefficient * stiffnessCoefficient));
    h = sqrt(2 * k * (sigmaOne + factor));
    N = floor(barLength / h);
    h = barLength / N;

    //Calculate excitaion location and mode cancellation location based on N
    cRatio1 = floor(0.2*N);
    cRatio2 = floor(0.8*N);
    excitationLocation = floor(0.5*N);
    
    //Calculate update equation constants
    MU = (stiffnessCoefficient * k) / (h * h);
    c1 = 2 * sigmaZero * k;
    c2 = (sigmaOne * k) / (h * h);

    initializeUpdateVectors();
}


idealBar::~idealBar() {

}

void idealBar::initializeUpdateVectors() {
    //std::cout<<N<<std::endl;
    for (int i = 1; i <= N + 1; i++) {
        u.push_back(0);
        uNext.push_back(0);
        uPrev.push_back(0);
    }
    
    isInitialized = true;
    
    /*for(int i = 0; i<3; ++i){
        uStates.push_back(std::vector<double> (N+1, 0));
    }*/
}

void idealBar::strike(int hannWidth) {
    //Initial Excitement - raised cosine
    for (int i = 0; i < hannWidth; i++) {
        u[excitationLocation - 2 + i] = 0.5 * (1 - cos(2 * juce::double_Pi * ((double)(i) / (double)(hannWidth - 1))));
        uPrev[excitationLocation - 2 + i] = 0.5 * (1 - cos(2 * juce::double_Pi * ((double)(i) / (double)(hannWidth - 1))));
    }
}

void idealBar::updateOperation() {
    for (int range = 2; range < N - 1; range++) {
        //Standard conditions
        uNext[range] = ((2 - 6 * MU * MU - c1 - 2 * c2) * u[range])
            + ((c1 + 2 * c2 - 1) * uPrev[range])
            - (MU * MU * (u[range + 2] + u[range - 2]))
            + ((4 * MU * MU + c2) * (u[range + 1] + u[range - 1]))
            - (c2 * (uPrev[range + 1] + uPrev[range - 1]));
    }
    
    //Boundary Conditions
    //Free boundary condition - left boundary 1
    uNext[1] = ((2 - 5*MU*MU - c1 - 2*c2) * u[1])
                    + ((c1 + 2*c2 - 1) * uPrev[1])
                    - (MU*MU * u[3])
                    + ((4*MU*MU + c2) * (u[2] + u[0]))
                    - (c2 * (uPrev[2] + uPrev[0]));
    
   //Free boundary condition - right boundary 1
    uNext[N-1] = ((2 - 5*MU*MU - c1 - 2*c2) * u[N-1])
                + ((c1 + 2*c2 - 1) * uPrev[N-1])
                - (MU*MU * u[N-3])
                + ((4*MU*MU + c2) * (u[N] + u[N-2]))
                - (c2 * (uPrev[N] + uPrev[N-2]));
    
    //Free boundary condition - left boundary 2
     uNext[0] = ((2 - 6*MU*MU - c1 - 2*c2) * u[0])
                 + ((c1 + 2*c2 - 1) * uPrev[0])
                 - (2*MU*MU * (u[2] - 2*u[1]));

    //Free boundary condition - right boundary 2
     uNext[N] = ((2 - 6*MU*MU - c1 - 2*c2) * u[N])
                 + ((c1 + 2*c2 -1) * uPrev[N])
                 - (2*MU*MU * (u[N-2] - 2*u[N-1]));

    uNext[cRatio1] = 0;
    uNext[cRatio2] = 0;
}

void idealBar::stateChange() {
    std::vector<double> temp;
    for (int i = 0; i < uPrev.size(); i++) {
        temp.push_back(0);
    }

    /*for (int i = 0; i<uPrev.size() ; i++){
        temp[i] = uPrev[i];
        uPrev[i] = u[i];
        u[i] = uNext[i];
        uNext[i] = temp[i];
    }*/ //Do Pointer Switch

    temp = uPrev;
    uPrev = u;
    u = uNext;
    uNext = temp;
    
    
}

double idealBar::getSchemeOutput(int location) {
    //std::cout<<u[location]<<std::endl;
    return(u[location]);
    //return 0;
}
