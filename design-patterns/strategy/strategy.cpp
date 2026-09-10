#include "strategy.hpp"
#include <iomanip>
#include <iostream>

std::string format_value(double value) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(1) << value;
  return out.str();
}

ThresholdRule::ThresholdRule(std::string sensor_name, double threshold)
    : sensor_name_(std::move(sensor_name)), threshold_(threshold) {}

bool ThresholdRule::triggered(const Reading &r) {
  return r.sensor_name == sensor_name_ && r.value > threshold_;
}

std::string ThresholdRule::describe() const {
  return sensor_name_ + " above " + format_value(threshold_);
}

RateOfChangeRule::RateOfChangeRule(std::string sensor_name, double max_delta)
    : sensor_name_(sensor_name), max_delta_(max_delta) {}

bool RateOfChangeRule::triggered(const Reading &r) {
  if (r.sensor_name != sensor_name_) {
    return false;
  }
  if (!previous_value.has_value()) {
    previous_value = r.value;
    return false;
  }
  double current_previous = previous_value.value();
  previous_value = r.value;
  return std::abs(current_previous - r.value) > max_delta_;
}

std::string RateOfChangeRule::describe() const {
  return sensor_name_ + " has changed by more than " + format_value(max_delta_);
}

RuleBasedAlerter::RuleBasedAlerter(std::unique_ptr<AlertRule> rule)
    : rule_(std::move(rule)) {}

void RuleBasedAlerter::on_reading(const Reading &r) {
  if (rule_->triggered(r)) {
    std::cout << "ALERT [" << rule_->describe() << "]: " << r.sensor_name
              << " = " << r.value << " " << r.unit << '\n';
  }
}
