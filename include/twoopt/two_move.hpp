#pragma once

#include "twoopt/instance.hpp"
#include "twoopt/types.hpp"

namespace twoopt {

// Remove the tour edges `first` and `second` and reconnect the two resulting paths.
struct TwoMove {
  EdgeId first = 0;
  EdgeId second = 0;
  Weight delta = 0;  // change of the tour length, negative if improving

  friend bool operator==(const TwoMove&, const TwoMove&) = default;
};

// Length change when a→b and c→d, in this order along the tour, are replaced by a→c and b→d.
inline Weight twoMoveDelta(const Instance& w, Vertex a, Vertex b, Vertex c, Vertex d) {
  return w(a, c) + w(b, d) - w(a, b) - w(c, d);
}

}  // namespace twoopt
