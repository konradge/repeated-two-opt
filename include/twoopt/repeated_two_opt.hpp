#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "twoopt/edge_sequence.hpp"
#include "twoopt/instance.hpp"
#include "twoopt/tour.hpp"
#include "twoopt/two_move.hpp"

namespace twoopt {

// Repeated 2-opt Optimization after de Berg, Buchin, Jansen, Woeginger
// (Theorem 7 on arXiv, Theorem 9 in ACM TALG): one sequence T(e) per tour edge e.
// O(n²) preprocessing and storage, O(n) + n updates of T(e) per iteration.
template <EdgeSequence Sequence>
class RepeatedTwoOpt {
 public:
  // The instance must outlive this object. O(n²).
  RepeatedTwoOpt(const Instance& instance, Tour tour)
      : instance_(instance), tour_(std::move(tour)), sequences_(static_cast<std::size_t>(tour_.size())) {
    for (EdgeId e = 0; e < tour_.size(); ++e) rebuild(e);
  }

  // Best improving move, or nothing if the tour is 2-optimal. O(n).
  std::optional<TwoMove> bestMove() const {
    std::optional<TwoMove> best;
    for (EdgeId e = 0; e < tour_.size(); ++e) {
      const Candidate candidate = sequences_[e].best();
      if (candidate.delta < (best ? best->delta : 0)) best = TwoMove{e, candidate.edge, candidate.delta};
    }
    return best;
  }

  // O(n) for the tour and the two new sequences, plus n-2 sequence updates.
  void apply(const TwoMove& move) {
    tour_.applyTwoMove(move.first, move.second);
    const Edge newA = tour_.edge(move.first);
    const Edge newB = tour_.edge(move.second);
    for (EdgeId e = 0; e < tour_.size(); ++e) {
      if (e != move.first && e != move.second) sequences_[e].applyTwoMove(move.first, move.second, newA, newB);
    }
    rebuild(move.first);
    rebuild(move.second);
  }

  const Tour& tour() const { return tour_; }
  const Sequence& sequence(EdgeId e) const { return sequences_[e]; }

 private:
  void rebuild(EdgeId e) {
    tour_.pathAfter(e, path_);
    sequences_[e].build(instance_, tour_.edge(e), path_);
  }

  const Instance& instance_;
  Tour tour_;
  std::vector<Sequence> sequences_;
  std::vector<Edge> path_;
};

// Applies best moves until the tour is 2-optimal; returns the number of moves.
template <class Algorithm>
long long runToLocalOptimum(Algorithm& algorithm) {
  long long moves = 0;
  while (const auto move = algorithm.bestMove()) {
    algorithm.apply(*move);
    ++moves;
  }
  return moves;
}

}  // namespace twoopt
