#include <print>

#include "PixelBuffer.hpp"

int main() {
  PixelBuffer a(4, 4);
  PixelBuffer b = a;

  std::println("about to leave main");
  std::fflush(stdout);
}
