#include "twoopt/util/tour.hpp"

#include <algorithm>
#include <cstdlib>
#include <ostream>
#include <stdexcept>

#include "twoopt/util/two_move.hpp"
#include "util/io.hpp"

namespace twoopt {

Tour::Tour(std::vector<Vertex> order) : order_(std::move(order)) {
  const int n = size();
  if (n < 3) throw std::invalid_argument("tour: need at least 3 vertices");
  std::vector<bool> seen(n, false);
  for (Vertex v : order_) {
    if (v < 0 || v >= n || seen[v]) throw std::invalid_argument("tour: not a permutation");
    seen[v] = true;
  }
  edgeAt_.resize(n);
  position_.resize(n);
  for (int p = 0; p < n; ++p) edgeAt_[p] = position_[p] = p;
}

Edge Tour::edgeAt(int position) const {
  const int next = position + 1 == size() ? 0 : position + 1;
  return {edgeAt_[position], order_[position], order_[next]};
}

Edge Tour::edge(EdgeId id) const { return edgeAt(position_[id]); }

bool Tour::adjacent(EdgeId x, EdgeId y) const {
  const int distance = std::abs(position_[x] - position_[y]);
  return distance == 1 || distance == size() - 1;
}

Weight Tour::length(const Instance& instance) const {
  Weight total = 0;
  for (int p = 0; p < size(); ++p) {
    const Edge e = edgeAt(p);
    total += instance(e.from, e.to);
  }
  return total;
}

void Tour::pathAfter(EdgeId id, std::vector<Edge>& path) const {
  path.clear();
  const int n = size();
  int p = position_[id];
  for (int k = 1; k < n; ++k) {
    p = p + 1 == n ? 0 : p + 1;
    path.push_back(edgeAt(p));
  }
}

void Tour::applyTwoMove(EdgeId x, EdgeId y) {
  int p = position_[x];
  int q = position_[y];
  if (p > q) std::swap(p, q);
  // New edges: order[p]→order[q] keeps the id at p, order[p+1]→order[q+1] the id at q.
  std::reverse(order_.begin() + p + 1, order_.begin() + q + 1);
  std::reverse(edgeAt_.begin() + p + 1, edgeAt_.begin() + q);
  for (int k = p + 1; k < q; ++k) position_[edgeAt_[k]] = k;
}

Tour Tour::read(std::istream& in) {
  const std::vector<long long> values = detail::readIntegers(in);
  if (values.empty() || values[0] < 0 || values.size() != static_cast<std::size_t>(values[0]) + 1) {
    throw std::runtime_error("tour: expected n followed by n vertices");
  }
  return Tour(std::vector<Vertex>(values.begin() + 1, values.end()));
}

void Tour::write(std::ostream& out) const {
  out << size() << '\n';
  for (int p = 0; p < size(); ++p) out << (p ? " " : "") << order_[p];
  out << '\n';
}

Weight twoMoveDelta(const Instance& instance, const Tour& tour, EdgeId x, EdgeId y) {
  Edge first = tour.edge(x);
  Edge second = tour.edge(y);
  if (tour.position(x) > tour.position(y)) std::swap(first, second);
  return twoMoveDelta(instance, first.from, first.to, second.from, second.to);
}

}  // namespace twoopt
