#ifndef __generatore_h__
#define __generatore_h__
#include<cmath>
#include "funzioneBase.h"
#include "gauss.h"

class generatore{

public:

generatore(int seed);
void setA(int a){m_a=a;}
void setC(int c){m_c=c;}
void setM(int m){m_m=m;}
double rand();
double gaussiana(double media,double sigma);
void setSeed(int seed){m_seed=seed;}
double getSeed() const {return m_seed;}
double exp(double rate);
double uniform(double a, double b);
double boxmuller(double media, double sigma);

private:

unsigned int m_a;
unsigned int m_c;
unsigned int m_m;
unsigned int m_seed;

};

#endif
