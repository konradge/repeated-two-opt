#pragma once

#include <concepts>
#include <optional>

#include "twoopt/util/tour.hpp"
#include "twoopt/util/two_move.hpp"

namespace twoopt {

// An algorithm for repeated 2-opt Optimization on a fixed instance.
//
//   bestMove()    best improving move in the order of TwoMove, or nothing if 2-optimal
//   apply(move)   applies a move returned by bestMove()
//   tour()        current tour
//
// All implementations pick the same move, so they produce the same sequence of tours.
template <class A>
concept TwoOptAlgorithm = requires(A algorithm, const A& constAlgorithm, const TwoMove& move) {
  { constAlgorithm.bestMove() } -> std::same_as<std::optional<TwoMove>>;
  algorithm.apply(move);
  { constAlgorithm.tour() } -> std::same_as<const Tour&>;
};

// Applies best moves until the tour is 2-optimal; returns the number of moves.
template <TwoOptAlgorithm Algorithm>
long long runToLocalOptimum(Algorithm& algorithm) {
  long long moves = 0;
  while (const auto move = algorithm.bestMove()) {
    algorithm.apply(*move);
    ++moves;
  }
  return moves;
}

}  // namespace twoopt
