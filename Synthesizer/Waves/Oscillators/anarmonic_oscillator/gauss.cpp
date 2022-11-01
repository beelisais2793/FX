#include "funzioneBase.h"
#include <iostream>
#include <cmath>
#include "gauss.h"

using namespace std;

gauss::gauss(double media, double sigma) {

	m_media=media;
	m_sigma=sigma;
}

double gauss::eval(double x) const {

	double y=1/(sqrt(2*M_PI)*m_sigma)*exp(-(x-m_media)*(x-m_media)/(2.*m_sigma*m_sigma));
	return y;
}
