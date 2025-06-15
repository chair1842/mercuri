#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

void srand(unsigned int seed);
int rand();
int rand_in_range(int min, int max);

#endif
