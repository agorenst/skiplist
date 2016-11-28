targets = measure_performance measure_theoretical_claims verify_performance_invariants

all: $(targets)

measure_performance : measure_performance.cpp skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< random_device.o -o $@
measure_theoretical_claims : measure_theoretical_claims.cpp skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< random_device.o -o $@
verify_performance_invariants : verify_performance_invariants.cpp skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< random_device.o -o $@

clean:
	rm $(targets) random_device.o
