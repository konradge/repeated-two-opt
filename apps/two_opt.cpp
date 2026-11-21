// Runs repeated 2-opt Optimization until the tour is 2-optimal.
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "twoopt/generators.hpp"
#include "twoopt/naive_two_opt.hpp"
#include "twoopt/repeated_two_opt.hpp"
#include "twoopt/treap_sequence.hpp"

using namespace twoopt;

namespace {

constexpr const char* kUsage =
    "usage: two_opt <instance-file> [--tour <tour-file>] [--algorithm treap|naive] [--print-tour]\n"
    "       without --tour the identity tour 0, 1, ..., n-1 is used\n";

struct Options {
  std::string instanceFile;
  std::string tourFile;
  std::string algorithm = "treap";
  bool printTour = false;
};

Options parse(const std::vector<std::string>& args) {
  Options options;
  for (std::size_t i = 0; i < args.size(); ++i) {
    const auto value = [&] {
      if (++i == args.size()) throw std::invalid_argument("missing value for " + args[i - 1]);
      return args[i];
    };
    if (args[i] == "--tour") options.tourFile = value();
    else if (args[i] == "--algorithm") options.algorithm = value();
    else if (args[i] == "--print-tour") options.printTour = true;
    else if (options.instanceFile.empty()) options.instanceFile = args[i];
    else throw std::invalid_argument("unexpected argument: " + args[i]);
  }
  if (options.instanceFile.empty()) throw std::invalid_argument("missing instance file");
  return options;
}

template <class T>
T readFile(const std::string& path) {
  std::ifstream in(path);
  if (!in) throw std::runtime_error("cannot open " + path);
  return T::read(in);
}

template <class Algorithm>
void run(Algorithm& algorithm, const Instance& instance, const Options& options) {
  std::cout << "initial length " << algorithm.tour().length(instance) << '\n';
  const long long moves = runToLocalOptimum(algorithm);
  std::cout << "moves " << moves << '\n' << "final length " << algorithm.tour().length(instance) << '\n';
  if (options.printTour) algorithm.tour().write(std::cout);
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Options options = parse(std::vector<std::string>(argv + 1, argv + argc));
    const Instance instance = readFile<Instance>(options.instanceFile);
    Tour tour = options.tourFile.empty() ? Tour(identityOrder(instance.size())) : readFile<Tour>(options.tourFile);
    if (tour.size() != instance.size()) throw std::runtime_error("tour and instance differ in size");
    std::cout << "n " << instance.size() << '\n';

    if (options.algorithm == "treap") {
      RepeatedTwoOpt<TreapSequence> algorithm(instance, std::move(tour));
      run(algorithm, instance, options);
    } else if (options.algorithm == "naive") {
      NaiveTwoOpt algorithm(instance, std::move(tour));
      run(algorithm, instance, options);
    } else {
      throw std::invalid_argument("unknown algorithm: " + options.algorithm);
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << '\n' << kUsage;
    return 1;
  }
}
