#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "factory/factory.hpp"
#include "observer/observer.hpp"

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
    constexpr double threshold = 1000.0;
    if (r.sensor_name == "co2" && r.value > threshold) {
      std::cout << "[functional] ALERT: co2 reading " << r.value << " "
                << r.unit << " exceeds threshold " << threshold << " " << r.unit
                << '\n';
    }
  };
  hubFunctional.subscribe(dashboardFunctional);
  hubFunctional.subscribe(thresholdFunctional);

  std::vector<std::unique_ptr<Sensor>> sensors;
  for (const auto *kind : {"temperature", "humidity", "co2", "pressure"}) {
    sensors.push_back(SensorFactory::create(kind));
  }

  for (int poll_num = 1; poll_num <= 6; ++poll_num) {
    std::cout << "\nPOLLING " << poll_num << '\n';
    for (auto &sensor : sensors) {
      Reading r = sensor->poll();
      hub.publish(r);
      hubFunctional.publish(r);
    }
  }
}
