#ifndef CFOAFILTERCHROMOSOME_H
#define CFOAFILTERCHROMOSOME_H
#include "chromosome.h"

/* E[0]  : X
 * E[1]  : Y
 * E[2]  : Z
 * E[3]  : W
 * E[4]  : Vs
 * I[0]  : 0 (always GND)
 * 0     : GND
 */

class CfoaFilterChromosome : public Chromosome
{
public:
    CfoaFilterChromosome(int ncapacitors, int nresistors);

    virtual ~CfoaFilterChromosome() =default;

    virtual QString toNetlist() const;
    virtual int esize() const;
    virtual int nShortCircuits() const;
};

#endif // CFOAFILTERCHROMOSOME_H
