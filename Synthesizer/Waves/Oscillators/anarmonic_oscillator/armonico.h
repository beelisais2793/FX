#ifndef _ARMONICO__H_
#define _ARMONICO__H_

#include"funzionevettBase.h"
#include"vettore_lineare.h"

class armonico:public funzionevettBase{
public:
	armonico(double omega0) {m_omega0=omega0;}
	~armonico();

	virtual vettore_lineare eval(const vettore_lineare& v,double &t) const;
virtual vettore_lineare eval2(const vettore_lineare v, double t) const;

private:
double m_omega0;
};

#endif
