#include <iostream>
#include <utility>

#include "decorator.hpp"

namespace design_patterns::decorator {

DashboardDecorator::DashboardDecorator(
    std::shared_ptr<observer::Observer> inner)
    : inner_(std::move(inner)) {}

void TimestampedDashboard::on_reading(const Reading &r) {
  std::cout << "[reading " << ++reading_count_ << "] ";
  inner_->on_reading(r);
}

RateLimitedDashboard::RateLimitedDashboard(
    std::shared_ptr<observer::Observer> inner, int keep_every_nth)
    : DashboardDecorator(inner), keep_every_nth_(keep_every_nth) {}

void RateLimitedDashboard::on_reading(const Reading &r) {
  if (count % keep_every_nth_ == 0) {
    inner_->on_reading(r);
  }
  count = (count + 1) % keep_every_nth_;
}

} // namespace design_patterns::decorator
