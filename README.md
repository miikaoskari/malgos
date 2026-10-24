![stack](/assets/stack.png) **malgos**

[![docs](https://img.shields.io/badge/docs-online-blue)](https://miikaoskari.github.io/malgos/index.html)

zero-allocation algorithm and data structure library built for freestanding environments. malgos provides containers with no hidden allocations.

api is still unstable and subject to change.

## available types

- containers
  - intrusive hashtable
  - intrusive doubly linked list
  - intrusive singly linked list
- allocators
  - arena

## footprint

compiled with `-Os` on Apple clang version 21.0.0 (clang-2100.1.1.101) for arm64-apple-darwin25.6.0

| module | text | total object size |
|---|---|---|
| hashtable | 480 B | 640 B |
| dlist | 252 B | 444 B |

## building

the project builds with either meson or cmake.

with meson:
```bash
meson setup builddir
meson compile -C builddir
```

## testing

this project uses [Unity](https://github.com/ThrowTheSwitch/Unity) by ThrowTheSwitch.

to run the unit tests with meson:
```bash
meson test -C builddir
```

## benchmarking

benchmarks use [google benchmark](https://github.com/google/benchmark) and are off by default:
```bash
meson setup builddir-bench -Dbenchmarks=enabled
meson compile -C builddir-bench
./builddir-bench/bench/bench_intrusive_hashtable
```
