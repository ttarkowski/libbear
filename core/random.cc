#include <random>
#include "random.h"

std::mt19937&
libbear::
random_engine()
{
  static std::mt19937 engine{ std::random_device{}() };
  return engine;
}

bool
libbear::
success(probability success_probability)
{
  return std::bernoulli_distribution{ success_probability }(random_engine());
}
