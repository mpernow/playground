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

class PressureSensor : public Sensor {
public:
  Reading poll() override {
    double value = 900 + tick_ * 1.0;
    ++tick_;
    return {"pressure", value, "hPa"};
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
SensorRegistrar<PressureSensor> pressure_reg{"pressure"};
} // namespace

class Observer {
public:
  virtual ~Observer() = default;
  virtual void on_reading(const Reading &r) = 0;
};

class SensorHub {
  // Note that using weak pointers here avoids the dangling pointer issue that
  // could happen when using raw pointers: If an Observer was destroyed while
  // SensorHub held a raw pointer to it, it would be a dangling pointer.
  // With weak pointers, we can check if they have expired and remove those,
  // as well as only access them if .lock() returns a shared pointer.
public:
  void subscribe(std::weak_ptr<Observer> obs) {
    observers_.push_back(std::move(obs));
  }
  void publish(const Reading &r) {
    std::erase_if(observers_,
                  [](const std::weak_ptr<Observer> &w) { return w.expired(); });
    for (auto &weak_obs : observers_) {
      if (auto obs = weak_obs.lock()) {
        obs->on_reading(r);
      }
    }
  }

private:
  std::vector<std::weak_ptr<Observer>> observers_;
};

class SensorHubFunctional {
  using ObserverFunc = std::function<void(const Reading &)>;

public:
  void subscribe(ObserverFunc obs) { observers_.push_back(std::move(obs)); }

  void publish(const Reading &r) {
    for (auto &obs : observers_) {
      obs(r);
    }
  }

private:
  std::vector<ObserverFunc> observers_;
};

class ConsoleDashboard : public Observer {
public:
  void on_reading(const Reading &r) override {
    std::cout << r.sensor_name << ": " << r.value << " " << r.unit << '\n';
  }
};

class ThresholdAlerter : public Observer {
public:
  explicit ThresholdAlerter(double threshold) : threshold_(threshold) {}

  void on_reading(const Reading &r) override {
    if (r.sensor_name == "co2") {
      if (r.value > threshold_) {
        std::cout << "ALERT: co2 reading " << r.value << " " << r.unit
                  << " exceeds threshold " << threshold_ << " " << r.unit
                  << '\n';
      }
    }
  }

private:
  double threshold_;
};

int main() {
  std::cout << std::fixed << std::setprecision(1);

  SensorHub hub;
  auto dashboard = std::make_shared<ConsoleDashboard>();
  auto alerter = std::make_shared<ThresholdAlerter>(1000.0);
  hub.subscribe(dashboard);
  hub.subscribe(alerter);

  SensorHubFunctional hubFunctional;
  auto dashboardFunctional = [](const Reading &r) {
    std::cout << "[functional] " << r.sensor_name << ": " << r.value << " "
              << r.unit << '\n';
  };
  auto thresholdFunctional = [](const Reading &r) {
    double threshold_ = 1000.0;
    if (r.sensor_name == "co2") {
      if (r.value > threshold_) {
        std::cout << "[functional]" << "ALERT: co2 reading " << r.value << " "
                  << r.unit << " exceeds threshold " << threshold_ << " "
                  << r.unit << '\n';
      }
    }
  };
  hubFunctional.subscribe(dashboardFunctional);
  hubFunctional.subscribe(thresholdFunctional);

  std::vector<std::unique_ptr<Sensor>> sensors;
  sensors.push_back(SensorFactory::create("temperature"));
  sensors.push_back(SensorFactory::create("humidity"));
  sensors.push_back(SensorFactory::create("co2"));
  sensors.push_back(SensorFactory::create("pressure"));

  for (int poll_num = 1; poll_num <= 6; ++poll_num) {
    std::cout << "\nPOLLING " << poll_num << '\n';
    for (auto &sensor : sensors) {
      Reading r = sensor->poll();
      hub.publish(r);
      hubFunctional.publish(r);
    }
  }
}
