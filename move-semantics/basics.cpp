#include <chrono>
#include <print>
#include <utility>

#include "PixelBuffer.hpp"
#include "demos.hpp"
#include "report.hpp"

void demo_copy_and_move() {
  section("copy vs move");
  PixelBuffer::reset_counters();
  {
    PixelBuffer frame(1920, 1080);
    PixelBuffer copy_of_frame = frame;          // lvalue -> copy constructor
    PixelBuffer moved_frame = std::move(frame); // xvalue -> move constructor

    std::println("after copy+move: copy_count={}, move_count={}",
                 PixelBuffer::copy_count, PixelBuffer::move_count);
    std::println("frame.empty() after move: {}", frame.empty());
    std::println("leaving scope");
    // Destructs in reverse order: first moved-to, then copied-to, finally the
    // moved-from frame which is already nullptr
  }
}

void demo_timing() {
  section("timing copy vs move");
  QuietLogs quiet; // logging would dominate the timings
  PixelBuffer::reset_counters();

  PixelBuffer src(1920, 1080);
  auto t0 = std::chrono::steady_clock::now();
  for (int i = 0; i < 200; ++i) {
    PixelBuffer tmp = src;
  }
  auto t1 = std::chrono::steady_clock::now();

  PixelBuffer src2(1920, 1080);
  auto t2 = std::chrono::steady_clock::now();
  for (int i = 0; i < 200; ++i) {
    PixelBuffer tmp = std::move(src2);
    src2 = std::move(tmp);
  }
  auto t3 = std::chrono::steady_clock::now();

  using ms = std::chrono::duration<double, std::milli>;
  using us = std::chrono::duration<double, std::micro>;
  std::println("200 copies: {:8.2f} ms total, {:8.2f} us each",
               ms(t1 - t0).count(), us(t1 - t0).count() / 200);
  std::println("400 moves:  {:8.2f} ms total, {:8.2f} us each",
               ms(t3 - t2).count(), us(t3 - t2).count() / 400);
  std::println("copy_count={} move_count={}", PixelBuffer::copy_count,
               PixelBuffer::move_count);
}
