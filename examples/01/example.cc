#include <cassert>
#include <iostream>
#include "ea/genotype.h"

int main() {
  using namespace libbear;
  
  const genotype g0{42, 'c', .42};
  const genotype g1{42, 'c', .042};
  std::cout << g0 << std::endl;
  std::cout << *g0[0] << std::endl;
  assert(g0 != g1);
}
