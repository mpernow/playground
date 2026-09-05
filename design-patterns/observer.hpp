#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "reading.hpp"

class Observer {
public:
  virtual ~Observer() = default;
  virtual void on_reading(const Reading &r) = 0;
};

// Holds weak_ptrs to its observers. This avoids the dangling-pointer problem a
// raw pointer would have if an Observer were destroyed while still subscribed:
// expired weak_ptrs are pruned, and observers are only touched via a locked
// shared_ptr.
class SensorHub {
public:
  void subscribe(std::weak_ptr<Observer> obs);
  void publish(const Reading &r);

private:
  std::vector<std::weak_ptr<Observer>> observers_;
};

// Variant that stores callables instead of Observer objects. Simpler to use,
// but the caller is responsible for keeping any captured state alive: there
// is no lifetime tracking here.
class SensorHubFunctional {
public:
  using ObserverFunc = std::function<void(const Reading &)>;

  void subscribe(ObserverFunc obs);
  void publish(const Reading &r);

private:
  std::vector<ObserverFunc> observers_;
};

class ConsoleDashboard : public Observer {
public:
  void on_reading(const Reading &r) override;
};

class ThresholdAlerter : public Observer {
public:
  explicit ThresholdAlerter(double threshold);

  void on_reading(const Reading &r) override;

private:
  double threshold_;
};
