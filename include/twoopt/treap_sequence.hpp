#pragma once

#include <cstdint>
#include <span>
#include <utility>
#include <vector>

#include "twoopt/edge_sequence.hpp"
#include "twoopt/instance.hpp"
#include "twoopt/random.hpp"
#include "twoopt/types.hpp"

namespace twoopt {

// T(e) as an implicit treap: in-order = order along P(e). Every subtree stores the
// minimum delta for its current orientation and for the reversed one, so a reversal
// is a lazy flag that swaps the two. Expected O(log n) per update (the paper's
// red-black tree gives worst-case O(log n)).
// Node i holds the tour edge with id i, so no lookup table is needed.
class TreapSequence {
 public:
  void build(const Instance& instance, Edge reference, std::span<const Edge> path);
  Candidate best() const;
  Edge oriented(EdgeId id) const;
  void applyTwoMove(EdgeId x, EdgeId y, Edge newA, Edge newB);

  Edge reference() const { return reference_; }

 private:
  using Index = std::int32_t;
  static constexpr Index kNone = -1;

  struct Node {
    Index left = kNone;
    Index right = kNone;
    Index parent = kNone;
    std::int32_t size = 1;
    std::uint32_t priority = 0;
    bool reversed = false;  // subtree still to be reversed; best/bestReversed already swapped
    Vertex from = 0;
    Vertex to = 0;
    Weight delta = 0;          // this edge as from→to
    Weight deltaReversed = 0;  // this edge as to→from
    Candidate best;            // subtree minimum
    Candidate bestReversed;    // subtree minimum after reversing the subtree
  };

  void makeLeaf(Index v, Vertex from, Vertex to);
  void flip(Index v);
  void push(Index v);
  void pull(Index v);
  void pullSubtree(Index v);
  void setLeft(Index v, Index child);
  void setRight(Index v, Index child);
  std::int32_t sizeOf(Index v) const { return v == kNone ? 0 : nodes_[v].size; }
  std::int32_t positionOf(Index v);
  std::pair<Index, Index> split(Index v, std::int32_t count);
  Index merge(Index left, Index right);

  const Instance* instance_ = nullptr;
  Edge reference_;
  Index root_ = kNone;
  std::vector<Node> nodes_;
  std::vector<Index> scratch_;
  Random random_{0};
};

static_assert(EdgeSequence<TreapSequence>);

}  // namespace twoopt
