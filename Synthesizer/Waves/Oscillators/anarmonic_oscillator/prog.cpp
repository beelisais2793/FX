#include<iostream>
#include"vettore_lineare.h"
#include"equazioni.h"
#include"armonico.h"
#include"TApplication.h"
#include"TGraph.h"
#include"TAxis.h"
#include"TCanvas.h"
#include"anarmonico.h"

using namespace std;

int main(){

TApplication *app=new TApplication("app", 0, 0);

double omega=1.;
rungekutta rk;
eulero e;
armonico *o=new armonico(omega); 
vettore_lineare y(2);
y.setComponent(0, 0.);
y.setComponent(1, 1.);
double t=0.;
double h=0.01;
double tmax=25;
int nstep=(tmax/h +0.5);
TGraph *g=new TGraph();
TGraph *a=new TGraph();

TCanvas *c1=new TCanvas("c1", "c1");
TCanvas *c2=new TCanvas("c2", "c2");

for(int i=0;i<nstep;i++){
	g->SetPoint(i, t, y.getComponent(0));
	t+=h;
	y=rk.passo(t, y, h, o);

}

c1->cd();
g->GetXaxis()->SetTitle("t [s]");
g->GetYaxis()->SetTitle("x [m]");
g->SetTitle("x(t)");
g->Draw("AL");
c1->Print("a.pdf");

y.setComponent(0, 0.); //risetto le condizioni iniziali
y.setComponent(1, 1.);
t=0.;
h=0.001;
double t1,t2,t3;

for(int j=0;j< 100;j++){

	for(int i=0;i<nstep;i++){

	y=rk.passo(t, y, 0.01, o);
	t+=0.01;
	
	}

double v=y.getComponent(1);

	if(v>0){
		while(v>=0){
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;
		}
	t1=t;
		while(v<=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t2=t;
		while(v>=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t3=t;
	}

	else if(v<0){
		while(v<=0){
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;
		}
	t1=t;
		while(v>=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t2=t;
		while(v<=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t3=t;
	}

a->SetPoint(j, h, t3-t1);
h+=0.001;
t=0.;
y.setComponent(0, 0.); //risetto le condizioni iniziali
y.setComponent(1, 1.);
}

c2->cd();
a->GetXaxis()->SetTitle("passo h []");
a->GetYaxis()->SetTitle("Periodo [s]");
a->SetTitle("Periodo(passo)");
a->Draw("AL*");
c2->Print("b.pdf");

//metodo di eulero

y.setComponent(0, 0.);
y.setComponent(1, 1.);
t=0.;
h=0.01;
TGraph *g2=new TGraph();
TGraph *a2=new TGraph();

TCanvas *c3=new TCanvas("c3", "c3");
TCanvas *c4=new TCanvas("c4", "c4");

for(int i=0;i<nstep;i++){
	g2->SetPoint(i, t, y.getComponent(0));
	t+=h;
	y=e.passo(t, y, h, o);

}

c3->cd();
g2->GetXaxis()->SetTitle("t [s]");
g2->GetYaxis()->SetTitle("x [m]");
g2->SetTitle("x(t)");
g2->Draw("AL");
c3->Print("c.pdf");

y.setComponent(0, 0.); //risetto le condizioni iniziali
y.setComponent(1, 1.);
t=0.;
h=0.001;
double x1,x2,x3;

for(int j=0;j< 100;j++){

	for(int i=0;i<nstep;i++){

	y=e.passo(t, y, 0.01, o);
	t+=0.01;
	
	}

double v=y.getComponent(1);

	if(v>0){
		while(v>=0){
		y=e.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;
		}
	x1=y.getComponent(0);
		while(v<=0) {
		y=e.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	x2=-y.getComponent(0);
		while(v>=0) {
		y=e.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	x3=y.getComponent(0);
	}

	else if(v<0){
		while(v<=0){
		y=e.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;
		}
	x1=-y.getComponent(0);
		while(v>=0) {
		y=e.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	x2=y.getComponent(0);
		while(v<=0) {
		y=e.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	x3=-y.getComponent(0);
	}
double x=(x3-x1)/x1;
a2->SetPoint(j, h, x);
h+=0.001;
t=0.;
y.setComponent(0, 0.); //risetto le condizioni iniziali
y.setComponent(1, 1.);
}

c4->cd();
a2->GetXaxis()->SetTitle("passo h []");
a2->GetYaxis()->SetTitle("Periodo [s]");
a2->SetTitle("Periodo(passo)");
a2->Draw("AL*");
c4->Print("d.pdf");

//periodo al variare della velocità iniziale

y.setComponent(0, 0.);
y.setComponent(1, 1.);


TGraph *u=new TGraph();

TCanvas *c5=new TCanvas("c5", "c5");

t=0.;
h=0.001;

for(int j=0;j< 3;j++){

double p=static_cast<double>(j)/2.+0.5;

y.setComponent(1, p);

	for(int i=0;i<nstep;i++){

	y=rk.passo(t, y, 0.01, o);
	t+=0.01;
	
	}

double v=y.getComponent(1);

	if(v>0){
		while(v>=0){
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;
		}
	t1=t;
		while(v<=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t2=t;
		while(v>=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t3=t;
	}

	else if(v<0){
		while(v<=0){
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;
		}
	t1=t;
		while(v>=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t2=t;
		while(v<=0) {
		y=rk.passo(t, y, h, o);
		v=y.getComponent(1);
		t+=h;	
		}
	t3=t;
	}

u->SetPoint(j,p, t3-t1);
t=0.;
y.setComponent(0, 0.); //risetto le condizioni iniziali
y.setComponent(1, 1.);
}

c5->cd();
u->GetXaxis()->SetTitle("v0 []");
u->GetYaxis()->SetTitle("Periodo [s]");
u->SetTitle("Periodo(v0)");
u->Draw("AL*");
c5->Print("u.pdf");

//con termine anarmonico

anarmonico *o2=new anarmonico(omega);
y.setComponent(0, 0.);
y.setComponent(1, 1.);


TGraph *z=new TGraph();

TCanvas *c6=new TCanvas("c6", "c6");

t=0.;
h=0.001;

for(int j=0;j< 3;j++){

double p=static_cast<double>(j)/2.+0.5;

y.setComponent(1, p);

	for(int i=0;i<nstep;i++){

	y=rk.passo(t, y, 0.01, o2);
	t+=0.01;
	
	}

double v=y.getComponent(1);

	if(v>0){
		while(v>=0){
		y=rk.passo(t, y, h, o2);
		v=y.getComponent(1);
		t+=h;
		}
	t1=t;
		while(v<=0) {
		y=rk.passo(t, y, h, o2);
		v=y.getComponent(1);
		t+=h;	
		}
	t2=t;
		while(v>=0) {
		y=rk.passo(t, y, h, o2);
		v=y.getComponent(1);
		t+=h;	
		}
	t3=t;
	}

	else if(v<0){
		while(v<=0){
		y=rk.passo(t, y, h, o2);
		v=y.getComponent(1);
		t+=h;
		}
	t1=t;
		while(v>=0) {
		y=rk.passo(t, y, h, o2);
		v=y.getComponent(1);
		t+=h;	
		}
	t2=t;
		while(v<=0) {
		y=rk.passo(t, y, h, o2);
		v=y.getComponent(1);
		t+=h;	
		}
	t3=t;
	}

z->SetPoint(j,p, t3-t1);
t=0.;
y.setComponent(0, 0.); //risetto le condizioni iniziali
y.setComponent(1, 1.);
}

c6->cd();
z->GetXaxis()->SetTitle("v0 []");
z->GetYaxis()->SetTitle("Periodo [s]");
z->SetTitle("Periodo(v0)");
z->Draw("AL*");
c6->Print("z.pdf");



app->Run();
return 0;
}


