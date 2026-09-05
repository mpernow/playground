#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "reading.hpp"

class Sensor {
public:
  virtual ~Sensor() = default;
  virtual Reading poll() = 0;
};

class SensorFactory {
public:
  using Creator = std::function<std::unique_ptr<Sensor>()>;

  static void register_kind(std::string kind, Creator creator);

  // Throws std::invalid_argument if the kind was never registered.
  static std::unique_ptr<Sensor> create(const std::string &kind);

private:
  static std::unordered_map<std::string, Creator> &registry();
};

// Construct one of these (typically as a namespace-scope object) to register a
// sensor type under a kind name. Its constructor runs at static-init time.
template <typename T> struct SensorRegistrar {
  explicit SensorRegistrar(std::string kind) {
    SensorFactory::register_kind(std::move(kind),
                                 [] { return std::make_unique<T>(); });
  }
};
