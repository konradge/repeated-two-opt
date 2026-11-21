#include "twoopt/treap_sequence.hpp"

#include <algorithm>
#include <cassert>

#include "twoopt/two_move.hpp"

namespace twoopt {

namespace {

bool connects(const Edge& edge, Vertex u, Vertex v) {
  return (edge.from == u && edge.to == v) || (edge.from == v && edge.to == u);
}

}  // namespace

void TreapSequence::build(const Instance& instance, Edge reference, std::span<const Edge> path) {
  instance_ = &instance;
  reference_ = reference;
  random_ = Random(static_cast<std::uint64_t>(reference.id) + 1);
  nodes_.assign(static_cast<std::size_t>(instance.size()), Node{});

  // Cartesian tree in O(n); scratch_ holds the right spine.
  scratch_.clear();
  for (const Edge& edge : path) {
    makeLeaf(edge.id, edge.from, edge.to);
    Index popped = kNone;
    while (!scratch_.empty() && nodes_[scratch_.back()].priority < nodes_[edge.id].priority) {
      popped = scratch_.back();
      scratch_.pop_back();
    }
    setLeft(edge.id, popped);
    if (!scratch_.empty()) setRight(scratch_.back(), edge.id);
    scratch_.push_back(edge.id);
  }
  root_ = scratch_.empty() ? kNone : scratch_.front();
  if (root_ != kNone) {
    nodes_[root_].parent = kNone;
    pullSubtree(root_);
  }
}

Candidate TreapSequence::best() const {
  assert(root_ != kNone);
  return nodes_[root_].best;
}

Edge TreapSequence::oriented(EdgeId id) const {
  bool reversed = false;
  for (Index v = id; v != kNone; v = nodes_[v].parent) reversed ^= nodes_[v].reversed;
  const Node& node = nodes_[id];
  return reversed ? Edge{id, node.to, node.from} : Edge{id, node.from, node.to};
}

void TreapSequence::applyTwoMove(EdgeId x, EdgeId y, Edge newA, Edge newB) {
  std::int32_t px = positionOf(x);
  std::int32_t py = positionOf(y);
  if (px > py) {
    std::swap(x, y);
    std::swap(px, py);
  }
  const Edge first = oriented(x);    // p→q
  const Edge second = oriented(y);   // r→s

  auto [before, rest] = split(root_, px);
  auto [firstNode, rest2] = split(rest, 1);
  auto [middle, rest3] = split(rest2, py - px - 1);
  auto [secondNode, after] = split(rest3, 1);
  assert(firstNode == x && secondNode == y);

  flip(middle);
  if (!connects(newA, first.from, second.from)) std::swap(newA, newB);
  assert(connects(newA, first.from, second.from) && connects(newB, first.to, second.to));
  makeLeaf(newA.id, first.from, second.from);
  makeLeaf(newB.id, first.to, second.to);

  root_ = merge(merge(merge(merge(before, newA.id), middle), newB.id), after);
  nodes_[root_].parent = kNone;
}

void TreapSequence::makeLeaf(Index v, Vertex from, Vertex to) {
  const Instance& w = *instance_;
  const Vertex a = reference_.from;
  const Vertex b = reference_.to;
  Node& node = nodes_[v];
  node = Node{};
  node.priority = static_cast<std::uint32_t>(random_.next());
  node.from = from;
  node.to = to;
  node.delta = twoMoveDelta(w, a, b, from, to);
  node.deltaReversed = twoMoveDelta(w, a, b, to, from);
  node.best = {node.delta, v};
  node.bestReversed = {node.deltaReversed, v};
}

void TreapSequence::flip(Index v) {
  if (v == kNone) return;
  Node& node = nodes_[v];
  std::swap(node.best, node.bestReversed);
  node.reversed = !node.reversed;
}

void TreapSequence::push(Index v) {
  Node& node = nodes_[v];
  if (!node.reversed) return;
  std::swap(node.left, node.right);
  std::swap(node.from, node.to);
  std::swap(node.delta, node.deltaReversed);
  flip(node.left);
  flip(node.right);
  node.reversed = false;
}

void TreapSequence::pull(Index v) {
  Node& node = nodes_[v];
  node.size = 1;
  node.best = {node.delta, v};
  node.bestReversed = {node.deltaReversed, v};
  for (const Index child : {node.left, node.right}) {
    if (child == kNone) continue;
    const Node& c = nodes_[child];
    node.size += c.size;
    node.best = std::min(node.best, c.best);
    node.bestReversed = std::min(node.bestReversed, c.bestReversed);
  }
}

void TreapSequence::pullSubtree(Index v) {
  if (v == kNone) return;
  pullSubtree(nodes_[v].left);
  pullSubtree(nodes_[v].right);
  pull(v);
}

void TreapSequence::setLeft(Index v, Index child) {
  nodes_[v].left = child;
  if (child != kNone) nodes_[child].parent = v;
}

void TreapSequence::setRight(Index v, Index child) {
  nodes_[v].right = child;
  if (child != kNone) nodes_[child].parent = v;
}

std::int32_t TreapSequence::positionOf(Index v) {
  // Push pending reversals top-down so that left/right on the path are final.
  scratch_.clear();
  for (Index u = v; u != kNone; u = nodes_[u].parent) scratch_.push_back(u);
  for (auto it = scratch_.rbegin(); it != scratch_.rend(); ++it) push(*it);

  std::int32_t position = sizeOf(nodes_[v].left);
  for (Index child = v, u = nodes_[v].parent; u != kNone; child = u, u = nodes_[u].parent) {
    if (nodes_[u].right == child) position += sizeOf(nodes_[u].left) + 1;
  }
  return position;
}

// Splits off the first `count` edges. The returned roots may have stale parents.
std::pair<TreapSequence::Index, TreapSequence::Index> TreapSequence::split(Index v, std::int32_t count) {
  if (v == kNone) return {kNone, kNone};
  push(v);
  if (sizeOf(nodes_[v].left) >= count) {
    const auto [left, right] = split(nodes_[v].left, count);
    setLeft(v, right);
    pull(v);
    return {left, v};
  }
  const auto [left, right] = split(nodes_[v].right, count - sizeOf(nodes_[v].left) - 1);
  setRight(v, left);
  pull(v);
  return {v, right};
}

TreapSequence::Index TreapSequence::merge(Index left, Index right) {
  if (left == kNone) return right;
  if (right == kNone) return left;
  if (nodes_[left].priority > nodes_[right].priority) {
    push(left);
    setRight(left, merge(nodes_[left].right, right));
    pull(left);
    return left;
  }
  push(right);
  setLeft(right, merge(left, nodes_[right].left));
  pull(right);
  return right;
}

}  // namespace twoopt
