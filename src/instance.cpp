#include "twoopt/instance.hpp"

#include <ostream>
#include <stdexcept>

#include "io.hpp"

namespace twoopt {

Instance::Instance(int n) : n_(n), weights_(static_cast<std::size_t>(n) * static_cast<std::size_t>(n), 0) {
  if (n < 0) throw std::invalid_argument("negative instance size");
}

void Instance::set(Vertex u, Vertex v, Weight weight) {
  weights_[index(u, v)] = weight;
  weights_[index(v, u)] = weight;
}

Instance Instance::read(std::istream& in) {
  const std::vector<long long> values = detail::readIntegers(in);
  if (values.empty() || values[0] < 0) throw std::runtime_error("instance: missing size");
  const int n = static_cast<int>(values[0]);
  if (values.size() != 1 + static_cast<std::size_t>(n) * static_cast<std::size_t>(n)) {
    throw std::runtime_error("instance: expected an n×n matrix");
  }
  Instance instance(n);
  instance.weights_.assign(values.begin() + 1, values.end());
  for (Vertex u = 0; u < n; ++u) {
    if (instance(u, u) != 0) throw std::runtime_error("instance: diagonal must be 0");
    for (Vertex v = 0; v < u; ++v) {
      if (instance(u, v) != instance(v, u)) throw std::runtime_error("instance: matrix not symmetric");
    }
  }
  return instance;
}

void Instance::write(std::ostream& out) const {
  out << n_ << '\n';
  for (Vertex u = 0; u < n_; ++u) {
    for (Vertex v = 0; v < n_; ++v) out << (v ? " " : "") << (*this)(u, v);
    out << '\n';
  }
}

}  // namespace twoopt
