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

## Decorator

In the `decorator` directory.

`DashboardDecorator` wraps an inner `Observer` and is itself an `Observer`, so
decorators stack: `TimestampedDashboard` prefixes a running reading count,
`RateLimitedDashboard` only forwards every Nth reading. Neither knows about
the other or about the `ConsoleDashboard` at the bottom of the stack.

- [decorator.hpp](decorator/decorator.hpp) / [decorator.cpp](decorator/decorator.cpp)
  — `DashboardDecorator`, `TimestampedDashboard`, `RateLimitedDashboard`
- [decorator_demo.cpp](decorator/decorator_demo.cpp) — decorator on its own

## Integrated

- [aurora.cpp](aurora.cpp) — factory-built sensors feeding a `SensorHub`
  whose alerting is Strategy-based (`RuleBasedAlerter` with `ThresholdRule`
  and `RateOfChangeRule`) and whose dashboard is a stack of Decorators
  (`RateLimitedDashboard` around `TimestampedDashboard` around
  `ConsoleDashboard`); the plain `ThresholdAlerter` and the functional hub
  are demonstrated in `observer_demo.cpp` instead
- [reading.hpp](reading.hpp) — the `Reading` value type shared by all four
  patterns

## Building

[Makefile](Makefile) — `make` builds `factory_demo`, `observer_demo`,
`strategy_demo`, `decorator_demo` and `aurora`.
