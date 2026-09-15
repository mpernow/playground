#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "factory/factory.hpp"
#include "observer/observer.hpp"
#include "strategy/strategy.hpp"

int main() {
  std::cout << std::fixed << std::setprecision(1);

  // Some rules for alerters
  auto threshold_rule = std::make_unique<ThresholdRule>("co2", 1000.0);
  auto rate_of_change_rule = std::make_unique<RateOfChangeRule>(
      "temperature",
      0.4); // Value chosen so that it is triggered by TemperatureSensor

  SensorHub hub;
  auto dashboard = std::make_shared<ConsoleDashboard>();
  // Both alerters can be created using the same type of Observer
  auto threshold_alerter =
      std::make_shared<RuleBasedAlerter>(std::move(threshold_rule));
  auto rate_of_change_alerter =
      std::make_shared<RuleBasedAlerter>(std::move(rate_of_change_rule));
  hub.subscribe(dashboard);
  hub.subscribe(threshold_alerter);
  hub.subscribe(rate_of_change_alerter);

  std::vector<std::unique_ptr<Sensor>> sensors;
  for (const auto *kind : {"temperature", "humidity", "co2", "pressure"}) {
    sensors.push_back(SensorFactory::create(kind));
  }

  for (int poll_num = 1; poll_num <= 6; ++poll_num) {
    std::cout << "\nPOLLING " << poll_num << '\n';
    for (auto &sensor : sensors) {
      hub.publish(sensor->poll());
    }
  }
}
