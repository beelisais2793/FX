#include <string>
#include <vector>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <random>

/**
 * Implementation of the binary logistic regression algorithm.
 * Requires two data files. One training file and one label
 * file with the classification for the corresponding row in the
 * training file. 
 * 
 * For example, a loan application of credit score, annual income,
 * and age.
 * Training file:
 * 720  70  51
 * 490  31  28
 * 
 * Label file:
 * 1
 * 0
 * 
 * @author Bruce Blum
 **/

class LogisticRegression{
    public:
        LogisticRegression();

        /**
         * Helper function so make a vector of doubles into a string.
         * 
         * @param dVec vector to make a string of values from.
         **/
        std::string static dblVectToString(const std::vector<double>& dVec);

        void addBiasElementToTrainingVector();

        /**
         * Uses the trained weights to make a classification on the values 
         * of the passed in vector.
         * 
         * @param dataVect holds the values used in the logistic function to
         *          make a classification.
         **/
        double classify(std::vector<double> dataVec);

        int estimate(unsigned long trainingIndex);

        /**
         * Returns the last calculated log likelyhood.
         **/
        double getCurrentLogLikelyhood();

        double getLearningRate();

        std::vector<double> getWeights();

        /**
         * Performs gradient ascent on the current training data and weights.
         * 
         * @param maxIterations the max number of update steps for the weights.
         **/
        void gradientAscentWeightUpdate(unsigned int maxIterations = 100);

        /**
         * Sets the weights to a random value [0, 1].
         * 
         * @param size of the weight vector.
         **/
        void initRandomWeights(unsigned int size = 2);

        /**
         * Loads the label data for the corresponding training data.
         * 
         * @param labels for the training data.
         **/
        void loadLabelData(std::vector<int> dataVec);

        /**
         * Loads the training data.
         * 
         * @param dataVec of training data. Each containing vector will have a 
         *      "1.0" added to it for the bias weight to pair with.
         **/
        void loadTrainingData(std::vector<std::vector<double>> dataVec);

        /**
         * Provides a way to set the weights to specific values usually from
         * a previous training session.
         * 
         * @param weights vector should be sized already to match the training
         *      vector with a bias element.
         **/
        void loadWeights(std::vector<double> weights);

        /**
         * Performs the logistic function on a given weight and data vector.
         * 
         * @param weightVec should match the size of the data vector.
         * @param xVec should match the size of the weight vector.
         **/
        double logisticFunction(std::vector<double> weightVec, std::vector<double> xVec);

        /**
         * Perform the sum of the log likelyhood using current weights.
         **/
        double logLikelyhoodSum();

        void setLearningRate(double rate);

        double sumDifferenceTimesTraining(unsigned int weigthIndex);

    private:
        double currentLogLikelyhood{0.0};
        std::vector<int> labelVec;
        double learningRate{0.00001};
        std::vector<std::vector<double>> trainingVec;
        std::vector<double> weightVec;
};

**
 * Implementation of the logistic regression using gradient ascent.
 * 
 * @author Bruce Blum
 **/

LogisticRegression::LogisticRegression(){

}

void LogisticRegression::addBiasElementToTrainingVector(){
    for(unsigned int i = 0; i < trainingVec.size(); i++){
        trainingVec.at(i).push_back(1.0);
    }
}

double LogisticRegression::classify(std::vector<double> dataVec){
    double classificationResult{0.0};

    dataVec.push_back(1.0);

    classificationResult = logisticFunction(weightVec, dataVec);

    return classificationResult;
}

std::string LogisticRegression::dblVectToString(const std::vector<double>& dVec) {
    std::string vectStr;
    for(int i = 0; i < dVec.size(); i++){
        vectStr += std::to_string(dVec.at(i));
        vectStr += " ";
    }

    return vectStr;
}

int LogisticRegression::estimate(unsigned long trainingIndex){
    
}

double LogisticRegression::getCurrentLogLikelyhood(){
    return currentLogLikelyhood;
}

double LogisticRegression::getLearningRate(){
    return learningRate;
}

std::vector<double> LogisticRegression::getWeights(){
    return weightVec;
}

/**
 * Update each weight by performing gradient ascent. Stop when log likelyhood is
 * max.
 **/
void LogisticRegression::gradientAscentWeightUpdate(unsigned int maxIterations){
    std::cout << "gradientAscentWeightUpdate()" << std::endl;

    double previousLogLike = -1 * DBL_MAX;

    for(unsigned int i = 0; i < maxIterations; i++){
        std::cout << "previousLogLike: " << previousLogLike << std::endl;
        for(unsigned int j = 0; j < weightVec.size(); j++){
            std::cout << "weight " << j << ": " << weightVec.at(j) << std::endl;
            weightVec.at(j) = weightVec.at(j) + learningRate * sumDifferenceTimesTraining(j);
            std::cout << "weight " << j << ": " << weightVec.at(j) << std::endl;
        }

        currentLogLikelyhood = logLikelyhoodSum();
        std::cout << "currentLogLikelyhood: " << currentLogLikelyhood << std::endl;
        if((currentLogLikelyhood > (-1*DBL_MAX)) && (previousLogLike > currentLogLikelyhood)){
            std::cout << "previousLogLike > currentLogLikelyhood break out of iteration loop." << std::endl;
            break;
        }

        previousLogLike = currentLogLikelyhood;
    }
}

void LogisticRegression::initRandomWeights(unsigned int size){
    std::cout << "initRandomWeights()" << std::endl;
    weightVec.clear();
    std::random_device randDev;
    std::default_random_engine generator(randDev());
    std::uniform_real_distribution<double> uniDist(0.0, 1.0);

    for(unsigned int i = 0; i < size + 1; i++){
        double randWeight = uniDist(generator);
        weightVec.push_back(randWeight);
        std::cout << randWeight << " ";
    }
    std::cout << std::endl;
}

void LogisticRegression::loadLabelData(std::vector<int> dataVec){
    labelVec = dataVec;
}

void LogisticRegression::loadTrainingData(std::vector<std::vector<double>> dataVec){
    trainingVec = dataVec;

    // Add a "1.0" to the end of each training sample.
    addBiasElementToTrainingVector();
}

void LogisticRegression::loadWeights(std::vector<double> weights){
    weightVec = weights;
}

double LogisticRegression::logisticFunction(std::vector<double> weightVec, std::vector<double> xVec){
    std::cout << "logisticFunction()" << LogisticRegression::dblVectToString(xVec) << std::endl;
    double result{0.0};
    double sumWeightAndX{0.0};

    for(int i = 0; i < weightVec.size(); i++){
        sumWeightAndX += weightVec.at(i) * xVec.at(i);
    }

    sumWeightAndX = static_cast<double>(-1.0) * sumWeightAndX;
    std::cout << "sumWeightAndX: " << sumWeightAndX << std::endl;

    result = static_cast<double>(1.0) / (static_cast<double>(1.0) + pow(M_E, sumWeightAndX));
    std::cout << "result: " << result << std::endl;
    return result;
}

double LogisticRegression::logLikelyhoodSum(){
    double sum{0.0};

    for(unsigned int i = 0; i < trainingVec.size(); i++){
        double logisticFncResult = logisticFunction(weightVec, trainingVec.at(i));
        std::cout << "logisticFncResult: " << logisticFncResult << std::endl;
        std::cout << "lableVect.at(" << i << "): " << labelVec.at(i) << std::endl;
        if(labelVec.at(i) == 1){
            std::cout << "label == 1" << std::endl;
            sum += log(logisticFncResult);
        } else{
            std::cout << "label != 1 " << std::endl;
            sum += log(1-logisticFncResult);
        }
        std::cout << "sum: " << sum << std::endl;
    }    
    
    std::cout << "sum: " << sum << std::endl;
    return sum;
}

void LogisticRegression::setLearningRate(double rate){
    learningRate = rate;
}

double LogisticRegression::sumDifferenceTimesTraining(unsigned int weigthIndex){
    std::cout << "sumDifferenceTimesTraining()" << std::endl;
    double sum{0.0};

    for(unsigned int i = 0; i < trainingVec.size(); i++){
        sum += (labelVec.at(i) - logisticFunction(weightVec, trainingVec.at(i))) 
                    * trainingVec.at(i).at(weigthIndex);
    }

    std::cout << "sum: " << sum << std::endl;
    return sum;
}