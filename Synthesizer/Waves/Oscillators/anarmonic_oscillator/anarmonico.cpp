#include"anarmonico.h"
#include"vettore_lineare.h"
#include"vettore.h"
#include"funzionevettBase.h"


vettore_lineare anarmonico::eval(const vettore_lineare& v, double &t) const {

vettore_lineare *a=new vettore_lineare(2);
a->setComponent(0, v.getComponent(1));
a->setComponent(1, -m_omega0*m_omega0*v.getComponent(0)-v.getComponent(0)*v.getComponent(0)/10.);

return *a;
}

vettore_lineare anarmonico::eval2(const vettore_lineare v, double t) const {

vettore_lineare *a=new vettore_lineare(2);
a->setComponent(0, v.getComponent(1));
a->setComponent(1, -m_omega0*m_omega0*v.getComponent(0)-v.getComponent(0)*v.getComponent(0)/10.);

return *a;
}
