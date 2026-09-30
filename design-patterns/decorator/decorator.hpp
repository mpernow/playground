#pragma once

#include <memory>

#include "../observer/observer.hpp"
#include "../reading.hpp"

namespace design_patterns::decorator {

// The DashboardDecorator holds a pointer to an inner Observer, and is also an
// Observer itself
class DashboardDecorator : public observer::Observer {
public:
  explicit DashboardDecorator(std::shared_ptr<observer::Observer> inner);

protected:
  std::shared_ptr<observer::Observer> inner_;
};

class TimestampedDashboard : public DashboardDecorator {
public:
  using DashboardDecorator::DashboardDecorator; // take in the constructor
  void on_reading(const Reading &r) override;

private:
  int reading_count_ = 0;
};

class RateLimitedDashboard : public DashboardDecorator {
public:
  RateLimitedDashboard(std::shared_ptr<observer::Observer> inner,
                       int keep_every_nth);
  void on_reading(const Reading &r) override;

private:
  int keep_every_nth_ = 0;
  int count = 0;
};

} // namespace design_patterns::decorator
