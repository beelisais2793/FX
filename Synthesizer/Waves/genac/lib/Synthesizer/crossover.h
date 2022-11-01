#ifndef CROSSOVER_H
#define CROSSOVER_H

class Chromosome;

class Crossover
{
public:
    static void onePoint(const Chromosome *parent1, const Chromosome *parent2,
                                Chromosome **child1, Chromosome **child2);
    static void twoPoint(const Chromosome *parent1, const Chromosome *parent2,
                                Chromosome **child1, Chromosome **child2);
    static void uniform(const Chromosome *parent1, const Chromosome *parent2,
                                Chromosome **child1, Chromosome **child2);
    static void singleGene(const Chromosome *parent1, const Chromosome *parent2,
                                Chromosome **child1, Chromosome **child2);
    static void multiGene(const Chromosome *parent1, const Chromosome *parent2,
                                Chromosome **child1, Chromosome **child2);
};

#endif // CROSSOVER_H
