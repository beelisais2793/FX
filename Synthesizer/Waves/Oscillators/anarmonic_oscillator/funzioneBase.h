#ifndef __funzioneBase_h__
#define __funzioneBase_h__
#include <iostream>

using namespace std;

class funzioneBase{ //non ha bisogno di un file.cpp nè di un file oggetto .o nel makefile

public:

virtual double eval(double x)const =0;
};

#endif //__funzioneBase_h__
