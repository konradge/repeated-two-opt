// The 7-point example of the seminar walkthrough (theorem7/ in the seminar folder).
#include <vector>

#include "check.hpp"
#include "twoopt/generators.hpp"
#include "twoopt/repeated_two_opt.hpp"
#include "twoopt/treap_sequence.hpp"

using namespace twoopt;

int main() {
  enum : Vertex { A, B, C, D, E, F, G };
  const Instance instance = euclideanInstance({{0, 0}, {7, 6}, {7, 2}, {4, -1}, {4, 9}, {0, 8}, {-2, 4}});
  RepeatedTwoOpt<TreapSequence> algorithm(instance, Tour({A, B, C, D, E, F, G}));
  CHECK(algorithm.tour().length(instance) == 39);

  // Edge ids start as positions: 0 = AB, 3 = DE.
  const auto move = algorithm.bestMove();
  CHECK(move == TwoMove({0, 3, -11}));

  algorithm.apply(*move);
  CHECK(algorithm.tour().order() == std::vector<Vertex>({A, D, C, B, E, F, G}));
  CHECK(algorithm.tour().length(instance) == 28);
  CHECK(!algorithm.bestMove());
  return 0;
}
