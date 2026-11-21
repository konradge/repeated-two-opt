#pragma once

#include <compare>
#include <concepts>
#include <span>

#include "twoopt/instance.hpp"
#include "twoopt/types.hpp"

namespace twoopt {

// Best 2-move partner within one sequence T(e).
struct Candidate {
  Weight delta = 0;
  EdgeId edge = 0;

  friend auto operator<=>(const Candidate&, const Candidate&) = default;
};

// The structure T(e) of Theorem 7 (arXiv) / Theorem 9 (TALG) for one reference edge e = a→b.
// It holds the path P(e) from b to a; every edge of it has a current direction along that path.
// For an edge c→d of P(e), removing a→b and c→d and adding a→c, b→d is a valid 2-move
// with delta w(a,c) + w(b,d) - w(a,b) - w(c,d).
//
//   build(instance, e, path)   path = P(e) in order, directed from b to a.          O(n)
//   best()                     edge of P(e) with the smallest delta; among equal       O(1)
//                              deltas the one with the smallest id (= Candidate order).
//   oriented(f)                f directed along P(e).                                 O(log n)
//   applyTwoMove(x, y, u, v)   P(e) = X x Y y Z with x = p→q, y = r→s becomes
//                              X (p→r) reverse(Y) (q→s) Z. u and v are the new edges
//                              in either order and with any direction; their ids
//                              must be x and y.                                       O(log n)
//
// The instance passed to build() must outlive the sequence.
template <class S>
concept EdgeSequence =
    std::default_initializable<S> &&
    requires(S sequence, const S& constSequence, const Instance& instance, Edge edge,
             std::span<const Edge> path, EdgeId id) {
      sequence.build(instance, edge, path);
      { constSequence.best() } -> std::same_as<Candidate>;
      { constSequence.oriented(id) } -> std::same_as<Edge>;
      sequence.applyTwoMove(id, id, edge, edge);
    };

}  // namespace twoopt
