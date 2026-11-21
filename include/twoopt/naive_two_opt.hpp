#pragma once

#include <optional>

#include "twoopt/instance.hpp"
#include "twoopt/tour.hpp"
#include "twoopt/two_move.hpp"

namespace twoopt {

// Repeated 2-opt Optimization by checking all Θ(n²) edge pairs in every iteration.
class NaiveTwoOpt {
 public:
  NaiveTwoOpt(const Instance& instance, Tour tour);  // instance must outlive this object

  // Best improving move, or nothing if the tour is 2-optimal. O(n²).
  std::optional<TwoMove> bestMove() const;
  void apply(const TwoMove& move);  // O(n)

  const Tour& tour() const { return tour_; }

 private:
  const Instance& instance_;
  Tour tour_;
};

}  // namespace twoopt
