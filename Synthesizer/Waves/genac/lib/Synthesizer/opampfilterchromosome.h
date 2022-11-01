#ifndef OPAMPFILTERCHROMOSOME_H
#define OPAMPFILTERCHROMOSOME_H
#include "chromosome.h"

/* E[0]  : +
 * E[1]  : -
 * E[2]  : OUT
 * E[3]  : Vs
 * I[0]  : 0 (always GND)
 * 0     : GND
 */

class OpampFilterChromosome : public Chromosome
{
public:
    OpampFilterChromosome(int ncapacitors, int nresistors);

    virtual ~OpampFilterChromosome() =default;

    virtual QString toNetlist() const;
    virtual int esize() const;
    virtual int nShortCircuits() const;
};

#endif // OPAMPFILTERCHROMOSOME_H
