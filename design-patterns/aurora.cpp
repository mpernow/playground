#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct Reading {
  std::string sensor_name;
  double value;
  std::string unit;
};

class Sensor {
public:
  virtual ~Sensor() = default;
  virtual Reading poll() = 0;
};

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

class SensorFactory {
public:
  using Creator = std::function<std::unique_ptr<Sensor>()>;

  static void register_kind(std::string kind, Creator creator) {
    registry()[std::move(kind)] = std::move(creator);
  }

  static std::unique_ptr<Sensor> create(const std::string &kind) {
    auto &reg = registry();
    auto it = reg.find(kind);
    if (it == reg.end()) {
      throw std::invalid_argument("unknown sensor kind: " + kind);
    }
    return it->second();
  }

private:
  static std::unordered_map<std::string, Creator> &registry() {
    static std::unordered_map<std::string, Creator> instance;
    return instance;
  }
};

template <typename T> struct SensorRegistrar {
  explicit SensorRegistrar(std::string kind) {
    SensorFactory::register_kind(std::move(kind),
                                 [] { return std::make_unique<T>(); });
  }
};

// Wrap these in anonymous namespace so that they are not visible externally.
// We only construct them so that the SensorRegistrar constructor is called,
// registering them in the SensorFactory.
namespace {
SensorRegistrar<TemperatureSensor> temperature_reg{"temperature"};
SensorRegistrar<HumiditySensor> humidit_reg{"humidity"};
SensorRegistrar<Co2Sensor> co2_reg{"co2"};
} // namespace
