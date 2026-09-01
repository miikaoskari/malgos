![stack](/assets/stack.png) **malgos**

[![docs](https://img.shields.io/badge/docs-online-blue)](https://miikaoskari.github.io/malgos/index.html)

zero-allocation algorithm and data structure library built for freestanding environments. malgos provides containers with no hidden allocations.

api is still unstable and subject to change.

## available types

- containers
  - intrusive hashtable
  - intrusive doubly linked list
  - intrusive singly linked list

## footprint

compiled with `-Os` on Apple clang version 21.0.0 (clang-2100.1.1.101) for arm64-apple-darwin25.6.0

| module | text | total object size |
|---|---|---|
| hashtable | 424 B | 552 B |
| dlist | 252 B | 444 B |

## building

to build the project:
```bash
cmake -B build -S .
cmake --build build
```

## testing

this project uses [Unity](https://github.com/ThrowTheSwitch/Unity) by ThrowTheSwitch.

to run the unit tests first build the project and then run:
```bash
cd build
ctest
```
