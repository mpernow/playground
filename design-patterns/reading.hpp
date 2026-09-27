#pragma once

#include <string>

namespace design_patterns {

// The value type all three patterns exchange. Small and copyable on purpose.
struct Reading {
  std::string sensor_name;
  double value;
  std::string unit;
};

} // namespace design_patterns
