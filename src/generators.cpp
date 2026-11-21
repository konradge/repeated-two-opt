#include "twoopt/generators.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>

#include "twoopt/random.hpp"

namespace twoopt {

namespace {

constexpr double kSide = 1000.0;

}  // namespace

Instance uniformRandomInstance(int n, Weight maxWeight, std::uint64_t seed) {
  Random random(seed);
  Instance instance(n);
  for (Vertex u = 0; u < n; ++u) {
    for (Vertex v = u + 1; v < n; ++v) instance.set(u, v, 1 + static_cast<Weight>(random.below(maxWeight)));
  }
  return instance;
}

Instance randomMetricInstance(int n, Weight maxWeight, std::uint64_t seed) {
  Instance instance = uniformRandomInstance(n, maxWeight, seed);
  for (Vertex k = 0; k < n; ++k) {
    for (Vertex u = 0; u < n; ++u) {
      for (Vertex v = u + 1; v < n; ++v) {
        const Weight viaK = instance(u, k) + instance(k, v);
        if (viaK < instance(u, v)) instance.set(u, v, viaK);
      }
    }
  }
  return instance;
}

Instance euclideanInstance(const std::vector<Point>& points, double scale) {
  const int n = static_cast<int>(points.size());
  Instance instance(n);
  for (Vertex u = 0; u < n; ++u) {
    for (Vertex v = u + 1; v < n; ++v) {
      const double distance = std::hypot(points[u].x - points[v].x, points[u].y - points[v].y);
      instance.set(u, v, std::llround(scale * distance));
    }
  }
  return instance;
}

std::vector<Point> uniformPoints(int n, std::uint64_t seed) {
  Random random(seed);
  std::vector<Point> points(n);
  for (Point& p : points) p = {kSide * random.uniform(), kSide * random.uniform()};
  return points;
}

std::vector<Point> clusteredPoints(int n, int clusters, std::uint64_t seed) {
  Random random(seed);
  const std::vector<Point> centers = uniformPoints(std::max(clusters, 1), seed ^ 0x5eed);
  const double spread = kSide / 20;
  std::vector<Point> points(n);
  for (Point& p : points) {
    const Point& c = centers[random.below(centers.size())];
    p = {std::clamp(c.x + spread * random.normal(), 0.0, kSide), std::clamp(c.y + spread * random.normal(), 0.0, kSide)};
  }
  return points;
}

std::vector<Point> circlePoints(int n) {
  std::vector<Point> points(n);
  for (int i = 0; i < n; ++i) {
    const double angle = 2 * std::numbers::pi * i / n;
    points[i] = {kSide / 2 * (1 + std::cos(angle)), kSide / 2 * (1 + std::sin(angle))};
  }
  return points;
}

std::vector<Point> gridPoints(int n) {
  const int columns = std::max(1, static_cast<int>(std::ceil(std::sqrt(n))));
  const double spacing = kSide / columns;
  std::vector<Point> points(n);
  for (int i = 0; i < n; ++i) points[i] = {spacing * (i % columns), spacing * (i / columns)};
  return points;
}

std::vector<Vertex> identityOrder(int n) {
  std::vector<Vertex> order(n);
  std::iota(order.begin(), order.end(), 0);
  return order;
}

std::vector<Vertex> randomOrder(int n, std::uint64_t seed) {
  Random random(seed);
  std::vector<Vertex> order = identityOrder(n);
  for (int i = n - 1; i > 0; --i) std::swap(order[i], order[random.below(i + 1)]);
  return order;
}

}  // namespace twoopt
