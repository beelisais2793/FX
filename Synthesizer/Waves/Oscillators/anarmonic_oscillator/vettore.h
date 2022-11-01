#ifndef __vettore_h__
#define __vettore_h__

class vettore {

public:

vettore();
vettore(unsigned int n);
~vettore(){delete[] m_v;}

unsigned int getN() const {return m_n;}
void setComponent(unsigned int, double);
double getComponent(unsigned int) const;

vettore(const vettore&); //copy constructor: scrivendo vettore b=a, creo un vettore uguale al precedente
vettore& operator=(const vettore&); //overloading dell'operatore di assegnazione: infatti non funziona con i puntatori, perchè finirebbero nella stessa zona di memoria; infatti nell'esercizio di test, io creo a, poi metto b=a, poi modifico b: cosa succede? che si modifica anche a!

virtual void print()=0;
virtual void print(char *nomefile)=0;
virtual void selection_sort()=0;
double modulo();

protected:

unsigned int m_n;
double* m_v;

};


#endif //__vettore_h__
