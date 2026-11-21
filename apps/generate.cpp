// Writes an instance or a start tour to stdout.
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "twoopt/generators.hpp"
#include "twoopt/tour.hpp"

using namespace twoopt;

namespace {

constexpr const char* kUsage =
    "usage: generate instance <uniform|metric|euclidean|clustered|circle|grid> <n> [seed] [clusters]\n"
    "       generate tour <identity|random> <n> [seed]\n";

Instance makeInstance(const std::string& kind, int n, std::uint64_t seed, int clusters) {
  constexpr Weight kMaxWeight = 1000;
  if (kind == "uniform") return uniformRandomInstance(n, kMaxWeight, seed);
  if (kind == "metric") return randomMetricInstance(n, kMaxWeight, seed);
  if (kind == "euclidean") return euclideanInstance(uniformPoints(n, seed));
  if (kind == "clustered") return euclideanInstance(clusteredPoints(n, clusters, seed));
  if (kind == "circle") return euclideanInstance(circlePoints(n));
  if (kind == "grid") return euclideanInstance(gridPoints(n));
  throw std::invalid_argument("unknown instance kind: " + kind);
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const std::vector<std::string> args(argv + 1, argv + argc);
    if (args.size() < 3) throw std::invalid_argument("missing arguments");
    const std::string& what = args[0];
    const std::string& kind = args[1];
    const int n = std::stoi(args[2]);
    const std::uint64_t seed = args.size() > 3 ? std::stoull(args[3]) : 1;
    const int clusters = args.size() > 4 ? std::stoi(args[4]) : 5;

    if (what == "instance") {
      makeInstance(kind, n, seed, clusters).write(std::cout);
    } else if (what == "tour" && kind == "identity") {
      Tour(identityOrder(n)).write(std::cout);
    } else if (what == "tour" && kind == "random") {
      Tour(randomOrder(n, seed)).write(std::cout);
    } else {
      throw std::invalid_argument("unknown command");
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << '\n' << kUsage;
    return 1;
  }
}
