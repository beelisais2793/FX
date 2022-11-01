#ifndef MUTATION_H
#define MUTATION_H

class Chromosome;

class Mutation
{
public:
    static void fullyRandomized(Chromosome* c);
    static void shortCircuit(Chromosome* c);
    static void swap(Chromosome* c);
};

#endif // MUTATION_H
