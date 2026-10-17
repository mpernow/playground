#include <chrono>
#include <print>

#include "PixelBuffer.hpp"

int main() {
  {
    std::println("Scope 1: Checking copy constructor");
    PixelBuffer a(4, 4);
    PixelBuffer b = a;

    std::println("About to leave scope 1");
    std::fflush(stdout);
  }

  {
    std::println("");
    std::println("Scope 2: Checking move constructor");
    PixelBuffer frame(1920, 1080);
    PixelBuffer copy_of_frame = frame;          //  l-value -> copy constructor
    PixelBuffer moved_frame = std::move(frame); // xvalue -> move constructor

    std::println(
        "after copy+move: copy_count={}, move_count={}",
        PixelBuffer::copy_count,
        PixelBuffer::move_count); // This includes the values from the previous
                                  // scope, since static inlined
    std::println("frame.empty() after move: {}", frame.empty());
    std::println("About to leave scope 2"); // Destructs in reverse order: first
                                            // moved-to, then copied-to, finally
                                            // the moved-from already nullptr
  }

  {
    std::println("");
    std::println("Scope 3: Timing move and copy");
    PixelBuffer src(1920, 1080);
    src.verbose = false; // Remove logging to make timing accurate
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
    std::println("About to leave scope 3");
  }
}
