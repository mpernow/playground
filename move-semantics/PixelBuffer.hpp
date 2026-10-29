#pragma once

#include <cstddef>
#include <cstdint>

class PixelBuffer {
public:
  PixelBuffer(int width, int height);
  PixelBuffer(const PixelBuffer &other);
  PixelBuffer &operator=(const PixelBuffer &other);
  PixelBuffer(PixelBuffer &&other) noexcept;
  PixelBuffer &operator=(PixelBuffer &&other) noexcept;
  ~PixelBuffer();

  std::size_t byte_size() const;
  static void reset_counters();

  bool empty() const;

  static inline int copy_count = 0;
  static inline int move_count = 0;
  static inline bool verbose = true;

private:
  int width_ = 0;
  int height_ = 0;
  std::uint8_t *pixels_ = nullptr;
};
