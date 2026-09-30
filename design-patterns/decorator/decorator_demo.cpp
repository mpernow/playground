#include <iomanip>
#include <iostream>
#include <memory>

#include "decorator.hpp"

int main() {

  std::cout << std::fixed << std::setprecision(1);

  design_patterns::observer::SensorHub hub;
  auto dashboard =
      std::make_shared<design_patterns::observer::ConsoleDashboard>();
  auto timestamped_dashboard =
      std::make_shared<design_patterns::decorator::TimestampedDashboard>(
          dashboard);
  auto rate_limited_dashboard =
      std::make_shared<design_patterns::decorator::RateLimitedDashboard>(
          timestamped_dashboard, 3);
  hub.subscribe(rate_limited_dashboard);
  // If we reverse the order, the timestamp will tick up on every reading, but
  // rate limiter will only forward to ConsoleDashboard every nth time that it
  // is reached

  hub.publish({"co2", 900.0, "ppm"});
  hub.publish({"co2", 1000.0, "ppm"});
  hub.publish({"co2", 1100.0, "ppm"});
  hub.publish({"co2", 1200.0, "ppm"});
  hub.publish({"co2", 1300.0, "ppm"});
  hub.publish({"co2", 1400.0, "ppm"});
  hub.publish({"co2", 1500.0, "ppm"});
}
