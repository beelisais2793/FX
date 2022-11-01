#ifndef __vettore_lineare_h__
#define __vettore_lineare_h__

#include<iostream>
#include<cmath>
#include"vettore.h"

class vettore_lineare:public vettore{
public:


vettore_lineare(unsigned int);


virtual void print();
virtual void print(char *nomefile);
virtual void selection_sort();
void scambia_by_reference(double &a, double &b);

vettore_lineare operator+(const vettore_lineare&) const;
double operator*(const vettore_lineare&) const;
vettore_lineare operator*(const double) const;
};

#endif
