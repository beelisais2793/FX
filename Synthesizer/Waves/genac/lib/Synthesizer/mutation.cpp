#include "mutation.h"
#include "chromosome.h"
#include "utilities.h"

void Mutation::fullyRandomized(Chromosome* c)
{
    int n = 0;
    int r = 0;
    n = c->size();

    for(int i = 0; i < n; ++i) {
        r = randomInt(1, c->size()-1);
        if(r < c->isize()) {
            (*c)[r] = randomInt(c->imin(), c->imax());
        } else {
            (*c)[r] = randomInt(c->emin(), c->emax());
        }
    }

    c->canonicalize();
}

void Mutation::shortCircuit(Chromosome* c)
{
   int r1 = randomInt(1,c->size()-1);
   int r2 = 1;
   if(r1 < c->isize()){
       r2 = randomInt(1,c->isize()-1);
   } else {
       r2 = randomInt(c->isize(),c->size()-1);
   }

   (*c)[r1] = (*c)[r2];
   c->canonicalize();
}

void Mutation::swap(Chromosome* c)
{
   int r1 = randomInt(1,c->size()-1);
   int r2 = 1;
   if(r1 < c->isize()){
        r2 = randomInt(1,c->isize()-1);
   } else {
       r2 = randomInt(c->isize(),c->size()-1);
   }

   int temp = (*c)[r1];
   (*c)[r1] = (*c)[r2];
   (*c)[r2] = temp;
   c->canonicalize();
}

