//
//  EMQFHalfband.hpp
//
//  Created by Raphael Pistachio on 22.12.16.
//  Copyright (c) 2016 Raphael Pistachio. All rights reserved.
//

#pragma once


#include <algorithm>

#include <cmath>

#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/jacobi_elliptic.hpp>

namespace afx {
namespace emqf {
    
/*
 Parameters
 ----------
 numPairs : int
   Number of imaginary pole pairs of the filter. The order of the filter will be (2*numPairs + 1).
 selectivityFactor : T
   Selectivity factor (usually xi in the literature)
 squaredPoles : array
   [Result] Will contain the squared magnitudes of the purely imaginary poles (called beta in the literature).
   Size must be numPairs. Ascending order.
   (Note that the final filter contains a pole at z=0)
 
 ----------
 Filter design steps according to
 Lutovac's Book 'Filter Design for Signal Processing'
 10.3.5 Zeros, Poles and Q-Factors
 */
template <typename T>
int getEMQFHalfbandFilterBySelectivityFactor(int numPairs, T selectivityFactor, T* squaredPoles) {
    int fail = 0;

    if (numPairs <= 0) { // check params
        fail = 1;
    }
    
    if (fail) {return fail;}
    
    T* squaredPolesIt = squaredPoles;
    
    bool sortNeeded = false;
    T lastSquaredPole = -1;
    
    T a = selectivityFactor;
    T n = (numPairs*2) + 1;
    T k = T(1)/a;
    for (int i=numPairs; i>0; i--) {
        T ind = T(i);
        
        T u = ((2*ind-1)/n + T(1)) * boost::math::ellint_1(k);
        T X = boost::math::jacobi_sn(k, u);

        // TODO: check possible 'catastrophic cancellation'
        T b = ((a+X*X) - std::sqrt((T(1)-X*X)*(a*a-X*X)) ) / ((a+X*X) + std::sqrt((T(1)-X*X)*(a*a-X*X)));
        
        // write result
        squaredPolesIt[0] = b;
        squaredPolesIt++;
        
        // check if sorting is needed; b should be ascending
        if (b < lastSquaredPole) {
            sortNeeded = true;
        }
        lastSquaredPole = b;
    }
    
    if (sortNeeded) {
        std::sort(squaredPoles, squaredPoles+numPairs);
    }
    
    return fail;
}



/*
 Parameters
 ----------
 numPairs : int
   Number of imaginary pole pairs of the filter. The order of the filter will be (2*numPairs + 1).
 As : T
   Stopband attenuation given in dB.
 squaredPoles : array
   [Result] Will contain the squared magnitudes of the purely imaginary poles (called beta in the literature).
   Size must be numPairs. Ascending order.
   (Note that the final filter contains a pole at z=0)
 
 ----------
 Filter design steps according to
 Lutovac's Book 'Filter Design for Signal Processing'
 9.7.2 Elliptic Minimal Q-Factor Transfer Functions,
   Design of Half-Band IIR Filters for Given Passband or Stopband Attenuation
 */
template <typename T>
int getEMQFHalfbandFilterByStopbandAttenuation(int numPairs, T As, T* squaredPoles) {
    T n = (numPairs*2) + 1;
    
    T L = std::pow(T(10), As/T(10)) - T(1);
    T t = ( (T(1) - std::pow(T(1) - (T(1)/(L*L)), T(1)/T(4)))   /   (T(1) + std::pow(T(1) - (T(1)/(L*L)), T(1)/T(4))) ) / T(2);
    T q = t + T(2)*std::pow(t,5) + T(15)*std::pow(t,9) + T(150)*std::pow(t,13);
    T g = std::exp(std::log(q)/n);
    
    T q0 = (g+std::pow(g,9)+std::pow(g,25)+std::pow(g,49)+std::pow(g,81)+std::pow(g,121)+std::pow(g,169))
         / (T(1) + T(2) * (std::pow(g,4)+std::pow(g,16)+std::pow(g,36)+std::pow(g,64)+std::pow(g,100)+std::pow(g,144)) );
    
    T a = T(1) / std::sqrt(T(1) - std::pow((T(1)-(T(2)*q0))/(T(1)+(T(2)*q0)), 4));
    
    return getEMQFHalfbandFilterBySelectivityFactor(numPairs, a, squaredPoles);
}


// Test data verified with high precision (mpmath) python implementation
/*
 getEMQFHalfbandFilterByStopbandAttenuation<double>(5, 44.3, &(betas.front()))
 [0]	double	0.16166891167226929	
 [1]	double	0.4798065522788309	
 [2]	double	0.73576067807786805	
 [3]	double	0.88490160827223596	
 [4]	double	0.96809905095653459
 */
    
} // namespace emqf
} // namespace afx