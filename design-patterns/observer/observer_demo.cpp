// Observer pattern on its own: the hub does not care where Readings come from,
// so this demo just synthesizes them. Shows weak_ptr pruning and the
// functional variant.

#include <iomanip>
#include <iostream>
#include <memory>

#include "observer.hpp"

int main() {
  std::cout << std::fixed << std::setprecision(1);

  SensorHub hub;
  auto dashboard = std::make_shared<ConsoleDashboard>();
  auto alerter = std::make_shared<ThresholdAlerter>(1000.0);
  hub.subscribe(dashboard);
  hub.subscribe(alerter);

  hub.publish({"co2", 900.0, "ppm"});
  hub.publish({"co2", 1200.0, "ppm"}); // alerter fires

  // Drop an observer; the hub prunes the now-expired weak_ptr on next publish.
  dashboard.reset();
  std::cout << "-- dashboard dropped --\n";
  hub.publish({"co2", 1300.0, "ppm"});

  std::cout << "\n-- functional hub --\n";
  SensorHubFunctional fhub;
  fhub.subscribe([](const Reading &r) {
    std::cout << r.sensor_name << ": " << r.value << " " << r.unit << '\n';
  });
  fhub.subscribe([](const Reading &r) {
    constexpr double threshold = 1000.0;
    if (r.sensor_name == "co2" && r.value > threshold) {
      std::cout << "ALERT: co2 reading " << r.value << " " << r.unit
                << " exceeds threshold " << threshold << " " << r.unit << '\n';
    }
  });
  fhub.publish({"humidity", 40.0, "%RH"});
  fhub.publish({"co2", 1500.0, "ppm"});
}
