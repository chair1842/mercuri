#include <stdint.h>

// random.c
static unsigned int rand_seed = 1;

void srand(unsigned int seed) {
    rand_seed = seed;
}

int rand() {
    // Constants from Numerical Recipes
    rand_seed = rand_seed * 1664525 + 1013904223;
    return (rand_seed >> 16) & 0x7FFF;  // Return 15-bit number (like ANSI C rand)
}

int rand_in_range(int min, int max) {
    if (max <= min) return min; // or return 0 or some error value
    return min + rand() % (max - min + 1);
}
