#include<iostream>
#include<cmath>
#include"vettore.h"
#include"vettore_lineare.h"
#include<fstream>

using namespace std;




vettore_lineare::vettore_lineare(unsigned int n):vettore(n) {}

void vettore_lineare::print() {

	for (int i=0; i<m_n; i++) cout<<m_v[i]<<endl;
}

void vettore_lineare::print(char *nomefile) {

	ofstream stream;
	stream.open(nomefile);	
	for (int i=0; i<m_n; i++) {stream<<m_v[i]<<endl;}
	stream.close();
}


void vettore_lineare::scambia_by_reference(double &a, double &b) {

	double c=0;
	c=a;
	a=b;
	b=c;
}

void vettore_lineare::selection_sort()  {

	for (int i=0; i<m_n; i++) {
		for (int j=i+1; j<m_n; j++) {
			if (m_v[j]<m_v[i])
				scambia_by_reference(m_v[i], m_v[j]);
		}
	}
}

vettore_lineare vettore_lineare::operator+(const vettore_lineare& b) const {

vettore_lineare v(m_n);

/*if (m_n!=b.getN()) {

cout<<"errore: due vettori sommati devono avere la stessa dimensione!"<<endl;
return -2;
}*/

for (unsigned int i=0; i<m_n;i++) 
	v.setComponent(i, this->getComponent(i)+b.getComponent(i));
return v;
}

double vettore_lineare::operator*(const vettore_lineare& b) const {

double x=0.;

if (m_n!=b.getN()) {
cout<<"errore: i due vettori devono avere la stessa dimensione!"<<endl;
return -2;
}

for (unsigned int i=0; i<m_n;i++) 
	x+=m_v[i]*b.getComponent(i);
return x;
}

vettore_lineare vettore_lineare::operator*(const double a) const {

vettore_lineare v(m_n);

for (unsigned int i=0; i<m_n;i++) 
	v.setComponent(i,m_v[i]*a);

return v;

}

