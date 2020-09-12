#include <cassert>
#include <iostream>
#include <libbear/ea/genotype.h>

int main() {
  using namespace libbear;
  
  const genotype g0{gene{42, 0, 99}, gene{'c', 'a', 'z'}, gene{.42, 0., 1.}};
  const genotype g1{gene{42, 0, 99}, gene{'c', 'a', 'z'}, gene{.042, 0., 1.}};
  std::cout << g0 << std::endl;
  std::cout << *g0[0] << std::endl;
  assert(g0 != g1);
}
