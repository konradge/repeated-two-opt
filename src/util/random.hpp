#pragma once

#include <cmath>
#include <cstdint>
#include <numbers>

namespace twoopt {

// Small deterministic generator (splitmix64), identical on every platform.
class Random {
 public:
  explicit Random(std::uint64_t seed) : state_(seed) {}

  std::uint64_t next() {
    std::uint64_t z = (state_ += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
  }

  // Uniform in [0, bound); the modulo bias is negligible for our bounds.
  std::uint64_t below(std::uint64_t bound) { return next() % bound; }

  // Uniform in [0, 1).
  double uniform() { return static_cast<double>(next() >> 11) * 0x1.0p-53; }

  // Standard normal (Box-Muller).
  double normal() {
    const double u = 1.0 - uniform();
    const double v = uniform();
    return std::sqrt(-2.0 * std::log(u)) * std::cos(2.0 * std::numbers::pi * v);
  }

 private:
  std::uint64_t state_;
};

}  // namespace twoopt
