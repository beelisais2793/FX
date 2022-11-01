#ifndef _EQUAZIONI__H_
#define _EQUAZIONI__H_

#include"vettore_lineare.h"
#include"funzionevettBase.h"

class equazione{
public:
virtual vettore_lineare passo(double t, const vettore_lineare& inizio, double h, funzionevettBase *f) const =0;
};


class eulero:public equazione{
public:
virtual vettore_lineare passo(double t, const vettore_lineare& x, double h, funzionevettBase *f) const;
};

class rungekutta:public equazione{
public:
virtual vettore_lineare passo(double t, const vettore_lineare& x, double h, funzionevettBase *f) const;
};


#endif
