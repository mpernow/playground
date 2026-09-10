#include "../observer/observer.hpp"
#include "../reading.hpp"
#include <cmath>
#include <optional>

class AlertRule {
public:
  virtual ~AlertRule() = default;
  virtual bool triggered(const Reading &r) = 0;
  virtual std::string describe() const = 0;
};

class ThresholdRule : public AlertRule {
public:
  ThresholdRule(std::string sensor_name, double threshold);
  bool triggered(const Reading &r) override;
  std::string describe() const override;

private:
  std::string sensor_name_;
  double threshold_;
};

class RateOfChangeRule : public AlertRule {
public:
  RateOfChangeRule(std::string sensor_name, double max_delta);
  bool triggered(const Reading &r) override;
  std::string describe() const override;

private:
  std::string sensor_name_;
  double max_delta_;
  std::optional<double> previous_value;
};

// Use the Strategy pattern to build rule based alerters
class RuleBasedAlerter : public Observer {
public:
  explicit RuleBasedAlerter(std::unique_ptr<AlertRule> rule);

  void on_reading(const Reading &r) override;

private:
  std::unique_ptr<AlertRule> rule_;
};
