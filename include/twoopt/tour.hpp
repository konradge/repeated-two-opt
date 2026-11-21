#pragma once

#include <iosfwd>
#include <vector>

#include "twoopt/instance.hpp"
#include "twoopt/types.hpp"

namespace twoopt {

// Hamiltonian cycle as a vertex array. Edge p goes from position p to p+1 (mod n).
// Edge ids start equal to positions; a 2-move gives the two new edges the ids of
// the two removed ones, so every id in 0..n-1 always names exactly one tour edge.
class Tour {
 public:
  explicit Tour(std::vector<Vertex> order);  // order must be a permutation of 0..n-1, n >= 3

  int size() const { return static_cast<int>(order_.size()); }
  const std::vector<Vertex>& order() const { return order_; }

  Edge edgeAt(int position) const;  // directed along the tour
  Edge edge(EdgeId id) const;       // directed along the tour
  int position(EdgeId id) const { return position_[id]; }
  bool adjacent(EdgeId x, EdgeId y) const;  // share a vertex

  Weight length(const Instance& instance) const;

  // P(id): the other n-1 edges in tour direction, starting right after `id`.
  void pathAfter(EdgeId id, std::vector<Edge>& path) const;

  // Removes edges x and y (not adjacent) and reverses the part between them. O(n).
  void applyTwoMove(EdgeId x, EdgeId y);

  // Text format: n, then the n vertices in tour order. '#' starts a comment.
  static Tour read(std::istream& in);
  void write(std::ostream& out) const;

 private:
  std::vector<Vertex> order_;
  std::vector<EdgeId> edgeAt_;  // id of the edge starting at each position
  std::vector<int> position_;   // inverse of edgeAt_
};

// Length change of the 2-move removing the tour edges x and y.
Weight twoMoveDelta(const Instance& instance, const Tour& tour, EdgeId x, EdgeId y);

}  // namespace twoopt
