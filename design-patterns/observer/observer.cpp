#include "observer.hpp"

#include <iostream>
#include <utility>

void SensorHub::subscribe(std::weak_ptr<Observer> obs) {
  observers_.push_back(std::move(obs));
}

void SensorHub::publish(const Reading &r) {
  std::erase_if(observers_,
                [](const std::weak_ptr<Observer> &w) { return w.expired(); });
  for (auto &weak_obs : observers_) {
    if (auto obs = weak_obs.lock()) {
      obs->on_reading(r);
    }
  }
}

void SensorHubFunctional::subscribe(ObserverFunc obs) {
  observers_.push_back(std::move(obs));
}

void SensorHubFunctional::publish(const Reading &r) {
  for (const auto &obs : observers_) {
    obs(r);
  }
}

void ConsoleDashboard::on_reading(const Reading &r) {
  std::cout << r.sensor_name << ": " << r.value << " " << r.unit << '\n';
}

ThresholdAlerter::ThresholdAlerter(double threshold) : threshold_(threshold) {}

void ThresholdAlerter::on_reading(const Reading &r) {
  if (r.sensor_name == "co2") {
    if (r.value > threshold_) {
      std::cout << "ALERT: co2 reading " << r.value << " " << r.unit
                << " exceeds threshold " << threshold_ << " " << r.unit << '\n';
    }
  }
}
