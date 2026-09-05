# Design Patterns

Code illustrating design patterns in C++.

## Factory

Sensors are created from a string key. Concrete sensor types stay private to
the implementation and self-register with the factory at static-init time.

- [factory.hpp](factory.hpp) / [factory.cpp](factory.cpp) — `Sensor`,
  `SensorFactory`, `SensorRegistrar`
- [factory_demo.cpp](factory_demo.cpp) — factory on its own

## Observer

A hub fans `Reading`s out to subscribers. Two variants: [`SensorHub`](observer.hpp)
holds `weak_ptr`s and prunes expired ones; `SensorHubFunctional` holds
`std::function`s.

- [observer.hpp](observer.hpp) / [observer.cpp](observer.cpp) — `Observer`,
  `SensorHub`, `SensorHubFunctional`, `ConsoleDashboard`, `ThresholdAlerter`
- [observer_demo.cpp](observer_demo.cpp) — observer on its own

## Integrated

- [aurora.cpp](aurora.cpp) — factory-built sensors feeding both hubs
- [reading.hpp](reading.hpp) — the `Reading` value type shared by both patterns

## Building

[Makefile](Makefile) — `make` builds `factory_demo`, `observer_demo` and
`aurora`.
