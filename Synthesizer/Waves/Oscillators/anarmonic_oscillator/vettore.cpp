#include "vettore.h"
#include<iostream>
#include<cmath>

using namespace std;

double vettore::modulo() {

double accu=0.;
for (int i=0; i<m_n; i++){
	accu+=pow(m_v[i],2);
}
return sqrt(accu);
}


vettore::vettore(){
m_n=0;
m_v=0;
}

vettore::vettore(unsigned int n) {

	m_n=n;
	m_v=new double[m_n];

}

void vettore::setComponent(unsigned int i, double x) {
	
	if (i<m_n)
		m_v[i]=x;
	else 
		cout<<"La dimensione del vettore e' " << m_n << endl;
}

double vettore::getComponent(unsigned int i) const {

	
	if (i<m_n)
		return m_v[i];
	else 
		cout<<"La dimensione del vettore e' " << m_n << endl;	

}

vettore::vettore(const vettore& v){

	m_n=v.m_n;
	m_v=new double[m_n];
	for (int i=0; i<m_n; i++) m_v[i]=v.m_v[i]; 

}

vettore& vettore::operator=(const vettore& v) {

	m_n=v.m_n;
	if(m_v) delete[] m_v;
	m_v=new double[m_n];
	for(int i=0; i<m_n; i++) m_v[i]=v.m_v[i];
	return *this;
}


