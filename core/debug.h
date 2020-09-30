#ifndef LIBBEAR_CORE_DEBUG_H
#define LIBBEAR_CORE_DEBUG_H

#include <iostream>

#ifdef NDEBUG
  static const bool debug = false;
#else
  static const bool debug = true;
#endif

#define DEBUG_MSG(x) do { std::cerr << "DEBUG: " << x << std::endl; } while (0)

#endif // LIBBEAR_CORE_DEBUG_H
