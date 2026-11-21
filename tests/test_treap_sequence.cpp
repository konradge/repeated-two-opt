// One sequence T(e) under random 2-moves, compared with a rebuild from the tour after every move.
// testSequence() works for every EdgeSequence, so new backends only need one more call in main().
#include <vector>

#include "data_structure/treap_sequence.hpp"
#include "sequence_model.hpp"
#include "twoopt/util/generators.hpp"
#include "util/random.hpp"

using namespace twoopt;

namespace {

template <EdgeSequence Sequence>
void testSequence() {
  Random random(11);
  for (int n = 3; n <= 40; ++n) {
    for (int trial = 0; trial < 3; ++trial) {
      const Instance instance = uniformRandomInstance(n, 1000, 100 * n + trial);
      Tour tour(randomOrder(n, 100 * n + trial));
      const Edge reference = tour.edge(static_cast<EdgeId>(random.below(n)));

      std::vector<Edge> path;
      tour.pathAfter(reference.id, path);
      Sequence sequence;
      sequence.build(instance, reference, path);
      test::checkSequence(sequence, instance, tour, reference);

      for (int step = 0; step < 100; ++step) {
        const EdgeId x = static_cast<EdgeId>(random.below(n));
        const EdgeId y = static_cast<EdgeId>(random.below(n));
        if (x == y || x == reference.id || y == reference.id || tour.adjacent(x, y)) continue;
        tour.applyTwoMove(x, y);
        // Pass the new edges in random order and direction.
        Edge a = tour.edge(x);
        Edge b = tour.edge(y);
        if (random.below(2)) std::swap(a.from, a.to);
        if (random.below(2)) std::swap(a, b);
        sequence.applyTwoMove(x, y, a, b);
        test::checkSequence(sequence, instance, tour, reference);
      }
    }
  }
}

}  // namespace

int main() {
  testSequence<TreapSequence>();
  return 0;
}
