#pragma once

#include <cstdint>
#include <vector>

#include "twoopt/instance.hpp"
#include "twoopt/types.hpp"

namespace twoopt {

struct Point {
  double x = 0;
  double y = 0;
};

// Weights: independent uniform integers in [1, maxWeight].
Instance uniformRandomInstance(int n, Weight maxWeight, std::uint64_t seed);
// Uniform weights closed under shortest paths, hence metric. O(n³).
Instance randomMetricInstance(int n, Weight maxWeight, std::uint64_t seed);
// Weights: Euclidean distances times `scale`, rounded to integers.
Instance euclideanInstance(const std::vector<Point>& points, double scale = 1.0);

// Point sets in [0, 1000]².
std::vector<Point> uniformPoints(int n, std::uint64_t seed);
std::vector<Point> clusteredPoints(int n, int clusters, std::uint64_t seed);
std::vector<Point> circlePoints(int n);  // regular n-gon
std::vector<Point> gridPoints(int n);    // first n points of a square grid, row by row

std::vector<Vertex> identityOrder(int n);
std::vector<Vertex> randomOrder(int n, std::uint64_t seed);

}  // namespace twoopt
