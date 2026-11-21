# Repeated 2-opt in O(n log n) per iteration

C++20 implementation of repeated **2-opt Optimization** from

> M. de Berg, K. Buchin, B. M. P. Jansen, G. Woeginger.
> *Fine-Grained Complexity Analysis of Two Classic TSP Variants.*
> ACM Transactions on Algorithms 17(1), 2021 (Theorem 9); arXiv:1607.02725 (Theorem 7).

After O(n²) preprocessing and with O(n²) storage, every iteration finds the best
2-move on a complete graph and applies it in O(n log n) time.
A naive Θ(n²)-per-iteration search is included as a reference.

## Build and test

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Usage

```bash
build/generate instance euclidean 1000 42 > instance.txt   # uniform|metric|euclidean|clustered|circle|grid
build/generate tour random 1000 42 > tour.txt               # identity|random
build/two_opt instance.txt --tour tour.txt --algorithm treap   # or naive; --print-tour
```

Instance file: `n`, then the symmetric n×n weight matrix. Tour file: `n`, then the vertices
in tour order. `#` starts a comment. Weights are integers, so all comparisons are exact.

## How it works

For every tour edge e = a→b there is one sequence T(e) over the path P(e) from b to a.
For an edge c→d on that path, the 2-move replacing a→b, c→d by a→c, b→d changes the
tour length by w(a,c) + w(b,d) − w(a,b) − w(c,d). T(e) keeps the minimum of this value,
so the best move overall is the minimum over the n sequences.

After a move, the two sequences of the removed edges are rebuilt in O(n). Every other
sequence is updated in O(log n): split off the part between the two removed edges,
reverse it lazily, insert the two new edges, and join.

## Code layout

![Files, interfaces and implementations](docs/architecture.svg)

| File | Content |
|---|---|
| `include/twoopt/edge_sequence.hpp` | `EdgeSequence` concept: the interface of one T(e) |
| `include/twoopt/treap_sequence.hpp` | implementation of T(e) as a treap with lazy reversal |
| `include/twoopt/repeated_two_opt.hpp` | the algorithm, templated on the sequence type |
| `include/twoopt/naive_two_opt.hpp` | Θ(n²) reference search |
| `include/twoopt/tour.hpp` | tour array with stable edge ids |
| `include/twoopt/instance.hpp` | weight matrix |
| `include/twoopt/generators.hpp` | random and structured instances, start tours |
| `tests/` | checks against the naive search and against rebuilt sequences |

### Adding another sequence implementation

Implement the four operations of `EdgeSequence` (`build`, `best`, `oriented`,
`applyTwoMove`; see `edge_sequence.hpp`), then add one line to `main()` in
`tests/test_treap_sequence.cpp` and `tests/test_repeated_two_opt.cpp`. The algorithm
itself does not change: `RepeatedTwoOpt<YourSequence>`.

## Differences from the paper

- The paper uses red-black trees (worst-case O(log n)). The treap gives expected
  O(log n) per operation.
- Each of the n² tree nodes takes about 80 bytes; n = 1500 needs about 200 MB.
