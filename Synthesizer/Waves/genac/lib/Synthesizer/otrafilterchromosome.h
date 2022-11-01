#ifndef OTRAFILTERCHROMOSOME_H
#define OTRAFILTERCHROMOSOME_H
#include "chromosome.h"

/* E[0]  : p
 * E[1]  : n
 * E[2]  : z
 * E[3]  : Vs
 * I[0]  : 0 (always GND)
 * 0     : GND
 */

class OtraFilterChromosome : public Chromosome
{
public:
    OtraFilterChromosome(int ncapacitors, int nresistors);

    virtual ~OtraFilterChromosome() =default;

    virtual QString toNetlist() const;
    virtual int esize() const;
    virtual int nShortCircuits() const;
};

#endif // OTRAFILTERCHROMOSOME_H
