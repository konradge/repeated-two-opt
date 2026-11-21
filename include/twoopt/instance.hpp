#pragma once

#include <cstddef>
#include <iosfwd>
#include <vector>

#include "twoopt/types.hpp"

namespace twoopt {

// Complete graph with symmetric edge weights, stored as a full n×n matrix.
class Instance {
 public:
  explicit Instance(int n);

  int size() const { return n_; }
  Weight operator()(Vertex u, Vertex v) const { return weights_[index(u, v)]; }
  void set(Vertex u, Vertex v, Weight weight);  // sets both directions

  // Text format: n, then the n×n matrix row by row. '#' starts a comment.
  static Instance read(std::istream& in);
  void write(std::ostream& out) const;

 private:
  std::size_t index(Vertex u, Vertex v) const {
    return static_cast<std::size_t>(u) * static_cast<std::size_t>(n_) + static_cast<std::size_t>(v);
  }

  int n_;
  std::vector<Weight> weights_;
};

}  // namespace twoopt
