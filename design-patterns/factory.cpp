#include "factory.hpp"

#include <stdexcept>
#include <utility>

void SensorFactory::register_kind(std::string kind, Creator creator) {
  registry()[std::move(kind)] = std::move(creator);
}

std::unique_ptr<Sensor> SensorFactory::create(const std::string &kind) {
  auto &reg = registry();
  auto it = reg.find(kind);
  if (it == reg.end()) {
    throw std::invalid_argument("unknown sensor kind: " + kind);
  }
  return it->second();
}

std::unordered_map<std::string, SensorFactory::Creator> &
SensorFactory::registry() {
  static std::unordered_map<std::string, Creator> instance;
  return instance;
}

// Concrete sensors and their registrations live entirely in this translation
// unit; nothing outside needs the types.
namespace {

class TemperatureSensor : public Sensor {
public:
  Reading poll() override {
    double value = 20.0 + tick_ * 0.5;
    ++tick_;
    return {"temperature", value, "C"};
  }

private:
  int tick_ = 0;
};

class HumiditySensor : public Sensor {
public:
  Reading poll() override {
    double value = 45.0 - tick_ * 1.0;
    ++tick_;
    return {"humidity", value, "%RH"};
  }

private:
  int tick_ = 0;
};

class Co2Sensor : public Sensor {
public:
  Reading poll() override {
    double value = 800.0 + tick_ * 50.0;
    ++tick_;
    return {"co2", value, "ppm"};
  }

private:
  int tick_ = 0;
};

class PressureSensor : public Sensor {
public:
  Reading poll() override {
    double value = 900.0 + tick_ * 1.0;
    ++tick_;
    return {"pressure", value, "hPa"};
  }

private:
  int tick_ = 0;
};

// These objects exist only so their constructors run at static-init time and
// populate the factory registry.
//
// This works because factory.cpp is linked straight into each executable. If
// it were bundled into a static library instead, the linker could drop this
// whole translation unit (nothing references it directly) and the registry
// would come up empty. The usual fixes are whole-archive / a CMake OBJECT
// library, or exposing an explicit register_builtin_sensors() function to call.
const SensorRegistrar<TemperatureSensor> temperature_reg{"temperature"};
const SensorRegistrar<HumiditySensor> humidity_reg{"humidity"};
const SensorRegistrar<Co2Sensor> co2_reg{"co2"};
const SensorRegistrar<PressureSensor> pressure_reg{"pressure"};

} // namespace
