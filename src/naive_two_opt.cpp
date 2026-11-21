#include "twoopt/naive_two_opt.hpp"

#include <utility>

namespace twoopt {

NaiveTwoOpt::NaiveTwoOpt(const Instance& instance, Tour tour) : instance_(instance), tour_(std::move(tour)) {}

std::optional<TwoMove> NaiveTwoOpt::bestMove() const {
  const int n = tour_.size();
  std::optional<TwoMove> best;
  Weight bestDelta = 0;
  for (int i = 0; i + 2 < n; ++i) {
    const Edge first = tour_.edgeAt(i);
    const int last = i == 0 ? n - 2 : n - 1;  // edge n-1 touches edge 0
    for (int j = i + 2; j <= last; ++j) {
      const Edge second = tour_.edgeAt(j);
      const Weight delta = twoMoveDelta(instance_, first.from, first.to, second.from, second.to);
      if (delta < bestDelta) {
        bestDelta = delta;
        best = TwoMove{first.id, second.id, delta};
      }
    }
  }
  return best;
}

void NaiveTwoOpt::apply(const TwoMove& move) { tour_.applyTwoMove(move.first, move.second); }

}  // namespace twoopt
