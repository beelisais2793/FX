#ifndef SELECTION_H
#define SELECTION_H

class Population;
class Chromosome;

class Selection
{
public:
    static void rouletteWheel(Population* population, Chromosome** parent);
    static void rank(Population* population, Chromosome** parent);
    static void tournament(Population* population, Chromosome** parent);
};

#endif /* SELECTION_H */
