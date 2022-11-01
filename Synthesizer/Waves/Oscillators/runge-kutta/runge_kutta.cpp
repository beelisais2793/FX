#include "runge_kutta.cc"

using namespace std;

int main (void) {
	/* vettore con le condizioni iniziali per l'angolo e la sua velocita' */
	long double init[2] = {M_PI/6, 0.};

	oscillatore sistema (0., 75., 2250);
	/* risolve e stampa coordinate */
	sistema.solve(init);
	
	exit(EXIT_SUCCESS);
}
