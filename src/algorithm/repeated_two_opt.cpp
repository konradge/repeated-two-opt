#include "algorithm/repeated_two_opt.hpp"

#include <utility>

#include "data_structure/treap_sequence.hpp"

namespace twoopt {

template <EdgeSequence Sequence>
RepeatedTwoOpt<Sequence>::RepeatedTwoOpt(const Instance& instance, Tour tour)
    : instance_(instance), tour_(std::move(tour)), sequences_(static_cast<std::size_t>(tour_.size())) {
  for (EdgeId e = 0; e < tour_.size(); ++e) rebuild(e);
}

// T(e) breaks ties by the smaller partner id, which for a fixed e is the same as
// TwoMove's tie-break, so the minimum over all T(e) is the overall best move.
template <EdgeSequence Sequence>
std::optional<TwoMove> RepeatedTwoOpt<Sequence>::bestMove() const {
  std::optional<TwoMove> best;
  for (EdgeId e = 0; e < tour_.size(); ++e) {
    const Candidate candidate = sequences_[e].best();
    if (candidate.delta >= 0) continue;
    const TwoMove move = makeTwoMove(e, candidate.edge, candidate.delta);
    if (!best || move < *best) best = move;
  }
  return best;
}

template <EdgeSequence Sequence>
void RepeatedTwoOpt<Sequence>::apply(const TwoMove& move) {
  tour_.applyTwoMove(move.first, move.second);
  const Edge newA = tour_.edge(move.first);
  const Edge newB = tour_.edge(move.second);
  for (EdgeId e = 0; e < tour_.size(); ++e) {
    if (e != move.first && e != move.second) sequences_[e].applyTwoMove(move.first, move.second, newA, newB);
  }
  rebuild(move.first);
  rebuild(move.second);
}

template <EdgeSequence Sequence>
void RepeatedTwoOpt<Sequence>::rebuild(EdgeId e) {
  tour_.pathAfter(e, path_);
  sequences_[e].build(instance_, tour_.edge(e), path_);
}

// One line per EdgeSequence implementation.
template class RepeatedTwoOpt<TreapSequence>;

}  // namespace twoopt
