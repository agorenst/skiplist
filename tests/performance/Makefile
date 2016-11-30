targets = measure_performance measure_theoretical_claims verify_performance_invariants measure_memory

all: $(targets)

measure_performance : measure_performance.cpp skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< random_device.o -o $@
measure_theoretical_claims : measure_theoretical_claims.cpp skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) -DSKIPLIST_MEASURES $(CXXINCLUDE) $< random_device.o -o $@
verify_performance_invariants : verify_performance_invariants.cpp skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< random_device.o -o $@
measure_memory : measure_memory.cpp noisy_allocator.h skiplist.h random_device.o
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< random_device.o -o $@

random_device.o : random_device.cpp skiplist.h
	$(CXX) $(CXXFLAGS) $(CXXINCLUDE) $< -c -o $@

clean:
	rm $(targets) random_device.o
