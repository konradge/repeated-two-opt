#pragma once

#include <optional>
#include <vector>

#include "twoopt/algorithm/two_opt_algorithm.hpp"
#include "twoopt/data_structure/edge_sequence.hpp"
#include "twoopt/util/instance.hpp"
#include "twoopt/util/tour.hpp"
#include "twoopt/util/two_move.hpp"

namespace twoopt {

// Repeated 2-opt Optimization after de Berg, Buchin, Jansen, Woeginger
// (Theorem 7 on arXiv, Theorem 9 in ACM TALG): one sequence T(e) per tour edge e.
// O(n²) preprocessing and storage, O(n) + n updates of T(e) per iteration.
//
// Defined in src/algorithm/repeated_two_opt.cpp, which instantiates it for every
// EdgeSequence implementation; a new implementation must be added there.
template <EdgeSequence Sequence>
class RepeatedTwoOpt {
 public:
  RepeatedTwoOpt(const Instance& instance, Tour tour);  // instance must outlive this object. O(n²)

  // Best improving move (in the order of TwoMove), or nothing if the tour is 2-optimal. O(n).
  std::optional<TwoMove> bestMove() const;
  // O(n) for the tour and the two new sequences, plus n-2 sequence updates.
  void apply(const TwoMove& move);

  const Tour& tour() const { return tour_; }
  const Sequence& sequence(EdgeId e) const { return sequences_[e]; }

 private:
  void rebuild(EdgeId e);

  const Instance& instance_;
  Tour tour_;
  std::vector<Sequence> sequences_;
  std::vector<Edge> path_;
};

}  // namespace twoopt
