#include"equazioni.h"
#include"funzionevettBase.h"
#include"vettore_lineare.h"


vettore_lineare eulero::passo(double t, const vettore_lineare& x, double h, funzionevettBase *f) const {

vettore_lineare *s=new vettore_lineare(x.getN());
*s=x+f->eval(x,t)*h;

return *s;
}

vettore_lineare rungekutta::passo(double t, const vettore_lineare& x, double h, funzionevettBase *f) const {

vettore_lineare k1(x.getN());
vettore_lineare k2(x.getN());
vettore_lineare k3(x.getN());
vettore_lineare k4(x.getN());

k1=f->eval(x,t);
k2=f->eval2(x+k1*(h/2),t+h/2);
k3=f->eval2(x+k2*(h/2),t+h/2);
k4=f->eval2(x+k3*h,t+h);

vettore_lineare a(x.getN());
a=x + (k1+k2*2+k3*2+k4)*(h/6);
return a;

}
