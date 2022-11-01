#ifndef __gauss_h__
#define __gauss_h__
#include <iostream>
#include <cmath>
#include "funzioneBase.h"

using namespace std;

class gauss:public funzioneBase {

public:

gauss(double media, double sigma);
virtual double eval(double x)const;

protected:

double m_media;
double m_sigma;

};

#endif
