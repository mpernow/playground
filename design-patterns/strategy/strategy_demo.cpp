#include "strategy.hpp"

int main() {
  SensorHub hub;
  auto threshold_rule = std::make_unique<ThresholdRule>("co2", 1000.0);
  auto rate_of_change_rule =
      std::make_unique<RateOfChangeRule>("temperature", 5.0);

  auto threshold_alerter =
      std::make_shared<RuleBasedAlerter>(std::move(threshold_rule));
  auto rate_of_change_alerter =
      std::make_shared<RuleBasedAlerter>(std::move(rate_of_change_rule));
  hub.subscribe(threshold_alerter);
  hub.subscribe(rate_of_change_alerter);

  hub.publish({"co2", 900.0, "ppm"});
  hub.publish({"co2", 1200.0, "ppm"});   // threshold_alerter fires
  hub.publish({"other", 1200.0, "ppm"}); // threshold_alerter doesn't fire

  hub.publish({"temperature", 10.0, "C"});
  hub.publish({"temperature", 12.0, "C"});
  hub.publish({"temperature", 17.5, "C"});
  hub.publish({"temperature", 17.0, "C"});
  hub.publish({"temperature", 11.0, "C"});
  hub.publish({"temperature", 10.0, "C"});
}
