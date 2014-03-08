all: driver test_get_priors

test_get_priors: skiplist.h test_get_priors.cpp
	clang++ -g -std=c++11 -Wall test_get_priors.cpp -o test_get_priors

driver: driver.cpp skiplist.h 
	clang++ -o driver -g -std=c++11 -Wall driver.cpp

unit_tests: unit_tests.cpp skiplist.h
	g++ -o unit_tests -std=c++0x -Wall -I /usr/include/boost unit_tests.cpp

clean:
	rm -f *~ *.o driver test_get_priors
