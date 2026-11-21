// Theorem 7/9 against the naive O(n²) search, in every iteration, on several instance types.
#include <functional>
#include <string>
#include <vector>

#include "sequence_model.hpp"
#include "twoopt/generators.hpp"
#include "twoopt/naive_two_opt.hpp"
#include "twoopt/repeated_two_opt.hpp"
#include "twoopt/treap_sequence.hpp"

using namespace twoopt;

namespace {

template <EdgeSequence Sequence>
void compareWithNaive(const Instance& instance, const Tour& start, bool checkAllSequences) {
  RepeatedTwoOpt<Sequence> fast(instance, start);
  NaiveTwoOpt naive(instance, start);
  while (true) {
    if (checkAllSequences) {
      for (EdgeId e = 0; e < fast.tour().size(); ++e) {
        test::checkSequence(fast.sequence(e), instance, fast.tour(), fast.sequence(e).reference());
      }
    }
    const auto move = fast.bestMove();
    const auto expected = naive.bestMove();
    CHECK(move == expected);  // same move, including the tie-break
    if (!move) break;
    CHECK(!fast.tour().adjacent(move->first, move->second));
    CHECK(twoMoveDelta(instance, fast.tour(), move->first, move->second) == move->delta);

    const Weight before = fast.tour().length(instance);
    fast.apply(*move);
    naive.apply(*expected);
    CHECK(fast.tour().order() == naive.tour().order());
    CHECK(fast.tour().length(instance) == before + move->delta);
  }
}

// Two independent runs must end in the same tour after the same number of moves.
template <EdgeSequence Sequence>
void compareFinalTours(const Instance& instance, const Tour& start) {
  RepeatedTwoOpt<Sequence> fast(instance, start);
  NaiveTwoOpt naive(instance, start);
  CHECK(runToLocalOptimum(fast) == runToLocalOptimum(naive));
  CHECK(fast.tour().order() == naive.tour().order());
}

}  // namespace

int main() {
  const std::vector<std::function<Instance(int, std::uint64_t)>> kinds = {
      [](int n, std::uint64_t seed) { return uniformRandomInstance(n, 1000, seed); },
      [](int n, std::uint64_t seed) { return uniformRandomInstance(n, 3, seed); },  // many ties
      [](int n, std::uint64_t seed) { return randomMetricInstance(n, 1000, seed); },
      [](int n, std::uint64_t seed) { return euclideanInstance(uniformPoints(n, seed)); },
      [](int n, std::uint64_t seed) { return euclideanInstance(clusteredPoints(n, 3, seed)); },
      [](int n, std::uint64_t) { return euclideanInstance(circlePoints(n)); },
      [](int n, std::uint64_t) { return euclideanInstance(gridPoints(n)); },
  };
  for (const auto& kind : kinds) {
    for (int n = 3; n <= 25; ++n) {
      for (std::uint64_t seed = 1; seed <= 4; ++seed) {
        compareWithNaive<TreapSequence>(kind(n, seed), Tour(randomOrder(n, seed)), true);
      }
    }
    for (int n : {60, 120}) compareWithNaive<TreapSequence>(kind(n, 9), Tour(randomOrder(n, 9)), false);
    compareFinalTours<TreapSequence>(kind(300, 5), Tour(randomOrder(300, 5)));
  }
  return 0;
}
