#pragma once

#include <cstdint>

namespace twoopt {

using Vertex = std::int32_t;
using EdgeId = std::int32_t;
using Weight = std::int64_t;

// A tour edge with an id that stays fixed while the edge exists.
// Whether from/to carry a direction is stated by the function returning it.
struct Edge {
  EdgeId id = 0;
  Vertex from = 0;
  Vertex to = 0;

  friend bool operator==(const Edge&, const Edge&) = default;
};

}  // namespace twoopt
