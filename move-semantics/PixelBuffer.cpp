#include "PixelBuffer.hpp"

#include <cstring>
#include <print>
#include <utility>

PixelBuffer::PixelBuffer(int width, int height)
    : width_(width), height_(height), pixels_(new std::uint8_t[byte_size()]{}) {
  std::println("constructed {}x{} ({} bytes) at {}", width_, height_,
               byte_size(), static_cast<void *>(pixels_));
  std::fflush(stdout);
}

PixelBuffer::PixelBuffer(const PixelBuffer &other)
    : width_(other.width_), height_(other.height_),
      pixels_(new std::uint8_t[other.byte_size()]) {
  std::memcpy(pixels_, other.pixels_, other.byte_size());
  ++copy_count;
}

PixelBuffer &PixelBuffer::operator=(const PixelBuffer &other) {
  if (this == &other) {

    return *this;
  }

  auto *new_pixels = new std::uint8_t[other.byte_size()];
  std::memcpy(new_pixels, other.pixels_, other.byte_size());
  delete[] pixels_;
  pixels_ = new_pixels;
  width_ = other.width_;
  height_ = other.height_;
  ++copy_count;
  return *this;
}

PixelBuffer::PixelBuffer(PixelBuffer &&other) noexcept
    : width_(std::exchange(other.width_, 0)),
      height_(std::exchange(other.height_, 0)),
      pixels_(std::exchange(other.pixels_, nullptr)) {
  ++move_count;
}

PixelBuffer &PixelBuffer::operator=(PixelBuffer &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  delete[] pixels_;
  pixels_ = std::exchange(other.pixels_, nullptr);
  width_ = std::exchange(other.width_, 0);
  height_ = std::exchange(other.height_, 0);
  ++move_count;
  return *this;
}

PixelBuffer::~PixelBuffer() {
  std::println("destructing {}x{} at {}", width_, height_,
               static_cast<void *>(pixels_));
  std::fflush(stdout);
  delete[] pixels_;
}

std::size_t PixelBuffer::byte_size() const {
  return static_cast<std::size_t>(width_) * height_ * 4;
}

bool PixelBuffer::empty() const { return pixels_ == nullptr; }
