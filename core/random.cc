#include <random>
#include <libbear/core/random.h>

std::mt19937&
libbear::
random_engine()
{
  static std::mt19937 engine{ std::random_device{}() };
  return engine;
}

