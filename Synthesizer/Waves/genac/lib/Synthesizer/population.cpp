#include "population.h"

void Population::initialize(int size, Chromosome::Type type,
                int ncapacitors, int nresistors,
                bool random)
{
    reserve(size);
    for (int i = 0; i < size; ++i) {
        Chromosome *c = Chromosome::create(type, ncapacitors,
                                           nresistors, random);
        append(c);
    }
}

Population::~Population()
{
    for (int i = 0; i < size(); ++i) {
        delete (*this)[i];
    }
}
