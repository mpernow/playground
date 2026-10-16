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
- [main.cpp](main.cpp) — constructs a buffer and copies it
