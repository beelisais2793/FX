#include <float.h>
#include <math.h>
#include <random>
#include <string>
#include <vector>
#include <iostream>

/**
*   @author Bruce Blum
*
*   Implementation of linear regression using gradient descent.
*   Basic usage:
*   Construct a object with LinearRegression().
*   Load training data with loadTrainingData().
*   Load corresponding target data with loadTargetData().
*   Set the learning rate with setLearningRate().
*   Perform gradient descent with gradientDescent().
*/
class LinearRegression {
    public:
        LinearRegression();

        /**
         * Calculates the mean squared error of the training data.
         * 
         * @returns Cost MSE on the current traning data using the current weights.
         */
        double cost();

        /**
         *  Helper function that converts a vector of doubles to a string. 
         */
        static std::string dblVectToString(const std::vector<double>& dVec);

        /**
         * Computes the hypothesis minus the corresponding target value.
         * 
         * @param trainingVecIndex row index of the training data vector to 
         *          compute the differece of the hypothesis using the current
         *          weights and the given target value in the target vector.
         */
        double differenceEquation(unsigned long trainingVecIndex);

        /**
         * Perform a estimate using the current weights.
         * 
         * @param testVect A vector of test data similar to a row of training
         *          data to get an estimate of the target value.
         */
        double estimate(std::vector<double> testVec);

        /**
         * @returns The current cost based on the last calculation using the cost()
         *      function.
         */
        double getCost();

        double getLearningRate();

        std::vector<double> getTargetData();

        std::vector<std::vector<double>> getTrainingData();

        std::vector<double> getWeights();

        /**
         * Performs linear regression on the training and target data with the goal
         * of updating the weights that results in a useful estimate of test data.
         * 
         * @param maxSteps Number of steps to take during the update process.
         */
        void gradientDecent(unsigned int maxSteps = 100);

        /**
         * Puts the target data into the target data vector.
         * 
         * @param target Row vector that is equal in lenght to the number of
         *      rows in the training data vector.
         */
        void loadTargetData(std::vector<double> target);

        /**
         * Puts the training data along with an additional row of "1.0" from the
         * equation of hypothesis = w0x0 + w1x1 + ... + wnxn + 1.0xn+1.
         * The training data vector size is increased by 1.
         */
        void loadTrainingData(std::vector<std::vector<double>> trainingSetVec);
    
        /**
         * Set the weights to specific values. Useful to start from a know set.
         * 
         * @param weights Row vector that is equal in length plus 1 of the number
         *      of features in the training data. 
         */
        void reloadWeights(std::vector<double> weights);
        
        /**
         * @param rate The learning rate that the weights will be updated by for each
         *      step.
         */
        void setLearningRate(double rate);
        
        /**
         * Performs gradient descent on all the weights.
         */
        void updateWeights();

    private:
        double learningRate{0.000001};
        double currentCost{0.0};
        std::vector<double> targetVec;
        std::vector<std::vector<double>> trainingVec;
        std::vector<double> weightVec;
};

LinearRegression::LinearRegression() {
    
}

/**
 * Calculates the mean squared error for the training data set.
 */
double LinearRegression::cost(){
    double cost{0.0};
    double hypothesis{0.0};
    
    for(unsigned long i = 0; i < trainingVec.size(); i++){
        hypothesis = 0.0;
        for(unsigned long j = 0; j < weightVec.size(); j++){
            hypothesis += weightVec.at(j) * trainingVec.at(i).at(j);        
        }

        cost += pow(hypothesis - targetVec.at(i), 2);
        
    }

    cost = (0.5 * cost) / static_cast<double>(trainingVec.size());

    currentCost = cost;

    return cost;
}

std::string LinearRegression::dblVectToString(const std::vector<double>& dVec) {
    std::string vectStr;
    for(int i = 0; i < dVec.size(); i++){
        vectStr += std::to_string(dVec.at(i));
        vectStr += " ";
    }

    return vectStr;
}

double LinearRegression::differenceEquation(unsigned long trainingVecIndex){
    double difference{0.0};
    double hypothesis{0.0};

    unsigned int numOfFeatures = weightVec.size();
    
    for(unsigned int i = 0; i < numOfFeatures; i++){
        hypothesis += weightVec.at(i) * trainingVec[trainingVecIndex][i];
        std::cout << "hypo=" << hypothesis << "+=" << weightVec.at(i) << " * " << trainingVec[trainingVecIndex][i] << std::endl;
    }

    difference = hypothesis - targetVec.at(trainingVecIndex);
    std::cout << "difference=" << difference << "=" << hypothesis << "-" << targetVec.at(trainingVecIndex) << std::endl;
    return difference;
}

double LinearRegression::estimate(std::vector<double> testVec) {
    double hypothesis{0.0};

    if(testVec.size() == weightVec.size() - 1) {
        testVec.push_back(1.0);
    } else{
        return 1;
    }

    for(int i = 0; i < testVec.size(); i++){
        hypothesis += testVec.at(i) * weightVec.at(i);
    }

    return hypothesis;
}

double LinearRegression::getCost(){
    return currentCost;
}

double LinearRegression::getLearningRate(){
    return learningRate;
}

std::vector<double> LinearRegression::getTargetData() {
    return targetVec;
}

std::vector<std::vector<double>> LinearRegression::getTrainingData(){
    return trainingVec;
}

std::vector<double> LinearRegression::getWeights(){
    return weightVec;
}

void LinearRegression::gradientDecent(unsigned int maxSteps){
    double prevCost{DBL_MAX};
    double curCost{0.0};
    curCost = cost();

    for(int i = 0; i < maxSteps; i++){
        std::cout << "\tSTEP " << i << " Weights: " << std::endl;
        
        updateWeights();
        curCost = cost();

        
        for(int j = 0; j < weightVec.size(); j++){
            std::cout << j << ": " << weightVec.at(j) << " ";
        }
        std::cout << std::endl;
        std::cout << "+++prevCost: " << prevCost << " curCost: " << curCost << std::endl;

        if(curCost > prevCost){
            std::cout << "Current cost higher than previous cost." << std::endl;
            break;
        } else{
            std::cout << "Setting previouse cost to current cost." << std::endl;
            prevCost = curCost;
        }
    }
}

void LinearRegression::loadTargetData(std::vector<double> target){
    targetVec = target;
}

void LinearRegression::loadTrainingData(std::vector<std::vector<double>> trainingSetVec){
    trainingVec = trainingSetVec;

    // Add an end element of 1 for each training row.
    for(int i = 0; i < trainingVec.size(); i++){
        trainingVec.at(i).push_back(1.0);
    }

    // If weights are not already set then initialize all to random value.
    if(weightVec.size() != trainingVec.at(0).size()){
        weightVec.clear();
        std::random_device randDev;
        std::default_random_engine generator(randDev());
        std::uniform_real_distribution<double> uniDist(0.0, 1.0);

        for(int i = 0; i < trainingVec.at(0).size(); i++){
            double randWeight = uniDist(generator);
            weightVec.push_back(randWeight);
            std::cout << "Initial weight " << std::to_string(i) << ": " << std::to_string(randWeight) << std::endl;
        }
    }
}

void LinearRegression::reloadWeights(std::vector<double> weights){
    weightVec = weights;
}

void LinearRegression::setLearningRate(double rate){
    learningRate = rate;
}

void LinearRegression::updateWeights(){
    double sumOfDer{0.0};
    double currWeight{0.0};
    double newWeight{0.0};

    // For each weight perform gradient descent to update it.
    for(unsigned int i = 0; i < weightVec.size(); i++){
        std::cout << "updateWeights(): updating weight " << std::to_string(i) << std::endl;
        sumOfDer = 0.0;
        double difEq{0.0};
        for(unsigned long j = 0; j < trainingVec.size(); j++){
            difEq = differenceEquation(j);
            sumOfDer += difEq * trainingVec[j][i];
            std::cout << "sum of der = " << sumOfDer << "=" << difEq << "*" << trainingVec[j][i] << std::endl;
        }
        currWeight = weightVec.at(i);
        newWeight = weightVec.at(i) - (learningRate * (sumOfDer / static_cast<double>(trainingVec.size())));
        std::cout << "****weight " << i << " updated to " << newWeight << " from " << currWeight <<  std::endl;
        weightVec.at(i) = newWeight;   
    }
}