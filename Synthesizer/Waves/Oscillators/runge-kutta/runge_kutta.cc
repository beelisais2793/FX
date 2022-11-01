#include "runge_kutta.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>

	/* costruttore */
	oscillatore::oscillatore(long double a, long double b, unsigned long int N) {
		/* calcolo l'incremento */
		h = (long double) (b - a)/N;
		x.n = N;
	}
	
	/* distruttore */
	oscillatore::~oscillatore (void) {}
	
	/* risolve il sistema */
	void oscillatore::solve (long double init[]) {
		/* faccio evolvere il sistema */
		for (unsigned long int i = 0; i <= x.n; i++ ) {
			/* impongo le condizioni iniziali */
			if ( i == 0 ) {
				for (unsigned short int j = 0; j < 2; j += 1)
					x.data[j] = init[j];
			} else
				evolve(i);

			/* stampo i risultati per il grafico */	
			printf("%Lg\t%Lg\t%Lg\n", i*h, x.data[0], x.data[1]);
		}
	}
	
	void oscillatore::evolve (unsigned long int i) {
		/* calcolo gli incrementi */
		k_evolve(i*h);
		
		for (unsigned short int j = 0; j < 2; j++ ) {
			for (unsigned short int a = 0; a < 4; a++ )
				x.data[j] += k.c[a]*k.data[a][j];
		}
	}

	/* modifica i 'k.data[][]' */
	void oscillatore::k_evolve (long double t) {
		/* inizializzo il tempo */
		T = t;
		for (unsigned short int l = 0; l < 4; l++ ) {
			if ( l == 0 ) {
				for (unsigned int i = 0; i < 2; i += 1)
					a[i] = x.data[i];
			} else {
				for (unsigned short int i = 0; i < 2; i += 1)
					a[i] = x.data[i] + k.peso[l-1]*k.data[l-1][i];
				/* incremento il tempo */
				T = t + k.peso[l-1]*h;
			}

			/* calcolo i 'k.data[][]' */
			k.data[l][0] = h*a[1];
			k.data[l][1] = h*(-sinl(a[0]) - q*a[1] + b*cosl(w*T));
		}
	}
