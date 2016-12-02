#include "Catch/single_include/catch.hpp"

//#define LOGGING_INFO
#define SKIPLIST_DEBUG
#include "skiplist.h"
#include <algorithm>
#include <set>
#include <iostream>

// This complements the small, exhaustive tests we have in other files.
// This does a larger series of inserts and removals and makes sure
// that on each operation the skip_list still looks like a set.
//
// We output the random seed for repeatability, in the case that this
// random test does in fact fail.

using namespace std;


const int N = 10000;
typedef skip_list<int> test_type;
TEST_CASE("Test over small range") {
    auto seed = skiplist_internal::rd();
    cout << "Random seed: " << seed << endl;
    skiplist_internal::gen.seed(seed);
    const int N = 17;
    const int M = 17;
    test_type l;
    set<int> s;
    for (int i = 0; i < N; ++i) {
        int value = skiplist_internal::dis(skiplist_internal::gen) % N;
        int height = std::min(30,skiplist_internal::good_height_generator())+1;
        int do_insert = skiplist_internal::dis(skiplist_internal::gen) % M;
        //cout << value << " " << height << " " << do_insert << endl;
        if (do_insert) {
            l.insert(value, height);
            s.insert(value);
        }
        else {
            l.erase(value);
            s.erase(value);
        }
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
}
TEST_CASE("Test over large range") {
    auto seed = skiplist_internal::rd();
    cout << "Random seed: " << seed << endl;
    skiplist_internal::gen.seed(seed);
    const int N = 15485867;
    const int M = 17;
    test_type l;
    set<int> s;
    for (int i = 0; i < 1000; ++i) {
        int value = skiplist_internal::dis(skiplist_internal::gen) % N;
        int height = std::min(30,skiplist_internal::good_height_generator())+1;
        int do_insert = skiplist_internal::dis(skiplist_internal::gen) % M;
        //cout << value << " " << height << " " << do_insert << endl;
        if (do_insert) {
            l.insert(value, height);
            s.insert(value);
        }
        else {
            l.erase(value);
            s.erase(value);
        }
        REQUIRE(equal(begin(l), end(l), begin(s), end(s)));
    }
}
