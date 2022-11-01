#ifndef __FUNZIONEVETTBASE_H__
#define __FUNZIONEVETTBASE_H__

#include"vettore.h"
#include"vettore_lineare.h"

class funzionevettBase{
public:

virtual vettore_lineare eval(const vettore_lineare& v, double &t) const=0;
virtual vettore_lineare eval2(const vettore_lineare v, double t) const=0;
};


#endif
