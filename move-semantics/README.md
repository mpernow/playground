# Move Semantics

Copy and move semantics in C++, built around a class that owns a raw buffer.

`PixelBuffer` holds a heap-allocated pixel array and implements the rule
of five: copy constructor and assignment deep-copy the pixels, move
constructor and assignment steal the pointer. Constructors and the destructor
log the buffer address, and static `copy_count` / `move_count` counters track
how often each path is taken.

## Build

Requires CMake and a C++23 compiler (for `<print>`).

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/main
```

## Layout

- [PixelBuffer.hpp](PixelBuffer.hpp) / [PixelBuffer.cpp](PixelBuffer.cpp) —
  `PixelBuffer`, an owning buffer with copy and move operations
- [report.hpp](report.hpp) — `report()` resets the counters, runs one
  experiment and prints its copy/move counts; `QuietLogs` mutes buffer logging
- [demos.hpp](demos.hpp) — one function per demo
- [basics.cpp](basics.cpp) — copy vs move, and timing both
- [elision.cpp](elision.cpp) — guaranteed elision, NRVO, defeating NRVO and
  `return std::move`
- [forwarding.cpp](forwarding.cpp) — deduction and reference collapsing as
  `static_assert`s, then `archive` with and without `std::forward`
- [main.cpp](main.cpp) — runs every demo in order
