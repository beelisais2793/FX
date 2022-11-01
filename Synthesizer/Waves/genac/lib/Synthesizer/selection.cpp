#include "selection.h"
#include "population.h"
#include "chromosome.h"
#include "utilities.h"
#include "dbg.h"

#define TOURNAMENT_SIZE (4)

void Selection::rouletteWheel(Population* population, Chromosome** parent)
{
    float sum = 0.0f;
    QList<Chromosome*>::Iterator it = population->begin();
    while (it != population->end()) {
        sum += (*it)->fitness();
        it++;
    }

    float r = randomFloat(0.0f, sum);
    sum = 0.0f;
    it = population->begin();
    while (it != population->end()) {
        sum += (*it)->fitness();
        if (r <= sum) {
            *parent = *it;
            break;
        }
        it++;
    }
}

// Precondition: population should be sorted.
void Selection::rank(Population* population, Chromosome** parent)
{
    int size = population->size();
    int sum = size * (size + 1) / 2;
    int r = randomInt(1, sum);
    int i = 1;
    sum = 0;
    QList<Chromosome*>::Iterator it = population->begin();
    while (it != population->end()) {
        sum += i;
        if (r <= sum) {
            *parent = (*population)[i - 1];
            break;
        }
        it++;
        i++;
    }
}

void Selection::tournament(Population* population, Chromosome** parent)
{
    int size = population->size();
    int r = 0;
    Chromosome* group[TOURNAMENT_SIZE] = {0};
    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        r = randomInt(0, size-1);
        group[i] = (*population)[r];
    }

    Chromosome *winner = group[0];
    for (int i = 1; i < TOURNAMENT_SIZE; ++i) {
        if (winner->fitness() < group[i]->fitness()) {
            winner = group[i];
        }
    }
    *parent = winner;
}
