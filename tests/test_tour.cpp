// Tour bookkeeping: ids, positions and 2-moves against a direct recomputation.
#include <algorithm>
#include <set>
#include <sstream>
#include <utility>

#include "check.hpp"
#include "twoopt/util/generators.hpp"
#include "twoopt/util/tour.hpp"
#include "util/random.hpp"

using namespace twoopt;

namespace {

std::set<std::pair<Vertex, Vertex>> undirectedEdges(const Tour& tour) {
  std::set<std::pair<Vertex, Vertex>> edges;
  for (int p = 0; p < tour.size(); ++p) {
    const Edge e = tour.edgeAt(p);
    edges.insert(std::minmax(e.from, e.to));
  }
  return edges;
}

void checkConsistent(const Tour& tour) {
  std::set<EdgeId> ids;
  for (int p = 0; p < tour.size(); ++p) {
    const Edge e = tour.edgeAt(p);
    CHECK(tour.position(e.id) == p);
    CHECK(tour.edge(e.id) == e);
    ids.insert(e.id);
  }
  CHECK(static_cast<int>(ids.size()) == tour.size());
}

}  // namespace

int main() {
  Random random(7);
  for (int n = 4; n <= 30; ++n) {
    const Instance instance = uniformRandomInstance(n, 100, n);
    Tour tour(randomOrder(n, n));
    for (int step = 0; step < 200; ++step) {
      const EdgeId x = static_cast<EdgeId>(random.below(n));
      const EdgeId y = static_cast<EdgeId>(random.below(n));
      if (x == y || tour.adjacent(x, y)) continue;

      auto expected = undirectedEdges(tour);
      const Edge ex = tour.edge(x);
      const Edge ey = tour.edge(y);
      expected.erase(std::minmax(ex.from, ex.to));
      expected.erase(std::minmax(ey.from, ey.to));
      const Weight before = tour.length(instance);
      const Weight delta = twoMoveDelta(instance, tour, x, y);

      tour.applyTwoMove(x, y);
      checkConsistent(tour);
      const Edge nx = tour.edge(x);
      const Edge ny = tour.edge(y);
      expected.insert(std::minmax(nx.from, nx.to));
      expected.insert(std::minmax(ny.from, ny.to));
      CHECK(undirectedEdges(tour) == expected);
      CHECK(tour.length(instance) == before + delta);
    }
  }

  // Round trip through the text format.
  const Tour tour(randomOrder(9, 3));
  std::stringstream text;
  tour.write(text);
  CHECK(Tour::read(text).order() == tour.order());
  const Instance instance = uniformRandomInstance(6, 50, 4);
  std::stringstream matrix;
  instance.write(matrix);
  const Instance copy = Instance::read(matrix);
  for (Vertex u = 0; u < 6; ++u)
    for (Vertex v = 0; v < 6; ++v) CHECK(copy(u, v) == instance(u, v));
  return 0;
}
