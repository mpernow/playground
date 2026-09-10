CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2

BINARIES := factory_demo observer_demo strategy_demo aurora

all: $(BINARIES)

factory_demo: factory/factory_demo.cpp factory/factory.cpp
observer_demo: observer/observer_demo.cpp observer/observer.cpp
strategy_demo: strategy/strategy_demo.cpp strategy/strategy.cpp observer/observer.cpp
aurora: aurora.cpp factory/factory.cpp observer/observer.cpp

$(BINARIES):
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(BINARIES)

.PHONY: all clean
