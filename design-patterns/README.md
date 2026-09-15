# Design Patterns

Code illustrating design patterns in C++.

## Factory

In the `factory` directory.

Sensors are created from a string key. Concrete sensor types stay private to
the implementation and self-register with the factory at static-init time.

- [factory.hpp](factory/factory.hpp) / [factory.cpp](factory/factory.cpp) —
  `Sensor`, `SensorFactory`, `SensorRegistrar`
- [factory_demo.cpp](factory/factory_demo.cpp) — factory on its own

## Observer

In the `observer` directory.

A hub fans `Reading`s out to subscribers. Two variants:
[`SensorHub`](observer/observer.hpp) holds `weak_ptr`s and prunes expired
ones; `SensorHubFunctional` holds `std::function`s.

- [observer.hpp](observer/observer.hpp) / [observer.cpp](observer/observer.cpp)
  — `Observer`, `SensorHub`, `SensorHubFunctional`, `ConsoleDashboard`,
  `ThresholdAlerter`
- [observer_demo.cpp](observer/observer_demo.cpp) — observer on its own

## Strategy

In the `strategy` directory.

`RuleBasedAlerter` is an `Observer` that delegates the alerting decision to an
`AlertRule` strategy, swappable at construction time: `ThresholdRule` fires
above a fixed value, `RateOfChangeRule` fires on a large jump between
consecutive readings.

- [strategy.hpp](strategy/strategy.hpp) / [strategy.cpp](strategy/strategy.cpp)
  — `AlertRule`, `ThresholdRule`, `RateOfChangeRule`, `RuleBasedAlerter`
- [strategy_demo.cpp](strategy/strategy_demo.cpp) — strategy on its own

## Integrated

- [aurora.cpp](aurora.cpp) — factory-built sensors feeding both hubs,
  including a `RuleBasedAlerter`
- [reading.hpp](reading.hpp) — the `Reading` value type shared by all three
  patterns

## Building

[Makefile](Makefile) — `make` builds `factory_demo`, `observer_demo`,
`strategy_demo` and `aurora`.
