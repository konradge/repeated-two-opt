#pragma once

#include <cstdio>
#include <cstdlib>

// Like assert, but also active in release builds.
#define CHECK(condition)                                                              \
  do {                                                                                \
    if (!(condition)) {                                                               \
      std::fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #condition); \
      std::exit(1);                                                                   \
    }                                                                                 \
  } while (false)
