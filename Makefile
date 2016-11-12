skiplist_test: skiplist_test.cpp skiplist.h
	clang++ -O2 -g -Wall -std=c++14 skiplist_test.cpp -o skiplist_test
clean:
	rm skiplist_test
