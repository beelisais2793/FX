#include "utilities.h"
#include <cstdlib>

int randomInt(int low,int high)
{
    return rand() % ( high - low + 1 ) + low;
}

float randomFloat(float low, float high)
{
    return ((float)rand()/(float)RAND_MAX) * (high - low) + low;
}

bool flip(float p)
{
    return (double)rand() <  p * ((float)RAND_MAX + 1);
}
