#ifndef VOSE_H
#define VOSE_H

#include <random>

class vose
{
public:
    vose(double*, unsigned int, std::mt19937&);
    void demo();
    unsigned int alias_method();
    
private:
    std::mt19937 &local_mt;
    
    // Don't declare the distribution with any arguments!!
    std::uniform_real_distribution<double> uniform;
    
    double *distribution;
    unsigned int array_size;
    unsigned int *alias;
};

#endif /* VOSE_H */
