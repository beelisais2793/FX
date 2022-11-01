#ifndef CCIIFILTERCHROMOSOME_H
#define CCIIFILTERCHROMOSOME_H
#include "chromosome.h"

/* E[0]  : X
 * E[1]  : Y
 * E[2]  : Z
 * E[3]  : Vs
 * I[0]  : 0 (always GND)
 * 0     : GND
 */

class CciiFilterChromosome : public Chromosome
{
public:
    CciiFilterChromosome(int ncapacitors, int nresistors);

    virtual ~CciiFilterChromosome() =default;

    virtual QString toNetlist() const;
    virtual int esize() const;
    virtual int nShortCircuits() const;
};

#endif // CCIIFILTERCHROMOSOME_H
