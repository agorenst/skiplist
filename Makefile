tests: skiplist_test
#tests: basic_set_interface exhaustive_testing skiplist_test

basic_set_interface: basic_set_interface.cpp skiplist.h
	clang++ -g -Wall -std=c++14 basic_set_interface.cpp -o basic_set_interface
exhaustive_testing: exhaustive_testing.cpp skiplist.h
	clang++ -O2 -g -Wall -std=c++14 exhaustive_testing.cpp -o exhaustive_testing
skiplist_test: skiplist_test.cpp skiplist.h
	clang++ -O2 -g -Wall -std=c++14 skiplist_test.cpp -o skiplist_test
clean:
	rm skiplist_test basic_set_interface exhaustive_testing
