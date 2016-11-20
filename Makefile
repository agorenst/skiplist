tests: benchmarks_test basic_set_interface exhaustive_testing

basic_performance_checks: basic_performance_checks.cpp
	clang++ -g -Wall -O2 -std=c++14 basic_performance_checks.cpp -o basic_performance_checks

verify_theoretical_properties: skiplist.h verify_theoretical_properties.cpp
	clang++ verify_theoretical_properties.cpp -O2 -Wall -std=c++14 -o verify_theoretical_properties

benchmarks_test: benchmarks_test.cpp skiplist.h
	clang++ -g -Wall -O4 -std=c++14 benchmarks_test.cpp -o core_test

basic_set_interface: basic_set_interface.cpp skiplist.h
	clang++ -g -Wall -std=c++14 basic_set_interface.cpp -o basic_set_interface
exhaustive_testing: exhaustive_testing.cpp skiplist.h
	clang++ -O2 -g -Wall -std=c++14 exhaustive_testing.cpp -o exhaustive_testing
skiplist_test: skiplist_test.cpp skiplist.h
	clang++ -O2 -g -Wall -std=c++14 skiplist_test.cpp -o skiplist_test
clean:
	rm skiplist_test basic_set_interface exhaustive_testing benchmarks_test rbtree_test skiplist_test verify_theoretical_properties core_test basic_performance_checks
