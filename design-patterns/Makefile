CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -O2

BINARIES := factory_demo observer_demo aurora

all: $(BINARIES)

factory_demo: factory_demo.cpp factory.cpp
observer_demo: observer_demo.cpp observer.cpp
aurora: aurora.cpp factory.cpp observer.cpp

$(BINARIES):
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(BINARIES)

.PHONY: all clean
