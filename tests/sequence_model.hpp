#pragma once

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

#include "check.hpp"
#include "twoopt/edge_sequence.hpp"
#include "twoopt/tour.hpp"
#include "twoopt/two_move.hpp"

namespace twoopt::test {

// P(reference) taken directly from the tour, directed from reference.to to reference.from.
inline std::vector<Edge> expectedPath(const Tour& tour, Edge reference) {
  std::vector<Edge> path;
  tour.pathAfter(reference.id, path);
  if (tour.edge(reference.id).from != reference.from) {
    std::reverse(path.begin(), path.end());
    for (Edge& edge : path) std::swap(edge.from, edge.to);
  }
  return path;
}

// Compares a sequence with a version computed from scratch.
template <EdgeSequence Sequence>
void checkSequence(const Sequence& sequence, const Instance& instance, const Tour& tour, Edge reference) {
  Candidate best{std::numeric_limits<Weight>::max(), 0};
  for (const Edge& edge : expectedPath(tour, reference)) {
    CHECK(sequence.oriented(edge.id) == edge);
    best = std::min(best, Candidate{twoMoveDelta(instance, reference.from, reference.to, edge.from, edge.to), edge.id});
  }
  CHECK(sequence.best() == best);
}

}  // namespace twoopt::test
