// Factory pattern on its own: build sensors from a string key without naming
// any concrete sensor type, and see what happens for an unknown key.

#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "factory.hpp"

int main() {
  std::cout << std::fixed << std::setprecision(1);

  std::vector<std::unique_ptr<Sensor>> sensors;
  for (const auto *kind : {"temperature", "humidity", "co2", "pressure"}) {
    sensors.push_back(SensorFactory::create(kind));
  }

  for (int poll_num = 1; poll_num <= 3; ++poll_num) {
    std::cout << "\nPOLLING " << poll_num << '\n';
    for (auto &sensor : sensors) {
      Reading r = sensor->poll();
      std::cout << "  " << r.sensor_name << ": " << r.value << " " << r.unit
                << '\n';
    }
  }

  try {
    SensorFactory::create("bogus");
  } catch (const std::invalid_argument &e) {
    std::cout << "\ncreate(\"bogus\") threw: " << e.what() << '\n';
  }
}
