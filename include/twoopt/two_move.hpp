#pragma once

#include <algorithm>
#include <tuple>

#include "twoopt/instance.hpp"
#include "twoopt/types.hpp"

namespace twoopt {

// Remove the tour edges `first` and `second` and reconnect the two resulting paths.
// Build it with makeTwoMove() so that first < second.
struct TwoMove {
  EdgeId first = 0;
  EdgeId second = 0;
  Weight delta = 0;  // change of the tour length, negative if improving

  friend bool operator==(const TwoMove&, const TwoMove&) = default;

  // The order every algorithm uses to pick the best move: smaller delta first,
  // ties broken by the smaller and then the larger edge id. This makes the
  // sequence of moves, and hence the final tour, independent of the algorithm.
  friend bool operator<(const TwoMove& a, const TwoMove& b) {
    return std::tie(a.delta, a.first, a.second) < std::tie(b.delta, b.first, b.second);
  }
};

inline TwoMove makeTwoMove(EdgeId x, EdgeId y, Weight delta) {
  return {std::min(x, y), std::max(x, y), delta};
}

// Length change when a→b and c→d, in this order along the tour, are replaced by a→c and b→d.
inline Weight twoMoveDelta(const Instance& w, Vertex a, Vertex b, Vertex c, Vertex d) {
  return w(a, c) + w(b, d) - w(a, b) - w(c, d);
}

}  // namespace twoopt
