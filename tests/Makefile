CXX = clang++
CXXFLAGS = -std=c++14 -Wall -O3 -g

all: measure_theoretical_claims

measure_theoretical_claims: measure_theoretical_claims.cpp ./../skiplist.h
	$(CXX) $(CXXFLAGS) -I./../ measure_theoretical_claims.cpp -o measure_theoretical_claims
measure_performance: measure_performance.cpp ./../skiplist.h
	$(CXX) $(CXXFLAGS) -I./../ measure_performance.cpp -o measure_performance
correctness_checking_rnd: correctness_checking_rnd.cpp ./../skiplist.h
	$(CXX) $(CXXFLAGS) -I./../ correctness_checking_rnd.cpp -o correctness_checking_rnd
set_modifiers: set_modifiers.cpp ./../skiplist.h
	$(CXX) $(CXXFLAGS) -I./../ set_modifiers.cpp -o set_modifiers

clean:
	rm -r -f correctness_checking_rnd set_modifiers measure_performance measure_theoretical_claims
