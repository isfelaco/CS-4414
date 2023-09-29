#include "rand.h"

static unsigned long seed = 1;

unsigned long
rand(void)
{
  seed = (seed * 48271UL) % 2147483647UL;
  return seed;
}