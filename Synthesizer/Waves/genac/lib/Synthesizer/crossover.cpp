#include "crossover.h"
#include "chromosome.h"
#include "utilities.h"

void Crossover::onePoint(const Chromosome *parent1, const Chromosome *parent2,
         Chromosome **child1, Chromosome **child2)
{
    int size = parent1->size();
    int r = randomInt(1, size - 1);
    *child1 = Chromosome::create(parent1->type(), parent1->ncapacitors(),
                                        parent1->nresistors(), false);
    *child2 = Chromosome::create(parent1->type(), parent1->ncapacitors(),
                                        parent1->nresistors(), false);
    for (int i = 0; i < size; ++i) {
        if (i <= r) {
            (**child1)[i] = parent1->at(i);
            (**child2)[i] = parent2->at(i);
        } else {
            (**child1)[i] = parent2->at(i);
            (**child2)[i] = parent1->at(i);
        }
    }
    (*child1)->canonicalize();
    (*child2)->canonicalize();
}

void Crossover::twoPoint(const Chromosome *parent1, const Chromosome *parent2,
                     Chromosome **child1, Chromosome **child2)
{
    int size = parent1->size() ;
    int a1 = randomInt(1, size-1);
    int a2 = randomInt(1, size-1);
    int r1 = (a1 < a2) ? a1 : a2;
    int r2 = (a1 < a2) ? a2 : a1;
    *child1 = Chromosome::create(parent1->type(), parent1->ncapacitors(),
                                 parent1->nresistors(), false);
    *child2 = Chromosome::create(parent1->type(), parent1->ncapacitors(),
                                 parent1->nresistors(), false);
    for (int i = 0; i < size; ++i) {
        if (i >= r1 && i <= r2) {
            (**child1)[i] = parent2->at(i);
            (**child2)[i] = parent1->at(i);
        } else {
            (**child1)[i] = parent1->at(i);
            (**child2)[i] = parent2->at(i);
        }
    }
    (*child1)->canonicalize();
    (*child2)->canonicalize();
}

void Crossover::uniform(const Chromosome *parent1, const Chromosome *parent2,
                    Chromosome **child1, Chromosome **child2)
{
    *child1 = Chromosome::create(parent1->type(), parent1->ncapacitors(),
                                 parent1->nresistors(), false);
    *child2 = Chromosome::create(parent1->type(), parent1->ncapacitors(),
                                 parent1->nresistors(), false);
    for (int i = 0; i < parent1->size(); ++i) {
        if (flip(0.5)) {
            (**child1)[i] = parent1->at(i);
            (**child2)[i] = parent2->at(i);
        } else {
            (**child1)[i] = parent2->at(i);
            (**child2)[i] = parent1->at(i);
        }
    }
    (*child1)->canonicalize();
    (*child2)->canonicalize();
}

void Crossover::singleGene(const Chromosome *parent1, const Chromosome *parent2,
                       Chromosome **child1, Chromosome **child2)
{
    int r = randomInt(1, parent1->size() - 1);
    *child1 = Chromosome::clone(*parent1);
    *child2 = Chromosome::clone(*parent2);
    (**child1)[r] = parent2->at(r);
    (**child2)[r] = parent1->at(r);
    (*child1)->canonicalize();
    (*child2)->canonicalize();
}

void Crossover::multiGene(const Chromosome *parent1, const Chromosome *parent2,
                       Chromosome **child1, Chromosome **child2)
{
    int n = randomInt(1, parent1->size() / 2);

    *child1 = Chromosome::clone(*parent1);
    *child2 = Chromosome::clone(*parent2);
    for (int i = 0;  i < n; ++i) {
        int r = randomInt(1, parent1->size() - 1);
        (**child1)[r] = parent2->at(r);
        (**child2)[r] = parent1->at(r);
    }
    (*child1)->canonicalize();
    (*child2)->canonicalize();
}

