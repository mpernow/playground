#pragma once

#include <string>

// The value type both patterns exchange. Small and copyable on purpose.
struct Reading {
  std::string sensor_name;
  double value;
  std::string unit;
};
