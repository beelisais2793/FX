//Vose Random Number Generation
#include "vose.h"

#include <random>
#include <iostream>
#include <limits>
#include <cmath>
#include <chrono>

using namespace std;

vose::vose(double *array, unsigned int size, mt19937 &mt) : local_mt(mt)
{
    //Initialize
    //std::random_device rd; // RNG that uses device for seed
    //auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    //mt19937 mt(seed); // Seeds Mersenne Twister with Device RNG
    
    mt19937 &local_mt = mt;
    
    // nextafter(x, y) Returns the next representable
    // value after x in the direction of y.
    uniform_real_distribution<double> uniform(0, 1);
    
    distribution = array;
    array_size = size;
    
    alias = new unsigned int[size];
    for (unsigned int i = 0; i<size; i++)
    {
        alias[i] = i+1;
    }
}


void vose::demo()
{
    for (int i=0; i<10; ++i)
        cout << uniform(local_mt) << endl;
}

unsigned int vose::alias_method()
{
    double x = uniform(local_mt);
    
    unsigned int i = floor(array_size*x)+1;
    double y = array_size*x+1-i;
    
    if (y<distribution[i])
        return i;
    
    else
        return alias[i];
        
}