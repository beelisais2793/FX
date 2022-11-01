#ifndef __oscillatore__
#define __oscillatore__

class oscillatore {
	public:
		/* risolve il sistema tra i tempi 'a' e 'b' campionando in 'N' p.ti */
		oscillatore (long double a, long double b, unsigned long int N);
		/* distruttore */
		virtual ~oscillatore ();
		
		/* accetta lo stato iniziale come argomento*/
		void solve (long double init[]);

	private:
		/* incremento "infinitesimo" nel tempo */
		long double h;
		/* parametri della funzione 'f' */
		long double q = .1, b = 1.5, w = 25;
		
		/* vettore ausiliario, variabile ausiliaria per il tempo */
		long double a[2], T;

		/* incrementi k_i */
		struct {
			/* quattro vettori da due componenti */
			long double data[4][2];
			/* uso questi per calcolare k_i da k_{i-1}: k_i = h*f( ... ) */
			long double peso[3] = {.5, .5, 1.};
			/*
			 * questi sono i coefficienti per calcolare x_{n+1} da x_n:
			 * x_{n+1} = x_n + (k_1 + 2*k_2 + 2*k_3 + k_4)/6
			 */
			long double c[4] = {
				(long double) 1/6,
				(long double) 1/3,
				(long double) 1/3,
				(long double) 1/6
				};
		} k;

		struct {
			/* valori di $\theta$ e $\dot{\theta} $ al tempo i*h */
			long double data[2];			
			/* numero campionamenti */
			unsigned long int n;
		} x;
		
		/* evolve il sistema */
		void evolve (unsigned long int i);
		/* calcola i $k_i$ */
		void k_evolve (long double t);
};

#endif /* __oscillatore__ */
