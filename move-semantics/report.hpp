#pragma once

#include <print>
#include <string_view>
#include <utility>

#include "PixelBuffer.hpp"

// Turns off PixelBuffer's constructor/destructor logging for one scope, so
// the counter output isn't buried under 8 MB buffers being created and freed.
class QuietLogs {
public:
  QuietLogs() : previous_(std::exchange(PixelBuffer::verbose, false)) {}
  ~QuietLogs() { PixelBuffer::verbose = previous_; }
  QuietLogs(const QuietLogs &) = delete;
  QuietLogs &operator=(const QuietLogs &) = delete;

private:
  bool previous_;
};

// Resets the counters, runs `action`, and prints how many copies and moves it
// caused. Every object `action` creates is destroyed before the counts are
// printed, so each line measures exactly one experiment.
template <typename F> void report(std::string_view label, F &&action) {
  PixelBuffer::reset_counters();
  std::forward<F>(action)();
  std::println("{:<28} copy_count={} move_count={}", label,
               PixelBuffer::copy_count, PixelBuffer::move_count);
}

inline void section(std::string_view title) {
  std::println("");
  std::println("--- {} ---", title);
}
